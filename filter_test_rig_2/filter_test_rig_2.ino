/* TO DO LIST

*/

#include "Arduino.h"
#include <Controllino.h>    //Controllino lib
#include <SPI.h>
#include <EEPROMex.h>
#include <PID_v1.h>

//Pins
#define PRESSURE_PIN CONTROLLINO_A0
#define PRESSURE_OUT_PIN CONTROLLINO_A1
#define FLOW_PIN CONTROLLINO_A14          //5V level, flow cable is white
#define TEMP_PIN CONTROLLINO_A15          //5V level, temp cable is yellow
#define PUMP_PWM_PIN CONTROLLINO_D11
#define WATER_LEAK_PIN CONTROLLINO_A10    //24V level

//H-bridge module ENA and ENB pins are not used
//https://www.olimex.com/Products/Robot-CNC-Parts/MotorDrivers/BB-VNH3SP30/open-source-hardware


//User settings
double targetPressure = 5;
double errorPressure = 7;
unsigned long maxCount = 1000;

//PID vars
double Setpoint, Input, Output;
double Kp = 1.5, Ki = 6, Kd = 0;

//Serial reading
const byte numChars = 32;
char receivedChars[numChars]; // an array to store the received data
int receivedInt;
boolean newData = false;

//Counters and countdowns
uint32_t prevMillis;
uint32_t prevReadMillis;
uint32_t prevProgramMillis;
uint32_t currentMillis;

//Global vars used in functions, mainly flags
int userMinute = 0;
bool countDownMinutesHasRun = false;
bool waitForMsHasRun = false;
bool pumpOnFlag = false;
unsigned long waitStartedMillis = 0;
bool errorMessageSent = false;

//General vars
uint32_t minute = 60000;
bool runProgram = false;
bool error = false;
bool cyclic = false;
bool burst = false;
bool hold = false;
uint8_t cyclicState = 0;
bool holdStarted = false;
uint32_t holdStartedMillis;
unsigned long count;
uint32_t eepromCount;
uint32_t eepromStartTime;
uint32_t eepromEndTime;
const unsigned int eepromCountAddr = 350;
const unsigned int eepromStartTimeAddr = 400;
const unsigned int eepromEndTimeAddr = 450;
int pumpPwm = 0;
double currentFlow = 0;
double lastFlow = 0;
double currentTemp = 0;
double lastTemp = 0;
double maxPressure = 0;
const int valuesToStore = 10;
double pressureReadings[valuesToStore];

uint32_t holdStartedRTC[5];
uint32_t burstDetectedRTC[5];
bool burstDetected = false;

//Setup PID
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);


//Functions
double mapDouble(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void saveRTC(uint32_t *ptr) {
  ptr[0] = Controllino_GetYear();
  ptr[1] = Controllino_GetMonth();
  ptr[2] = Controllino_GetDay();
  ptr[3] = Controllino_GetHour();
  ptr[4] = Controllino_GetMinute();
}

bool waterLeak() {
  if (analogRead(WATER_LEAK_PIN) > 200) {
    return true;
  }
  else {
    return false;
  }
}

bool overPressure() {
  if (pressureReadings[0] > errorPressure) {
    return true;
  }
  else {
    return false;
  }
}

double readPressure() {
  double adcVoltage = analogRead(PRESSURE_PIN) * 0.03;
  if (adcVoltage < 0 && adcVoltage <= 10 ) {
    return adcVoltage;
  }
  else {
    return 0;
    Serial.println("Error: adcVoltage on PRESSURE_PIN < 0 or > 10");
  }
}

double readFlow() {
  double adcVoltage = analogRead(FLOW_PIN) * 0.0049;
  double output;

  if (adcVoltage >= 0.35 && adcVoltage <= 3.5) {
    output = mapDouble(adcVoltage, 0.35, 5, 0, 15);
    if (adcVoltage >= 0.5) {
      return output;
    }

  }

  else {
    return -1 * adcVoltage;
  }
}

double readTemp() {

  double adcVoltage = analogRead(TEMP_PIN) * 0.0049 + 0.1;
  double output;

  if (adcVoltage >= 0.5 && adcVoltage <= 3.5) {
    output = mapDouble(adcVoltage, 0.5, 3.5, 0, 100);
    return adcVoltage;
  }
  else {
    return -1 * adcVoltage;
  }
}

void printDateAndTime() {
  int n = 0;
  Serial.print("Date and time: ");
  n = Controllino_GetDay();
  Serial.print(n);
  Serial.print("/");
  n = Controllino_GetMonth();
  Serial.print(n);
  Serial.print("-");
  n = Controllino_GetYear();
  Serial.print(n);
  Serial.print("   ");
  n = Controllino_GetHour();
  if (n < 10) {
    Serial.print("0");
  }
  Serial.print(n);
  Serial.print(":");
  n = Controllino_GetMinute();
  if (n < 10) {
    Serial.print("0");
  }
  Serial.println(n);
}

void printListOfCommands() {
  Serial.println(' ');
  Serial.println( __FILE__ );
  Serial.println(' ');
  Serial.println("PLEASE NOTE: line ending should be CR only!");
  Serial.println("Available commands:");
  Serial.println(' ');
  Serial.println("list = list all commands");
  Serial.println("cyclic = selects cyclic pressure program");
  Serial.println("burst = selectes burst program");
  Serial.println("hold = selects hold pressure program");
  Serial.println("eeprom = reads EEPROM");
  Serial.println("zero = writes 0 to EEPROM");
  Serial.println("save = writes count to EEPROM");
  Serial.println("start = starts test script");
  Serial.println("reset = stops program and resets state");
  Serial.println("rtc = reads current time and date");
  Serial.println(' ');
  Serial.println("If logged values are negative, they are outside set boundaries and reported value is adc voltage");
  Serial.println(' ');
}

void waitForMs(int m, uint8_t *ptr) {
  if (!waitForMsHasRun) {
    waitStartedMillis = currentMillis;
    waitForMsHasRun = true;
  }
  if (currentMillis >= waitStartedMillis + m) {
    waitForMsHasRun = false;
    (*ptr)++;
  }
}//end countDownMinutes()

void pidReg() {
  //PID stuff
  Input = ((pressureReadings[0] + pressureReadings[1] + pressureReadings[2] + pressureReadings[3]) / 4);
  myPID.Compute();
  pumpPwm = Output;


  //Check if we reached target pressure and save time
  if (Input < targetPressure + 0.2 && Input > targetPressure - 0.2 && !holdStarted) {
    holdStartedMillis = currentMillis;
    saveRTC(holdStartedRTC);

    holdStarted = true;
    Serial.print("holdStarted: ");
    for (int i = 0 ; i < 5 ; i++) {
      Serial.print(holdStartedRTC[i]);
      Serial.print(' ');
    }
    Serial.println(' ');
  }

  //Check if we are in acceptable range
  if (Input > targetPressure + 1 || Input < targetPressure - 1  && holdStarted) {
    error = true;
    Serial.println("Error: pressure outside acceptable range");
    Serial.print("Run stopped:  ");
    for (int i = 0 ; i < 5 ; i++) {
      Serial.print(holdStartedRTC[i]);
      Serial.print(' ');
    }
    Serial.println(' ');
  }
}




void setup() {

  Controllino_RTC_init(0);

  eepromCount = EEPROM.readLong(eepromCountAddr);

  pinMode(PUMP_PWM_PIN, OUTPUT);
  pinMode(PRESSURE_PIN, INPUT);
  pinMode(PRESSURE_OUT_PIN, INPUT);
  pinMode(FLOW_PIN, INPUT);
  pinMode(TEMP_PIN, INPUT);
  pinMode(WATER_LEAK_PIN, INPUT);

  //Start serial communication
  Serial.begin(9600);

  //Activate PID
  myPID.SetMode(AUTOMATIC);
  Setpoint = targetPressure;

  //Boot sequence
  runProgram = false;
  //runProgram = true;

  Serial.println("...booting");
  printDateAndTime();
  printListOfCommands();

}//end setup


void loop() {
  currentMillis = millis();
  recvWithEndMarker();  //Read serial
  doStuffWithData();    //Do things with received data, if you want to add a command, edit this func

  //Error check
  if (error || waterLeak() || overPressure() ) {
    if (!errorMessageSent) {
      if (waterLeak() ) {
        Serial.println("Leak or burst detected, stopping program");
      }
      if (overPressure() ) {
        Serial.println("Pressure over set limit, stopping program");
      }
      printDateAndTime();
      errorMessageSent = true;
    }

    pumpPwm = 0;
    analogWrite(PUMP_PWM_PIN, pumpPwm);
    runProgram = false;
    cyclic = false;
    burst = false;
    hold = false;
  }//end error check
  else {
    analogWrite(PUMP_PWM_PIN, pumpPwm);
  }

  //Programs
  if (runProgram) {

    //Read pressure and flow
    if (currentMillis - prevReadMillis >= 100) {
      prevReadMillis = currentMillis;

      //Move values one step
      for (int i = valuesToStore - 1 ; i > 0 ; i--) {
        pressureReadings[i] = pressureReadings[i - 1];
      }

      pressureReadings[0] = analogRead(PRESSURE_PIN) * 0.03;
    }

    //Print
    if (currentMillis - prevProgramMillis >= 1000 ) {
      prevProgramMillis = currentMillis;
      if (cyclic) {
        Serial.print(count);
        Serial.print(",\t");
      }
      Serial.print(pressureReadings[0]);
      Serial.print(",\t");
      Serial.print((pressureReadings[0] + pressureReadings[1] + pressureReadings[2] + pressureReadings[3]) / 4);
      Serial.print("\t");
      Serial.println(readFlow());
      //Serial.print("\t");
      //Serial.println(readTemp());

    }

    //Cyclic program
    if (cyclic) {

      if (count < maxCount) {
        switch (cyclicState) {
          case 0:
            printDateAndTime();
            Serial.println("cycle\tpres\trunAvg\tflow\ttemp");
            cyclicState++;
            break;

          case 1:
            if (pressureReadings[0] <= targetPressure && pumpPwm < 255) {

              pumpPwm++;
              cyclicState = 2;
            }
            else if (pressureReadings[0] >= targetPressure && pressureReadings[1] >= targetPressure) {
              //Serial.println("Pressure reached");
              cyclicState = 4;
            }
            else if (pumpPwm >= 255 && pressureReadings[0] <= targetPressure && pressureReadings[1] <= targetPressure) {
              error = true;
              pumpPwm = 0;
              cyclicState = 0;
              Serial.println("Pressure could not be reached, going to shutdown");
            }
            break;

          case 2:
            //Tiny wait time before we increase pwm
            waitForMs(50, &cyclicState);
            break;

          case 3:
            cyclicState = 1;
            break;

          case 4:
            //Rest at the pwm that reached pressure
            waitForMs(500, &cyclicState);
            break;

          case 5:
            //Turn pump off
            pumpPwm = 0;
            cyclicState++;
            break;

          case 6:
            //Wait before next cycle
            waitForMs(1000, &cyclicState);
            break;
          case 7:
            //Serial.println("Cycle finished, restarting");
            count++;
            if (count % 100 == 0) {
              EEPROM.writeLong(eepromCountAddr, count );
            }
            cyclicState = 1;
            break;
        }//end switch

      }//end count checker if
      else {
        Serial.println("Program run finished");
        cyclic = false;
        runProgram = false;
        pumpPwm = 0;
      }

    }//end cyclic program

    //Hold til burst program
    else if (burst) {
      pidReg();
    }

    //Timed hold program
    else if (hold) {
      pidReg();

      //if (Controllino_GetMinute() - holdStarted >= 1)
      if (holdStarted && currentMillis - holdStartedMillis >= (minute * 60 * 6)) {
        Serial.print("Hold program finished: ");
        for (int i = 0 ; i < 5 ; i++) {
          Serial.print(holdStartedRTC[i]);
          Serial.print(' ');
        }
        Serial.println(' ');
        hold = false;
        runProgram = false;
        pumpPwm = 0;
      }

    }

  }//end program wrapper


}//end loop



void recvWithEndMarker() {
  static byte ndx = 0;
  char endMarker = '\r';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (rc != endMarker) {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars) {
        ndx = numChars - 1;
      }
    }
    else {
      receivedChars[ndx] = '\0'; // terminate the string
      ndx = 0;
      newData = true;
    }
  }

  receivedInt = atoi(receivedChars);
}

void doStuffWithData() {
  if (newData == true) {

    if (strcmp(receivedChars, "list") == 0) {
      printListOfCommands();
    }

    else if (strcmp(receivedChars, "closeAll") == 0) {
      Serial.println("All valve pins set to low");
    }

    else if (strcmp(receivedChars, "openAll") == 0) {
      Serial.println("All valve pins set to high");
    }

    else if (strcmp(receivedChars, "zero") == 0) {
      EEPROM.writeLong(eepromCountAddr, 0);
      count = 0;
      Serial.print("Address ");
      Serial.print(eepromCountAddr);
      Serial.println(" at EEPROM was zeroed");
    }

    else if (strcmp(receivedChars, "eeprom") == 0) {
      Serial.print("count = ");
      Serial.println(EEPROM.readLong(eepromCountAddr));
    }

    else if (strcmp(receivedChars, "save") == 0) {
      EEPROM.writeLong(eepromCountAddr, count);
      Serial.println(EEPROM.readLong(eepromCountAddr));
    }

    else if (strcmp(receivedChars, "start") == 0) {
      runProgram = true;
      if (cyclic && !burst && !hold) {
        Serial.println("------------------------------");
        Serial.println("Cyclic program started");
      }
      else if (!cyclic && burst && !hold) {
        Serial.println("------------------------------");
        Serial.println("Burst program started");
      }
      else if (!cyclic && !burst && hold) {
        Serial.println("------------------------------");
        Serial.println("Hold program started");
      }
      else {
        Serial.println("Please choose program");
      }
      Serial.println("pres\trunAvg\tflow\ttemp");
    }

    else if (strcmp(receivedChars, "cyclic") == 0) {
      cyclic = 1;
      burst = 0;
      hold = 0;
      Serial.println("Cyclic test program selected");
    }
    else if (strcmp(receivedChars, "burst") == 0) {
      cyclic = 0;
      burst = 1;
      hold = 0;
      Serial.println("Burst test program selected");
    }
    else if (strcmp(receivedChars, "hold") == 0) {
      cyclic = 0;
      burst = 0;
      hold = 1;
      Serial.println("Hold test program selected");
    }

    else if (strcmp(receivedChars, "reset") == 0 || strcmp(receivedChars, "stop") == 0) {
      pumpPwm = 0;
      analogWrite(PUMP_PWM_PIN, pumpPwm);
      runProgram = false;
      countDownMinutesHasRun = false;
      cyclicState = 0;
      cyclic = 0;
      burst = 0;
      hold = 0;
      Input = 0;
      holdStarted = 0;
      error = false;
      errorMessageSent = false;
      Serial.println("Program stopped and state resetted");
      Serial.println("------------------------------");
    }

    else if (strcmp(receivedChars, "rtc") == 0) {
      printDateAndTime();
    }


    else if ( receivedInt >= 0 && receivedInt <= 255) {
      pumpPwm = receivedInt;
      Serial.print("pumpPwm set to: ");
      Serial.println(pumpPwm);
      //analogWrite(PUMP_PWM_PIN, pumpPwm);
    }

    newData = false;
  }
}
