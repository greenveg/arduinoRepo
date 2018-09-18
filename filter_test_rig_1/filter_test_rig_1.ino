#include "Arduino.h"
#include <Controllino.h>    //Controllino lib
#include <SPI.h>
#include <EEPROMex.h>

//Pins
#define PRESSURE_PIN CONTROLLINO_A0
#define FLOW_PIN CONTROLLINO_A14          //Flow cable is white
#define TEMP_PIN CONTROLLINO_A15          //Temp cable is yellow
#define PUMP_PWM_PIN CONTROLLINO_D11      //5V level
#define PUMP_INA_PIN CONTROLLINO_D7       //5V level
#define PUMP_INB_PIN CONTROLLINO_D9       //5V level

//H-bridge module ENA and ENB pins are not used
//https://www.olimex.com/Products/Robot-CNC-Parts/MotorDrivers/BB-VNH3SP30/open-source-hardware



//Serial reading
const byte numChars = 32;
char receivedChars[numChars]; // an array to store the received data
int receivedInt;
boolean newData = false;

//Counters and countdowns
unsigned long previousMillis = 0;
unsigned long previousReadMillis = 0;
unsigned long currentMillis = 0;
unsigned long waitStartedMillis = 0;

//Global vars used in functions, mainly flags
int userMinute = 0;
bool countDownMinutesHasRun = false;
bool waitForMsHasRun = false;
bool rampToPressureHasRun = true;
bool pumpOnFlag = false;

//Script vars
unsigned long minute = 60000;
uint8_t state = 0;
uint8_t errorState = 99;
unsigned long count = 0;
unsigned long maxCount = 10;
uint8_t eepromCount = 0;
const unsigned int eepromCountAddr = 350;
unsigned long pumpStartedMillis = 0;
bool runProgram = false;
int pumpPwm = 0;
double currentPressure = 0;
double lastPressure = 0;
double currentFlow = 0;
double lastFlow = 0;
double currentTemp = 0;
double lastTemp = 0;

//double pressure[10] = {1.19, 2,37

double maxPressure = 3.5;
const uint16_t restAtPressureMs = 1000;


//Functions
double readPressure() {
  double adcVoltage = analogRead(PRESSURE_PIN)*0.03;
  if (adcVoltage < 0 && adcVoltage <= 10 ) {
    return adcVoltage;
  }
  else {
    return 0;
    Serial.println("Error: adcVoltage on PRESSURE_PIN < 0 or > 10");
  }
}


double readFlow() {
  double adcVoltage = analogRead(FLOW_PIN)*0.03;
  if (adcVoltage > 0 && adcVoltage < 3.5) {
    double output = map(adcVoltage, 0.5, 3.5, 1, 12);
    return output;
  }
  else {
    Serial.println("Error: adcVoltage on FLOW_PIN < 0 or >3.5");
    return 0;
  } 
}

double readTemp() {
  double adcVoltage = analogRead(TEMP_PIN)*0.03;
  if (adcVoltage > 0 && adcVoltage < 3.5) {
    double output = map(adcVoltage, 0.5, 3.5, 1, 12);
    return output;
  }
  else {
    Serial.println("Error: adcVoltage on TEMP_PIN < 0 or >3.5");
    return 0;
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
  Serial.print(n);
  Serial.print(":");
  n = Controllino_GetMinute();
  Serial.print(n);
}

void printListOfCommands() {
  Serial.println(' ');
  Serial.println("PLEASE NOTE: line ending should be CR only!");
  Serial.println("Available commands:");
  Serial.println(' ');
  Serial.println("list = list all commands");
  Serial.println("pump = turns pump on or off");
  Serial.println("eeprom = reads address 350 on EEPROM");
  Serial.println("zero = writes 0 to address 350 on EEPROM");
  Serial.println("save = writes count to address 350 on EEPROM");
  Serial.println("start = starts test script");
  Serial.println("reset = stops program and resets state");
  Serial.println("rtc = reads current time and date");
  Serial.println("++ = goes to next state in program");
  Serial.println(' ');
}

void countDownMinutes(int m, String message) {
  if (!countDownMinutesHasRun) {
    userMinute = m;
    waitStartedMillis = currentMillis;
    previousMillis = waitStartedMillis;
    //Serial.print(userMinute);
    //ssstSerial.println(message);
    userMinute--;
    countDownMinutesHasRun = true;
  }

  if (currentMillis >= waitStartedMillis + m * minute) {
    countDownMinutesHasRun = false;
    //Serial.println("state++");
    state++;
  }
  else if (userMinute > 0 && currentMillis >= previousMillis + minute) {
    previousMillis = currentMillis;
    //Serial.print(userMinute);
    //stSerial.println(message);
    userMinute--;
  }
}//end countDownMinutes()


void waitForMs(int m) {
  if (!waitForMsHasRun) {
    waitStartedMillis = currentMillis;
    waitForMsHasRun = true;
  }
  if (currentMillis >= waitStartedMillis + m) {
    waitForMsHasRun = false;
    state++;
  }
}//end countDownMinutes()



void setup() {
  Controllino_RTC_init(0);

  eepromCount = EEPROM.read(4);

  pinMode(PUMP_PWM_PIN, OUTPUT);
  pinMode(PUMP_INA_PIN, OUTPUT);
  pinMode(PUMP_INB_PIN, OUTPUT);
  pinMode(PRESSURE_PIN, INPUT);
  pinMode(FLOW_PIN, INPUT);

  // CW
  digitalWrite(PUMP_INA_PIN, LOW);
  digitalWrite(PUMP_INB_PIN, HIGH);
  
  //Start serial communication
  Serial.begin(9600);

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

  //Program
  if (runProgram) {
    if (currentMillis - previousReadMillis >= 500) {
      previousReadMillis = currentMillis;
      //Serial.println("Reading sensors");

      lastPressure = currentPressure;
      //lastTemp = currentTemp;
      //lastFlow = currentFlow;

      currentPressure = analogRead(PRESSURE_PIN)*0.03;
      //currentTemp = readTemp();
      //currentFlow= readFlow();
      
      Serial.print(count);
      Serial.print(",");
      Serial.print(currentPressure);
      Serial.print(",");
      Serial.println(pumpPwm);
      /*
      Serial.print(analogRead(TEMP_PIN)*0.03);
      Serial.print(",");
      Serial.println(analogRead(FLOW_PIN)*0.03);
      */
    }


    if (count < maxCount) {
      
      
      switch (state) {
        case 0:
          printDateAndTime();
          Serial.println(' ');
          Serial.print("Cycle ");
          Serial.print(count + 1);
          Serial.print("/");
          Serial.println(maxCount);

          state++;
          break;  

        case 1:
          pumpStartedMillis = currentMillis;  
          analogWrite(PUMP_PWM_PIN, 50);
          state++;
          break;

        case 2:
          if (currentPressure <= maxPressure && pumpPwm < 255) {
            pumpPwm++;
            analogWrite(PUMP_PWM_PIN, pumpPwm);
            state = 3;
          }
          else if (pumpPwm >= 255 && currentPressure <= maxPressure && lastPressure <= maxPressure) {
            Serial.println("Pressure could not be reached, going to error state");
            pumpPwm = 0;
            analogWrite(PUMP_PWM_PIN, pumpPwm);
            state = 99; //error state
          }
          else if (currentPressure >= maxPressure && lastPressure >= maxPressure) {
            state = 5;
          }
          break;
          
        case 3:
          waitForMs(100);
          break;
          
        case 4:
          state = 2;
          break;
          
        case 5:
          waitForMs(restAtPressureMs);
          break;
          
        case 6:
          Serial.println("Pressure reached, shutting down");
          pumpPwm = 0;
          analogWrite(PUMP_PWM_PIN, pumpPwm);
          state++;
          break;
          
        case 7:
          waitForMs(1000);
          break;
        case 8:
          Serial.println("Cycle finished, restarting");
          count++;
          state = 2;
          break;
        case 99:
          
          break; 

      }//end switch
      
    }//end count checker if
    else {
      Serial.println("Program run finished");
      runProgram = false;
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

    else if (strcmp(receivedChars, "pump") == 0) {
      if (pumpOnFlag == false) {
        analogWrite(PUMP_PWM_PIN, pumpPwm);
        pumpOnFlag = true;
      }
      else if (pumpOnFlag == true) {
        analogWrite(PUMP_PWM_PIN, 0);
        pumpOnFlag = false;
      }
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
      Serial.println("------------------------------");
      Serial.println("Program started");
    }

    else if (strcmp(receivedChars, "reset") == 0) {
      analogWrite(PUMP_PWM_PIN, 0);

      runProgram = false;
      countDownMinutesHasRun = false;
      state = 0;
      Serial.println("Program stopped and state resetted");
      Serial.println("------------------------------");
    }

    else if (strcmp(receivedChars, "rtc") == 0) {
      printDateAndTime();
    }

    else if (strcmp(receivedChars, "++") == 0) {
      if (runProgram) {
        state++;
        countDownMinutesHasRun = false;
        waitForMsHasRun = false;
      }
      else {
        Serial.println("program has to be running for this to function");
      }
    }


    else if ( receivedInt >= 0 && receivedInt <= 255) {
      pumpPwm = receivedInt;
      Serial.print("pumpPwm set to: ");
      Serial.println(pumpPwm);
      analogWrite(PUMP_PWM_PIN, pumpPwm);
    }

    newData = false;
  }
}
