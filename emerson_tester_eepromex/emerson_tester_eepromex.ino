#include <SPI.h>
#include <Controllino.h>    //Controllino lib
#include <EEPROMex.h>
//#include "Arduino.h"

#define LOW_TEMP_OUT_VALVE_PIN CONTROLLINO_D2
#define HIGH_TEMP_OUT_VALVE_PIN CONTROLLINO_D1
#define LOW_TEMP_IN_VALVE_PIN CONTROLLINO_D3
#define HIGH_TEMP_IN_VALVE_PIN CONTROLLINO_D0
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
uint32_t maxCount = 100     ;

//General vars
unsigned long previousMillis = 0; 
unsigned long previousReadMillis = 0; 
unsigned long currentMillis = 0;
unsigned long waitStartedMillis = 0;
unsigned int eepromCountAddr = 350;

unsigned long startTime = 0;
uint8_t state = 0;
uint8_t eepromCount = 0;
unsigned long count = 0;
unsigned long minute = 60000;

int read1 = 0;
int read2 = 0;
int read3 = 0;
int read4 = 0;
int readSum = 0;


bool pumpOnFlag = false;


int userMinute = 0; //global var needed in countDownMinutes()
bool countDownMinutesHasRun = false;
bool waitForMsHasRun = false;
bool runProgram = false;
bool runPump = false;


//Functions

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
  Serial.println("zero = writes 0 to address 350 on EEPROM");
  Serial.println("save = writes count to address 350 on EEPROM");
  Serial.println("start = starts emerson test program");
  Serial.println("reset = stops and resets program");
  Serial.println("rtc = reads current time and date");
  Serial.println("setDate = set startDate as current date");
  Serial.println("++ = goes to next state in program");
  Serial.println("[number between 0 and 255] runs pumpPwm");
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



void setup() {
  Controllino_RTC_init(0);

  eepromCount = EEPROM.read(4);

  pinMode(PUMP_PWM_PIN, OUTPUT);
  pinMode(LOW_TEMP_OUT_VALVE_PIN, OUTPUT);
  pinMode(HIGH_TEMP_OUT_VALVE_PIN, OUTPUT);
  pinMode(LOW_TEMP_IN_VALVE_PIN, OUTPUT);
  pinMode(HIGH_TEMP_IN_VALVE_PIN, OUTPUT);
  pinMode(EMERSON_1, INPUT);
  pinMode(EMERSON_2, INPUT);
  pinMode(EMERSON_3, INPUT);
  pinMode(EMERSON_4, INPUT);

  //Start serial communication
  Serial.begin(9600);

  //Boot sequence
  runProgram = false;

  Serial.println("...booting");
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
          /*
          Serial.println(' ');
          Serial.print("Cycle ");
          Serial.print(count+1);
          Serial.print("/");
          Serial.println(maxCount);
          */
          state++;
          break;
     
        case 1:        
          digitalWrite(PUMP_PWM_PIN, 0);
          waitForMs(pumpDelay);
          break;
        
        case 2:
          digitalWrite(LOW_TEMP_IN_VALVE_PIN, HIGH);
          digitalWrite(LOW_TEMP_OUT_VALVE_PIN, HIGH);
          digitalWrite(HIGH_TEMP_IN_VALVE_PIN, LOW);
          digitalWrite(HIGH_TEMP_OUT_VALVE_PIN, LOW);
          waitForMs(pumpDelay);
          break;

        case 3:
          analogWrite(PUMP_PWM_PIN, 150);
          state++;
          break;

        case 4:
          waitForMs(5000);
          break;

        case 5:
          digitalWrite(PUMP_PWM_PIN, 0);      
          waitForMs(pumpDelay);
          break;
     
        case 6:  
          digitalWrite(LOW_TEMP_IN_VALVE_PIN, LOW);
          digitalWrite(LOW_TEMP_OUT_VALVE_PIN, LOW);
          digitalWrite(HIGH_TEMP_IN_VALVE_PIN, HIGH);
          digitalWrite(HIGH_TEMP_OUT_VALVE_PIN, HIGH);
          waitForMs(pumpDelay);
          break;
        
        case 7:
          analogWrite(PUMP_PWM_PIN, pumpPwm);  
          state++;
          break;
        
        case 8:
          waitForMs(5000); 
          break;
         
        case 9:
          count++;
          state = 1;
          

          break;
    
      }//end switch
    }//end count checker if
    else {
      Serial.println("Program run finished");
      analogWrite(PUMP_PWM_PIN, 0);
      digitalWrite(LOW_TEMP_IN_VALVE_PIN, LOW);
      digitalWrite(LOW_TEMP_OUT_VALVE_PIN, LOW);
      digitalWrite(HIGH_TEMP_IN_VALVE_PIN, LOW);
      digitalWrite(HIGH_TEMP_OUT_VALVE_PIN, LOW);
      runProgram = false;
    }
    

    if (currentMillis - previousReadMillis >= 1000) {
      read1 = analogRead(EMERSON_1);
      read2 = analogRead(EMERSON_2);
      read3 = analogRead(EMERSON_3);
      read4 = analogRead(EMERSON_4);
      readSum = read1+read2+read3+read4;

      Serial.print(count);
      Serial.print(",");
      Serial.print(read1);
      Serial.print(",");
      Serial.print(read2);
      Serial.print(",");
      Serial.print(read3);
      Serial.print(",");
      Serial.println(read4);

      if (read1+10 < readSum/4 || read1-10 > readSum/4 || 
        read2+10 < readSum/4 || read2-10 > readSum/4 ||
        read3+10 < readSum/4 || read3-10 > readSum/4 ||
        read4+10 < readSum/4 || read4-10 > readSum/4 ) {
        
        runProgram = false;
        digitalWrite(PUMP_PWM_PIN, 0);
        digitalWrite(LOW_TEMP_OUT_VALVE_PIN, LOW);
        digitalWrite(HIGH_TEMP_OUT_VALVE_PIN, LOW);
        digitalWrite(LOW_TEMP_IN_VALVE_PIN, LOW);
        digitalWrite(HIGH_TEMP_IN_VALVE_PIN, LOW);
        Serial.println("Error: one or more sensors is out of spec");
        Serial.print("Cycle: ");
        Serial.println(count);
      }
      
      previousReadMillis = currentMillis;
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
        digitalWrite(LOW_TEMP_IN_VALVE_PIN, LOW);
        digitalWrite(LOW_TEMP_OUT_VALVE_PIN, LOW);
        digitalWrite(HIGH_TEMP_IN_VALVE_PIN, LOW);
        digitalWrite(HIGH_TEMP_OUT_VALVE_PIN, LOW);
        Serial.println("All valve pins set to low");
    }

    else if(strcmp(receivedChars, "open") == 0) {
        digitalWrite(LOW_TEMP_IN_VALVE_PIN, HIGH);
        digitalWrite(LOW_TEMP_OUT_VALVE_PIN, HIGH);
        digitalWrite(HIGH_TEMP_IN_VALVE_PIN, HIGH);
        digitalWrite(HIGH_TEMP_OUT_VALVE_PIN, HIGH);
        
        Serial.println("All valve pins set to high");
    }

    else if(strcmp(receivedChars, "cold") == 0) {
      digitalWrite(LOW_TEMP_IN_VALVE_PIN, HIGH);
      digitalWrite(LOW_TEMP_OUT_VALVE_PIN, HIGH);
      digitalWrite(HIGH_TEMP_IN_VALVE_PIN, LOW);
      digitalWrite(HIGH_TEMP_OUT_VALVE_PIN, LOW);
    
      Serial.println("Cold circuit open");
    }
    
    else if(strcmp(receivedChars, "hot") == 0) {
      digitalWrite(LOW_TEMP_IN_VALVE_PIN, LOW);
      digitalWrite(LOW_TEMP_OUT_VALVE_PIN, LOW);
      digitalWrite(HIGH_TEMP_IN_VALVE_PIN, HIGH);
      digitalWrite(HIGH_TEMP_OUT_VALVE_PIN, HIGH);
      
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
  
    else if(strcmp(receivedChars, "start") == 0) {
      runProgram = true;
      //startTime = millis();
      Serial.println("------------------------------");
      Serial.println("Program started");
    }

    else if(strcmp(receivedChars, "reset") == 0) {
      digitalWrite(PUMP_PWM_PIN, LOW);
      digitalWrite(LOW_TEMP_OUT_VALVE_PIN, LOW);
      digitalWrite(HIGH_TEMP_OUT_VALVE_PIN, LOW);
      digitalWrite(LOW_TEMP_IN_VALVE_PIN, LOW);
      digitalWrite(HIGH_TEMP_IN_VALVE_PIN, LOW);
      runProgram = false;
      countDownMinutesHasRun = false;
      state = 0;
      Serial.println("Program stopped and state resetted");
      Serial.println("------------------------------");
    }

    else if(strcmp(receivedChars, "rtc") == 0) {
      printDateAndTime();
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
      pumpPwm = receivedInt;
      analogWrite(PUMP_PWM_PIN, pumpPwm);
    }
    
  
   newData = false;
  }
}
