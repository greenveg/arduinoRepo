#include <EEPROM.h>
#include <SPI.h>
#include <Controllino.h>    //Controllino lib

#define PUMP_1_PIN CONTROLLINO_D1
#define PUMP_2_PIN CONTROLLINO_D3
#define VALVE_1_PIN CONTROLLINO_D5
#define VALVE_2_PIN CONTROLLINO_D7

/*
 * TO DO
 * -add random temp setting in interval 20-43C for every session
 * -add nightly heat treatment  
 */

//Serial reading
const byte numChars = 32;
char receivedChars[numChars]; // an array to store the received data
int receivedInt;
boolean newData = false;

//General vars
uint8_t pumpPwm = 50;
int maxCount = 500;

unsigned long previousMillis = 0; 
unsigned long currentMillis = 0;
unsigned long waitStartedMillis = 0;
unsigned long startTime = 0;
unsigned long pump1StartTime = 0;
unsigned long pump1PreviousMillis = 0;
uint8_t state = 0;
uint8_t count = 0;
unsigned int lastHeatCount = 0;
unsigned long minute = 60000;
uint8_t userTemp = 0;

int userMinute = 0;
bool countDownMinutesHasRun = false;
bool waitForMsHasRun = false;
bool runProgram = false;
bool runPump = false;
int startDate = 0;


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
  Serial.println("Available commands:");
  Serial.println("list = list all commands");
  Serial.println("pump1On = turn on pump indefinitely");
  Serial.println("pump1Off = turn off pump");
  Serial.println("pump2On = turn on pump indefinitely");
  Serial.println("pump2Off = turn off pump");
  Serial.println("eeprom = reads address 4 on EEPROM");
  Serial.println("zero = writes 0 to address 4 on EEPROM");
  Serial.println("start = starts pollination program");
  Serial.println("stop = stops program");
  Serial.println("reset = stops program and resets state");
  Serial.println("setDate = set startDate as current date");
  Serial.println("++ = goes to next state in program");
  Serial.println("ON = sends SHOWER ON command on Serial2");
  Serial.println("OFF = sends SHOWER OFF command on Serial2");
  Serial.println("OPEN = sends OPEN ENCLOSURE command on Serial2");
  Serial.println("CLOSE = sends CLOSE ENCLOSURE command on Serial2");
  Serial.println("CSV = sends START CSV LOGGING command on Serial2");
  Serial.println("CSP = sends CSP REBOOT command on Serial2");
  Serial.println("OUTER_HEAT = sends OUTER_HOT_WATER_TREATMENT_TRIGGERED on Serial2");
  Serial.println(' ');
}

void countDownMinutes(int m, String message) {
  if (!countDownMinutesHasRun) {
    userMinute = m;
    waitStartedMillis = currentMillis;
    previousMillis = waitStartedMillis;
    Serial.print(userMinute);
    Serial.println(message);
    userMinute--;
    countDownMinutesHasRun = true;
  }
  
  if (currentMillis >= waitStartedMillis + m*minute) {
    countDownMinutesHasRun = false;
    Serial.println("state++");
    state++;
  }
  else if (userMinute > 0 && currentMillis >= previousMillis + minute) {
    previousMillis = currentMillis;
    Serial.print(userMinute);
    Serial.println(message);
    userMinute--;
  }
}//end countDownMinutes()


void waitForMs(int m) {
  if (!waitForMsHasRun) {
    waitStartedMillis = millis();
    waitForMsHasRun = true;
  }
  if (millis() >= waitStartedMillis + m) {
    waitForMsHasRun = false;
    Serial.println("state++");
    state++;
  }
}//end countDownMinutes()



void setup() {
  Controllino_RTC_init(0);

  count = EEPROM.read(4);
  
  pinMode(PUMP_1_PIN, OUTPUT);
  pinMode(PUMP_2_PIN, OUTPUT);
  pinMode(VALVE_1_PIN, OUTPUT);

  //Start serial communication
  Serial.begin(9600);
  Serial2.begin(115200);

  startDate = Controllino_GetDay();
    
  //Boot sequence
  runProgram = false;

  Serial.println("...booting");
  Serial.println("PLEASE NOTE: line ending should be New Line only!");
  printDateAndTime();
  printListOfCommands();
  

}//end setup

void loop() {
  currentMillis = millis();  
 
  recvWithEndMarker();  //Read serial
  doStuffWithData();    //Do things with received data



  //Program
  if (runProgram) {
    if (count < maxCount) {
      switch (state) {
        case 0:
          Serial.println("inside state 0, this state contains blocking delays");
          Serial.println(' ');
          printDateAndTime();
          Serial.println(' ');
          Serial.print("Session ");
          Serial.print(count+1);
          Serial.print("/");
          Serial.print(maxCount);
          Serial.println(" starts now");

          //Calculate shower temp and write to computer
          userTemp = random(30, 43);
          Serial.print("Shower temp set to: ");
          Serial.println(userTemp);

          Serial2.write("OPEN ENCLOSURE\n");
          delay(1000);
          Serial2.write("CLOSE ENCLOSURE\n");
          delay(1000);          
          Serial2.write("SHOWER ON\n");
          Serial.println("SHOWER ON command was sent to shower");     
          delay(6000);
          Serial2.write("SHOWER ON\n");
          Serial.println("SHOWER ON command was sent to shower");
          delay(1000);

          
          //Set shower temperature
          Serial2.write("NEW USER TEMPERATURE:");
          Serial2.write(userTemp);
          Serial2.write("\n");
          
          state++;
          break;
     
        case 1:
          countDownMinutes(2, " minutes to shampoo");       
          break;
          
        case 2:
          state = 8;
          /*
          digitalWrite(VALVE_1_PIN, HIGH);
          analogWrite(PUMP_1_PIN, pumpPwm);
          waitForMs(1500);
          */
          break;
        
        case 3:
            analogWrite(PUMP_1_PIN, 0);
            digitalWrite(VALVE_1_PIN, LOW);
            state ++;
          break;      
    
        case 4:
          countDownMinutes(2, " minutes to conditioner");  
          break;  
    
        case 5:
          digitalWrite(VALVE_2_PIN, HIGH);
          analogWrite(PUMP_2_PIN, pumpPwm);
          waitForMs(3000);        
          break;  
          
        case 6:
          analogWrite(PUMP_2_PIN, 0);
          digitalWrite(VALVE_2_PIN, LOW);
          state ++;
          break;
    
        case 7:
          countDownMinutes(6, " minutes to shower off");  
          break; 
  
         case 8:
          Serial2.write("SHOWER OFF\n"); 
          Serial.println("SHOWER OFF command was sent to shower");
          delay(1000);
          Serial2.write("SHOWER OFF\n"); 
          delay(1000);
          Serial2.write("SHOWER OFF\n"); 
          
          count++;
          EEPROM.write(4, count);
          
          if ( (count%3) == 0) {
            
            state = 9;  
          }
          else {
            state = 10;
          }
          break;
          
        case 9:
          Serial2.write("OUTER_HOT_WATER_TREATMENT_TRIGGERED\n");
          Serial.println("Ext. heat treat. command was sent to shower");
          delay(1000);
          Serial2.write("OUTER_HOT_WATER_TREATMENT_TRIGGERED\n");
          
          
          state++;
          break;
          
          
        case 10:
          countDownMinutes(60, " minutes to next session");
          break;
          
        case 11:
          state = 0;
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
    
    if(strcmp(receivedChars, "pump1On") == 0) {
      digitalWrite(VALVE_1_PIN, HIGH);
      analogWrite(PUMP_1_PIN, pumpPwm);
    }
    
    else if(strcmp(receivedChars, "list") == 0) {
      printListOfCommands();
    }
    
    else if(strcmp(receivedChars, "pump1Off") == 0) {
      analogWrite(PUMP_1_PIN, 0);
      digitalWrite(VALVE_1_PIN, LOW);
    }
    
    else if(strcmp(receivedChars, "pump2On") == 0) {
      digitalWrite(VALVE_2_PIN, HIGH);
      analogWrite(PUMP_2_PIN, pumpPwm);
    }
    
    else if(strcmp(receivedChars, "pump2Off") == 0) {
      analogWrite(PUMP_2_PIN, 0);
      digitalWrite(VALVE_2_PIN, LOW);
    }
    
    else if(strcmp(receivedChars, "zero") == 0) {
      EEPROM.write(4, 0);
      count = 0;
      Serial.println("Address 4 at EEPROM was zeroed");
    }
    
    else if(strcmp(receivedChars, "eeprom") == 0) {
      Serial.print("count = ");
      Serial.println(EEPROM.read(4));
    }
    
    else if(strcmp(receivedChars, "ON") == 0) {
      Serial2.write("SHOWER ON\n");
    }

    else if(strcmp(receivedChars, "OFF") == 0) {
      Serial2.write("SHOWER OFF\n");
    }
    
    else if(strcmp(receivedChars, "OPEN") == 0) {
      Serial2.write("OPEN ENCLOSURE\n");
    }
    
    else if(strcmp(receivedChars, "CLOSE") == 0) {
      Serial2.write("CLOSE ENCLOSURE\n");
    }

    
    else if(strcmp(receivedChars, "CSV") == 0) {
      Serial2.write("START_CSV_LOGGING\n");
    }

    else if(strcmp(receivedChars, "CSP") == 0) {
      Serial2.write("CSP_REBOOT\n");
    }
    
    else if(strcmp(receivedChars, "OUTER_HEAT") == 0) {
      Serial2.write("OUTER_HOT_WATER_TREATMENT_TRIGGERED\n");
    }
    
    else if(strcmp(receivedChars, "start") == 0) {
      runProgram = true;
      //startTime = millis();
      Serial.println("------------------------------");
      Serial.println("Program started");
    }
    
    else if(strcmp(receivedChars, "stop") == 0) {
      runProgram = false;
      countDownMinutesHasRun = false;
      Serial.println("Program stopped");
      Serial.println("------------------------------");
    }  

    else if(strcmp(receivedChars, "reset") == 0) {
      runProgram = false;
      countDownMinutesHasRun = false;
      state = 0;
      Serial.println("Program stopped and state resetted");
      Serial.println("------------------------------");
    }
    
    else if(strcmp(receivedChars, "setDate") == 0) {
      startDate = Controllino_GetDay();
      Serial.print("Date set to: ");
      Serial.println(startDate);
    }    
    else if(strcmp(receivedChars, "++") == 0) {
      if (runProgram) {state++;}
      else {Serial.println("program has to be running for this to function");}  
    }

    else if(strcmp(receivedChars, "open1") == 0) {
      Serial.println("Open valve 1");
      digitalWrite(VALVE_1_PIN, HIGH);  
    } 

    else if(strcmp(receivedChars, "close1") == 0) {
      Serial.println("Close valve 1");
      digitalWrite(VALVE_1_PIN, LOW);
    } 

    else if(strcmp(receivedChars, "open2") == 0) {
      Serial.println("Open valve 2");
      digitalWrite(VALVE_2_PIN, HIGH);  
    } 

    else if(strcmp(receivedChars, "close2") == 0) {
      Serial.println("Close valve 2");
      digitalWrite(VALVE_2_PIN, LOW);
    }
    
    
    /*
    else if( receivedInt >= 0 && receivedInt <= 100) {
      //runPump(receivedInt); 
    }
    */
  
   newData = false;
  }
}




