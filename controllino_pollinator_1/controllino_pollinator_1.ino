#include <EEPROM.h>
#include <Controllino.h>    //Controllino lib

#define PUMP_1_PIN CONTROLLINO_D1
#define PUMP_2_PIN CONTROLLINO_D3
//#define PUMP_3_PIN CONTROLLINO_D5
#define STATUS_LED_PIN CONTROLLINO_D4



//Serial reading
const byte numChars = 32;
char receivedChars[numChars]; // an array to store the received data
int receivedInt;
boolean newData = false;


uint8_t pumpPwm = 50;
unsigned long previousMillis = 0; 
unsigned long currentMillis = 0;
unsigned long startTime = 0;
unsigned long pump1StartTime = 0;
unsigned long pump1PreviousMillis = 0;
uint8_t state = 0;
uint8_t lastState = 255;
unsigned long minute = 60000;


bool runProgram = false;
bool runPump = false;
int waitForMinutes = 0;

/*
void waitForMinutes(int minutes) {

}
*/
void setup() {
  
  pinMode(PUMP_1_PIN, OUTPUT);
  pinMode(PUMP_2_PIN, OUTPUT);
  //pinMode(PUMP_3_PIN, OUTPUT);

  //Start serial communication
  Serial.begin(9600);
  Serial2.begin(115200);

  
  //Boot sequence
  runProgram = false;

  Serial.println("...booting");
  Serial.println("PLEASE NOTE: line ending should be New Line only!");
  Serial.println("Available commands:");
  Serial.println("pump1On = turn on pump indefinitely");
  Serial.println("pump1Off = turn off pump");
  Serial.println("pump2On = turn on pump indefinitely");
  Serial.println("pump2Off = turn off pump");
  Serial.println("start = starts pollination program");
  Serial.println("stop = stops program");
  Serial.println("++ = goes to next state in program");
  Serial.println("ON = sends SHOWER ON command on Serial2");
  Serial.println("OFF = sends SHOWER OFF command on Serial2");
  Serial.println("OPEN = sends OPEN ENCLOSURE command on Serial2");
  Serial.println("CLOSE = sends CLOSE ENCLOSUER command on Serial2");

}//end setup

void loop() {
  unsigned long currentMillis = millis();  
  recvWithEndMarker();  //Read serial
  doStuffWithData();    //Do things with received data



  //Program
  if (runProgram) {
    switch (state) {
      case 0:
        Serial2.write("SHOWER ON\n");
        Serial.println("SHOWER ON command was sent to shower");
        
        waitForMinutes = 4;
        Serial.print(waitForMinutes);
        Serial.println(" minutes til event");
        waitForMinutes--;
        previousMillis = currentMillis;
        state++;
        
        break;
   
      case 1:
        
        if (waitForMinutes >= 0) {
          if(millis() >= previousMillis + 2000) {
            previousMillis = millis();
            if(waitForMinutes > 0) {
              Serial.print(waitForMinutes);
              Serial.println(" minutes til event");
            }
            waitForMinutes--;
          }    
        }
        else {
          Serial.println("state++");
          state++;
       } 

        analogWrite(PUMP_1_PIN, 255);   
        
        break;
        
     case 2:
      //Serial.println("state 2");
      if(currentMillis >= previousMillis + 3800) {
        previousMillis = currentMillis;
        analogWrite(PUMP_1_PIN, 0);
        //state ++;
      }
      break;
      
      case 3:
        Serial.println("state 3");
        if(currentMillis >= previousMillis + minute*2) {
          previousMillis = currentMillis;
          analogWrite(PUMP_2_PIN, 50);
          state ++;
        }
        break;      
  
      case 4:
        if(currentMillis >= previousMillis + 3800) {
          previousMillis = currentMillis; 
          analogWrite(PUMP_2_PIN, 0);
          state ++;
        }
        break;  
  
      case 5:
        Serial.println("state 5");
        if(currentMillis >= previousMillis + minute*4) {
          previousMillis = currentMillis;
          Serial.write("SHOWER OFF\n");
          state ++;
        }
        break;  
        
      case 6:
        if(currentMillis >= previousMillis + minute*12) {
          state ++;
        }
        break;
  
      case 7:
        state = 0;
        break; 
    
    }//end switch
  }//end program wrapper

 //Serial.println("loop");
  
}//end loop



void recvWithEndMarker() {
  static byte ndx = 0;
  char endMarker = '\n';
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
      analogWrite(PUMP_1_PIN, pumpPwm);
      Serial.println("inside pumpOn");
    }
    
    else if(strcmp(receivedChars, "pump1Off") == 0) {
      analogWrite(PUMP_1_PIN, 0);
    }
    
    else if(strcmp(receivedChars, "pump2On") == 0) {
      analogWrite(PUMP_2_PIN, pumpPwm);
    }
    
    else if(strcmp(receivedChars, "pump2Off") == 0) {
      analogWrite(PUMP_2_PIN, 0);
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
    
    else if(strcmp(receivedChars, "start") == 0) {
      runProgram = true;
      //startTime = millis();
      Serial.println("------------------------------");
      Serial.println("Program started");
    }
    
    else if(strcmp(receivedChars, "stop") == 0) {
      runProgram = false;
      Serial.println("Program stopped");
      Serial.println("------------------------------");
    }  

    else if(strcmp(receivedChars, "++") == 0) {
      if (runProgram) {state++;}
      else {Serial.println("program has to be running for this to function");}  
    }  
    
    /*
    else if( receivedInt >= 0 && receivedInt <= 100) {
      //runPump(receivedInt); 
    }
    */
  
   newData = false;
  }
}




