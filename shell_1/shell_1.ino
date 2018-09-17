#include <SPI.h>
#include <Controllino.h>    //Controllino lib
#include <EEPROMex.h>
//#include "Arduino.h"

#define FLOW_PIN CONTROLLINO_A5
#def

//Serial reading
const byte numChars = 32;
char receivedChars[numChars]; // an array to store the received data
int receivedInt;
boolean newData = false;

unsigned int eepromCountAddr = 350;

//General vars
unsigned long previousMillis = 0; 
unsigned long previousReadMillis = 0; 
unsigned long currentMillis = 0;
unsigned long waitStartedMillis = 0;

unsigned long startTime = 0;
uint8_t state = 0;
uint8_t eepromCount = 0;
unsigned long count = 0;
unsigned long maxCount = 3000;
unsigned long minute = 60000;
int pumpDelay = 1000;



int userMinute = 0; //global var needed in countDownMinutes()
bool countDownMinutesHasRun = false;
bool waitForMsHasRun = false;
bool runProgram = false;
bool runPump = false;
uint8_t previousDate = 0;
uint8_t currentDate = 0;



//Functions

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
  Serial.println("openAll = opens all valve");
  Serial.println("clsoeAll = closes all valve");
  Serial.println("eeprom = reads address 350 on EEPROM");
  Serial.println("zero = writes 0 to address 350 on EEPROM");
  Serial.println("save = writes count to address 350 on EEPROM");
  Serial.println("start = starts emerson test program");
  Serial.println("pause = pauses program");
  Serial.println("reset = stops program and resets state");
  Serial.println("rtc = reads current time and date");
  Serial.println("setDate = set startDate as current date");
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

  pinMode(PUMP_RELAY, OUTPUT);
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

  previousDate = Controllino_GetDay();
    
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

  if (read1+10 < readSum/4 || read1-10 > readSum/4 || 
      read2+10 < readSum/4 || read2-10 > readSum/4 ||
      read3+10 < readSum/4 || read3-10 > readSum/4 ||
      read4+10 < readSum/4 || read4-10 > readSum/4 ){
        
        runProgram = false;
        digitalWrite(PUMP_RELAY, LOW);
        digitalWrite(LOW_TEMP_OUT_VALVE_PIN, LOW);
        digitalWrite(HIGH_TEMP_OUT_VALVE_PIN, LOW);
        digitalWrite(LOW_TEMP_IN_VALVE_PIN, LOW);
        digitalWrite(HIGH_TEMP_IN_VALVE_PIN, LOW);
        Serial.println("Error: one or more sensors is out of spec");
        Serial.print("Cycle: ");
        Serial.println(count);
      }
  //Program
  if (runProgram) {
    if (currentMillis - previousReadMillis >= 2000) {
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
      
      previousReadMillis = currentMillis;
    }
    
    if (count < maxCount) {
      switch (state) {
        case 0:
          printDateAndTime();
          Serial.println(' ');
          Serial.print("Cycle ");
          Serial.print(count+1);
          Serial.print("/");
          Serial.println(maxCount);
          
          state++;
          break;
     
        case 1:
          //Serial.println("state: 1");
        
          digitalWrite(PUMP_RELAY, LOW);
          waitForMs(pumpDelay);
          break;
        
        case 2:
          //Serial.println("state: 2");
          digitalWrite(LOW_TEMP_IN_VALVE_PIN, HIGH);
          digitalWrite(LOW_TEMP_OUT_VALVE_PIN, HIGH);
          digitalWrite(HIGH_TEMP_IN_VALVE_PIN, LOW);
          digitalWrite(HIGH_TEMP_OUT_VALVE_PIN, LOW);
          waitForMs(pumpDelay);
          break;

        case 3:
          //Serial.println("state: 3");
          digitalWrite(PUMP_RELAY, HIGH);
          state++;
          break;

        case 4:
          countDownMinutes(1, "");
          break;

        case 5:
          digitalWrite(PUMP_RELAY, LOW);      
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
          digitalWrite(PUMP_RELAY, HIGH);  
          //Serial.println("Hot cycle started");
          state++;
          break;
        
        case 8:
          countDownMinutes(1, ""); 
          break;
         
        case 9:
          count++;
          state = 1;
          /*
          Serial.print("Cycle ");
          Serial.print(count+1);
          Serial.print("/");
          Serial.println(maxCount); 
          */
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
    
    if(strcmp(receivedChars, "list") == 0) {
      printListOfCommands();
    }
    
    else if(strcmp(receivedChars, "pump") == 0) {
      if (pumpOnFlag == false) {
        digitalWrite(PUMP_RELAY, HIGH);
        pumpOnFlag = true;
      }
      else if (pumpOnFlag == true) {
        analogWrite(PUMP_RELAY, 0);
        pumpOnFlag = false;
      }
    }
    
    else if(strcmp(receivedChars, "closeAll") == 0) {
        digitalWrite(LOW_TEMP_IN_VALVE_PIN, LOW);
        digitalWrite(LOW_TEMP_OUT_VALVE_PIN, LOW);
        digitalWrite(HIGH_TEMP_IN_VALVE_PIN, LOW);
        digitalWrite(HIGH_TEMP_OUT_VALVE_PIN, LOW);
        Serial.println("All valve pins set to low");
    }

    else if(strcmp(receivedChars, "openAll") == 0) {
        digitalWrite(LOW_TEMP_IN_VALVE_PIN, HIGH);
        digitalWrite(LOW_TEMP_OUT_VALVE_PIN, HIGH);
        digitalWrite(HIGH_TEMP_IN_VALVE_PIN, HIGH);
        digitalWrite(HIGH_TEMP_OUT_VALVE_PIN, HIGH);
        
        Serial.println("All valve pins set to high");
    }

    else if(strcmp(receivedChars, "openCold") == 0) {
      digitalWrite(LOW_TEMP_IN_VALVE_PIN, HIGH);
      digitalWrite(LOW_TEMP_OUT_VALVE_PIN, HIGH);
      digitalWrite(HIGH_TEMP_IN_VALVE_PIN, LOW);
      digitalWrite(HIGH_TEMP_OUT_VALVE_PIN, LOW);
    
      Serial.println("Cold valve pins set to high");
    }
    
    else if(strcmp(receivedChars, "openHot") == 0) {
      digitalWrite(LOW_TEMP_IN_VALVE_PIN, LOW);
      digitalWrite(LOW_TEMP_OUT_VALVE_PIN, LOW);
      digitalWrite(HIGH_TEMP_IN_VALVE_PIN, HIGH);
      digitalWrite(HIGH_TEMP_OUT_VALVE_PIN, HIGH);
      
      Serial.println("Hot valve pins set to highclose");
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
    
    else if(strcmp(receivedChars, "pause") == 0) {
      runProgram = false;
      digitalWrite(PUMP_RELAY, LOW);
      countDownMinutesHasRun = false;
      Serial.println("Program stopped");
      Serial.println("------------------------------");
    }  

    else if(strcmp(receivedChars, "reset") == 0) {
      digitalWrite(PUMP_RELAY, LOW);
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
  
    else if(strcmp(receivedChars, "setDate") == 0) {
      previousDate = Controllino_GetDay();
      Serial.print("Date set to: ");
      Serial.println(previousDate);
    }    
    else if(strcmp(receivedChars, "++") == 0) {
      if (runProgram) {
        state++;
        countDownMinutesHasRun = false;
        waitForMsHasRun = false;
      }
      else {Serial.println("program has to be running for this to function");}  
    }
   
    
    else if( receivedInt >= 400 && receivedInt <= 3000) {
      //EEPROM.writeLong(eepromCountAddr, receivedInt);
      
      pumpDelay = receivedInt;
      Serial.print("pumpDelay set to: ");
      Serial.println(pumpDelay);
    }
    
  
   newData = false;
  }
}
