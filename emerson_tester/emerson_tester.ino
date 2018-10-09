#include <SPI.h>
#include <Controllino.h>    
#include <EEPROMex.h>


#define COLD_OUT_PIN CONTROLLINO_D2
#define HOT_OUT_PIN CONTROLLINO_D1
#define COLD_IN_PIN CONTROLLINO_D3
#define HOT_IN_PIN CONTROLLINO_D0
#define PUMP_PWM_PIN CONTROLLINO_D6
#define EMERSON_1 CONTROLLINO_A0
#define EMERSON_2 CONTROLLINO_A1
#define EMERSON_3 CONTROLLINO_A2
#define EMERSON_4 CONTROLLINO_A3


//Serial reading
const byte numChars = 32;
char receivedChars[numChars]; // an array to store the received data
int receivedInt;
boolean newData = false;

//User settings
uint8_t pumpPwm = 100;
uint32_t pumpDelay = 1000;
uint32_t maxCount = 10;
uint32_t readTimer = 500;
double allowedAdcError = 8.00;
 

//General vars
unsigned long previousMillis = 0; 
unsigned long previousReadMillis = 0; 
unsigned long currentMillis = 0;
unsigned long waitStartedMillis = 0;
uint32_t readCount = 0;
unsigned int eepromCountAddr = 350;
unsigned long startTime = 0;
uint8_t state = 0;
uint8_t eepromCount = 0;
unsigned long count = 0;
unsigned long minute = 60000;

//Flags and stuff, used in functions
bool pumpOnFlag = false;
int userMinute = 0;   
bool countDownMinutesHasRun = false;
bool waitForMsHasRun = false;
bool runProgram = false;
bool runPump = false;

//Sensor data handling
const int numberOfSensors = 4;
const int numberOfSavedValues = 4;
const int refSensor = 3;
uint16_t sensorReadings[numberOfSavedValues][numberOfSensors];
uint16_t runningSum[numberOfSensors];
double runningSumDouble[numberOfSensors];
double runningAvg[numberOfSensors];
double momAvg;
bool sensorWithAdcError[4];
bool sensorWithRunningAvgError[4];



//Functions
void recvWithEndMarker();
void doStuffWithData();

void printDateAndTime() {
  int n;
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
  Serial.println("pump = turns pump on or off");
  Serial.println("open = opens all valves");
  Serial.println("close = closes all valves");
  Serial.println("cold = opens cold circuit and closes hot");
  Serial.println("hot = opens hot circuit and closes cold");
  Serial.println("eeprom = reads address 350 on EEPROM");
  Serial.println("zero = zeroes counter and writes 0 to EEPROM");
  Serial.println("save = writes count to EEPROM");
  Serial.println("start = starts emerson test program");
  Serial.println("reset = stops and resets program");
  Serial.println("rtc = reads current time and date");
  Serial.println("++ = goes to next state in program");
  Serial.println("[number between 0 and 255] sets pump PWM and runs pump");
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
  
  if (currentMillis >= waitStartedMillis + m*minute) {
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

void valveControl(bool coldIn, bool hotIn, bool coldOut, bool hotOut) {
  digitalWrite(COLD_IN_PIN, coldIn);
  digitalWrite(COLD_OUT_PIN, coldOut);
  digitalWrite(HOT_IN_PIN, hotIn);
  digitalWrite(HOT_OUT_PIN, hotOut);
}

void report() {
  Serial.println("Sensor report:");
  Serial.println("If 1, sensor reading was outside values");
  Serial.println(' ');
  for (int i=0 ; i<3 ; i++) {
    Serial.print("Sensor "); Serial.print(i+1);
    Serial.print(" runningAvgCheck: ");
    Serial.println(sensorWithRunningAvgError[i]);
    Serial.print("Sensor "); Serial.print(i+1);
    Serial.print(" adcCheck: ");
    Serial.println(sensorWithAdcError[i]);
  }
}

void setup() {
  Controllino_RTC_init(0);

  eepromCount = EEPROM.readLong(eepromCountAddr);

  pinMode(PUMP_PWM_PIN, OUTPUT);
  pinMode(COLD_OUT_PIN, OUTPUT);
  pinMode(HOT_OUT_PIN, OUTPUT);
  pinMode(COLD_IN_PIN, OUTPUT);
  pinMode(HOT_IN_PIN, OUTPUT);
  pinMode(EMERSON_1, INPUT);
  pinMode(EMERSON_2, INPUT);
  pinMode(EMERSON_3, INPUT);
  pinMode(EMERSON_4, INPUT);

  //Start serial communication
  Serial.begin(9600);

  //Boot sequence
  runProgram = false;

  Serial.println("...booting");
  Serial.println("Emerson tester");
  printDateAndTime();
  printListOfCommands();
  

}//end setup

void loop() {
  currentMillis = millis();  
 
  recvWithEndMarker();  //Read serial
  doStuffWithData();    //Do things with received data, if you want to add a command, edit this function
  
  //Program
  if (runProgram) {
    
    if (count < maxCount) {
      switch (state) {
        case 0:
          printDateAndTime();
          Serial.print("Script will run ");Serial.print(maxCount);
          Serial.println(" sessions and then shut off");
          Serial.print("Allowed error for sensor is: ");
          Serial.println(allowedAdcError);
          Serial.println(' ');
          Serial.print("cycle\t");
          Serial.print("s1\t");
          Serial.print("s1Avg\t");
          Serial.print("s2\t");
          Serial.print("s2Avg\t");
          Serial.print("s3\t");
          Serial.print("s3Avg\t");
          Serial.print("s4\t");
          Serial.print("s4Avg\t");
          Serial.println("Mom. avg");
          
          valveControl(0, 0, 0, 0);
          analogWrite(PUMP_PWM_PIN, 0);
          state++;
          break;
     
        case 1: //Cold circuit        
          valveControl(1, 0, 1, 0);
          analogWrite(PUMP_PWM_PIN, pumpPwm);
          waitForMs(5000);
          break;
          
        case 2: //Bleeds in hot water
          valveControl(0, 1, 1, 0);
          waitForMs(1000);
          break;

        case 3: //Hot circuit
          valveControl(0, 1, 0, 1);
          waitForMs(5000);
          break;

        case 4: //Bleeds in cold water
          valveControl(1, 0, 0, 1);
          waitForMs(1000);
          break;
             
        case 5:
          count++;
          state = 1;
          break;
    
      }//end switch
    }//end count checker if
    else {
      Serial.println("Program run finished");
      analogWrite(PUMP_PWM_PIN, 0);
      valveControl(0, 0, 0, 0);
      report();
      runProgram = false;
    }
    
    //Sensor reading and data handling
    if (currentMillis - previousReadMillis >= readTimer) {
      previousReadMillis = currentMillis;
      readCount++;
      
      //Shift values one row down
      for (int i=2 ; i>=0 ; i--) {
        for (int k=0 ; k<4 ; k++) {
          sensorReadings[i+1][k] = sensorReadings[i][k];
        } 
      }

      //Insert new values in array
      sensorReadings[0][0] = analogRead(CONTROLLINO_A0);
      sensorReadings[0][1] = analogRead(CONTROLLINO_A1);
      sensorReadings[0][2] = analogRead(CONTROLLINO_A2);
      sensorReadings[0][3] = analogRead(CONTROLLINO_A3);

      //Data handling      
      momAvg=0;
      for (int i = 0 ; i<numberOfSensors ; i++) {
        double avg = 0;
        for (int j = 0 ; j<numberOfSavedValues ; j++) {
          avg += sensorReadings[j][i];
        }
        avg /= 4;
        runningAvg[i] = avg;

         momAvg += sensorReadings[0][i];
      }
      momAvg = momAvg/numberOfSensors;
            
      //Print log friendly string on serial
      Serial.print(count);
      Serial.print("\t");
      for (int i=0 ; i<4 ; i++) {
        Serial.print(sensorReadings[0][i]);Serial.print("\t");
        Serial.print(runningAvg[i]);Serial.print("\t");
      }
      Serial.println(momAvg);

      
      //Check if sensors 1-3 are within spec
      if (readCount>6 && state != 2 && state != 4 ) {
        for (int i=0 ; i<3 ; i++) {
          if ((runningAvg[i] < (runningAvg[refSensor]-allowedAdcError)) || 
              (runningAvg[i] > (runningAvg[refSensor]+allowedAdcError)) ) {
            //Serial.print("Running avg for sensor ");Serial.print(i+1);Serial.print(" was off by more than ");
            //Serial.println(allowedAdcError);
            sensorWithRunningAvgError[i] = 1;
          }
          
          if (sensorReadings[i] < sensorReadings[refSensor]-(int)allowedAdcError ||
              sensorReadings[i] > sensorReadings[refSensor]+(int)allowedAdcError) {
            //Serial.print("Sensor ");Serial.print(i);Serial.print("reading was off by more than ");
            //Serial.println(allowedAdcError);
            sensorWithAdcError[i] = 1;
          }
          
        }
      }
    
        
    }//end readTimer

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
    
    if(strcmp(receivedChars, "list") == 0) {
      printListOfCommands();
    }
    
    else if(strcmp(receivedChars, "pump") == 0) {
      if (pumpOnFlag == false) {
        analogWrite(PUMP_PWM_PIN, pumpPwm);
        pumpOnFlag = true;
      }
      else if (pumpOnFlag == true) {
        analogWrite(PUMP_PWM_PIN, 0);
        pumpOnFlag = false;
      }
    }
    
    else if(strcmp(receivedChars, "close") == 0) {
      valveControl(0, 0, 0, 0);
      Serial.println("All valve pins set to low");
    }

    else if(strcmp(receivedChars, "open") == 0) {
      valveControl(1, 1, 1, 1);
      Serial.println("All valve pins set to high");
    }

    else if(strcmp(receivedChars, "cold") == 0) {
      valveControl(1, 0, 1, 0);
      Serial.println("Cold circuit open");
    }
    
    else if(strcmp(receivedChars, "hot") == 0) {
      valveControl(0, 1, 0, 1);
      Serial.println("Hot circuit open");
    }
   
    else if(strcmp(receivedChars, "zero") == 0) {
      EEPROM.writeLong(eepromCountAddr, 0);
      count = 0;
      Serial.print("Address ");
      Serial.print(eepromCountAddr);
      Serial.println(" at EEPROM was zeroed");
    }
    
    else if(strcmp(receivedChars, "eeprom") == 0) {
      Serial.print("count = ");
      Serial.println(EEPROM.readLong(eepromCountAddr));
    }

    else if(strcmp(receivedChars, "save") == 0) {
      EEPROM.writeLong(eepromCountAddr, count);
      Serial.println(EEPROM.readLong(eepromCountAddr));
    }

    else if(strcmp(receivedChars, "sensorError") == 0) {
      for (int i=0 ; i<4 ; i++) {
        Serial.println(sensorWithAdcError[i]);
      }
    }
   
    else if(strcmp(receivedChars, "start") == 0) {
      runProgram = true;
      //startTime = millis();
      Serial.println("------------------------------");
      Serial.println("Program started");
    }

    else if(strcmp(receivedChars, "reset") == 0) {
      analogWrite(PUMP_PWM_PIN, 0);
      valveControl(0, 0, 0, 0);
      runProgram = false;
      countDownMinutesHasRun = false;
      readCount = 0;
      state = 0;
      Serial.println("Program stopped and state resetted");
      Serial.println("------------------------------");
    }

    else if(strcmp(receivedChars, "rtc") == 0) {
      printDateAndTime();
    } 

    else if(strcmp(receivedChars, "report") == 0) {
      report();
    } 
    
    else if(strcmp(receivedChars, "++") == 0) {
      if (runProgram) {
        state++;
        countDownMinutesHasRun = false;
        waitForMsHasRun = false;
      }
      else {Serial.println("program has to be running for this to function");}  
    }
   
    
    else if( receivedInt >= 0 && receivedInt <= 255) {
      analogWrite(PUMP_PWM_PIN, receivedInt);
    }
    
  
   newData = false;
  }
}
