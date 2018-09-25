#include <SPI.h>
#include <Controllino.h>    //Controllino lib
#include <EEPROMex.h>

#define PUMP_1_PIN CONTROLLINO_D10
#define PUMP_2_PIN CONTROLLINO_D11
#define MIXER_PUMP_1_PIN CONTROLLINO_D9
#define MIXER_PUMP_2_PIN CONTROLLINO_D8

/*
 * TO DO
 * -add nightly pause  
 * -add mixer pumps and control
 */

//Serial reading
const byte numChars = 32;
char receivedChars[numChars]; // an array to store the received data
int receivedInt;
boolean newData = false;

//General vars
uint8_t pumpPwm = 100;
unsigned long maxCount = 10;
unsigned long count = 0;
unsigned long minute = 60000;

unsigned long previousMillis = 0; 
unsigned long currentMillis = 0;
uint8_t state = 0;
uint8_t userTemp = 0;

bool pump1OnFlag = false;
bool pump2OnFlag = false;

bool countDownMinutesHasRun = false;
bool waitForMsHasRun = false;
int userMinute = 0;
unsigned long waitStartedMillis = 0;

bool runProgram = false;




//Functions
void setShowerTemp (int C) {
  String tempString = "NEW USER TEMPERATURE:";
  tempString += C;
  tempString += "\n";

  char tempArray[25];
  tempString.toCharArray(tempArray, 25);
  Serial2.write(tempArray);

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
  Serial.println("p1 = turns pump 1 on or off");
  Serial.println("p2 = turns pump 2 on or off");
  Serial.println("eeprom = reads address 4 on EEPROM");
  Serial.println("zero = writes 0 to address 4 on EEPROM");
  Serial.println("start = starts pollination program");
  Serial.println("stop = stops program");
  Serial.println("reset = stops program and resets state");
  Serial.println("rtc = reads current time and date");
  Serial.println("++ = goes to next state in program");
  Serial.println("ON = sends SHOWER ON command on Serial2");
  Serial.println("OFF = sends SHOWER OFF command on Serial2");
  Serial.println("OPEN = sends OPEN ENCLOSURE command on Serial2");
  Serial.println("CLOSE = sends CLOSE ENCLOSURE command on Serial2");
  Serial.println("CSV = sends START CSV LOGGING command on Serial2");
  Serial.println("CSP = sends CSP REBOOT command on Serial2");
  Serial.println("OUTER_HEAT = sends OUTER_HOT_WATER_TREATMENT_TRIGGERED on Serial2");
  Serial.println("[number] = sets shower temp to [number]");
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
  pinMode(MIXER_PUMP_1_PIN, OUTPUT);
  pinMode(MIXER_PUMP_2_PIN, OUTPUT);

  //Start serial communication
  Serial.begin(9600);
  Serial2.begin(115200);
    
  //Boot sequence
  runProgram = false;
  Serial.println("...booting");
  Serial.println(__FILE__);
  printDateAndTime();
  printListOfCommands();
  

}//end setup

void loop() {
  currentMillis = millis();  
 
  recvWithEndMarker();  //Read serial
  doStuffWithData();    //Do things with received data, if you want to add a command, edit this func



  //Program
  if (runProgram) {
    if (count < maxCount) {
      switch (state) {
        case 0:
          Serial.println("Inside state 0, this state contains blocking delays!");
          printDateAndTime();
          Serial.println(' ');
          Serial.print("Session ");
          Serial.print(count+1);
          Serial.print("/");
          Serial.print(maxCount);
          Serial.println(" starts now");
        
          Serial2.write("SHOWER ON\n");
          Serial.println("SHOWER ON command was sent to shower");     
          /*
          delay(2000);
          Serial2.write("SHOWER ON\n");
          Serial.println("SHOWER ON command was sent to shower"); 
          delay(1000);
          */
          //Calculate a randomized shower temp 
          userTemp = random(21, 24);
          setShowerTemp(userTemp);
          Serial.print("Shower temp set to: ");
          Serial.println(userTemp);

          //Start mixer pumps
          analogWrite(MIXER_PUMP_1_PIN, 100);
          analogWrite(MIXER_PUMP_2_PIN, 100);
          
          state++;
          break;
     
        case 1:
          countDownMinutes(2, " minutes to shampoo");       
          break;
          
        case 2:      
          analogWrite(PUMP_1_PIN, 100);
          waitForMs(10000);
          
          break;
        
        case 3:
            analogWrite(PUMP_1_PIN, 0);
            state ++;
          break;      
    
        case 4:
          countDownMinutes(4, " minutes to conditioner");  
          break;  
    
        case 5:
          analogWrite(PUMP_2_PIN, 75);
          waitForMs(7000);        
          break;  
          
        case 6:
          analogWrite(PUMP_2_PIN, 0);
          
          //Stop mixer pumps
          analogWrite(MIXER_PUMP_1_PIN, 0);
          analogWrite(MIXER_PUMP_2_PIN, 0);
          
          state ++;
          break;
    
        case 7:
          countDownMinutes(6, " minutes to shower off");  
          break; 
  
         case 8:
          Serial2.write("SHOWER OFF\n"); 
          Serial.println("SHOWER OFF command was sent to shower");
          /*
          delay(1000);
          Serial2.write("SHOWER OFF\n"); 
          delay(1000);
          Serial2.write("SHOWER OFF\n"); 
          */
          count++;
          EEPROM.write(4, count);
          
         
          if ( Controllino_GetHour >= 14 ) {
              Serial.println("7 hours to next session");
              state = 12;  
          }
          else if ( (count%10) == 0) { 
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
          countDownMinutes(8, " minutes to next session");
          break;
          
        case 11:
          state = 0;
          break;
          
        case 12:
          countDownMinutes(7*60, " ");
          break;

       case 13: 
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
    
    if(strcmp(receivedChars, "list") == 0) {
      printListOfCommands();
    }
    
    else if(strcmp(receivedChars, "p1") == 0) {
      if (pump1OnFlag == false) {
        analogWrite(PUMP_1_PIN, pumpPwm);
        Serial.print("Pump 1 turned on at ms:");
        Serial.println(currentMillis);
        pump1OnFlag = true;
      }
      else if (pump1OnFlag == true) {
        analogWrite(PUMP_1_PIN, 0);
        Serial.print("Pump 1 turned off at ms:");
        Serial.println(currentMillis);
        pump1OnFlag = false;
      }
    }

    else if(strcmp(receivedChars, "p11") == 0) {
      if (pump1OnFlag == false) {
        analogWrite(PUMP_1_PIN, 150);
        Serial.print("Pump 1 turned on at ms:");
        Serial.println(currentMillis);
        pump1OnFlag = true;
      }
      else if (pump1OnFlag == true) {
        analogWrite(PUMP_1_PIN, 0);
        Serial.print("Pump 1 turned off at ms:");
        Serial.println(currentMillis);
        pump1OnFlag = false;
      }
    }

    else if(strcmp(receivedChars, "p2") == 0) {
      if (pump2OnFlag == false) {
        analogWrite(PUMP_2_PIN, pumpPwm);
        Serial.print("Pump 2 turned on at ms:");
        Serial.println(currentMillis);
        pump2OnFlag = true;
      }
      else if (pump2OnFlag == true) {
        analogWrite(PUMP_2_PIN, 0);
        Serial.print("Pump 2 turned off at ms:");
        Serial.println(currentMillis);
        pump2OnFlag = false;
      }
    }
       
    else if(strcmp(receivedChars, "p22") == 0) {
      if (pump2OnFlag == false) {
        analogWrite(PUMP_2_PIN, 150);
        Serial.print("Pump 2 turned on at ms:");
        Serial.println(currentMillis);
        pump2OnFlag = true;
      }
      else if (pump2OnFlag == true) {
        analogWrite(PUMP_2_PIN, 0);
        Serial.print("Pump 2 turned off at ms:");
        Serial.println(currentMillis);
        pump2OnFlag = false;
      }
    }

    
    
    else if(strcmp(receivedChars, "zero") == 0) {
      count = 0;
      EEPROM.writeLong(4, count);
      Serial.println("Address 4 at EEPROM was zeroed");
    }
    
    else if(strcmp(receivedChars, "eeprom") == 0) {
      Serial.print("count = ");
      Serial.println(EEPROM.readLong(4));
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

    else if(strcmp(receivedChars, "rtc") == 0) {
      printDateAndTime();
    } 
   
    else if(strcmp(receivedChars, "++") == 0) {
      if (runProgram) {
        state++;
        countDownMinutesHasRun = false;
      }
      else {Serial.println("program has to be running for this to function");}  
    }
   
    
    else if( receivedInt >= 0 && receivedInt <= 49) {
      setShowerTemp(receivedInt); 
    }
    
  
   newData = false;
  }
}
