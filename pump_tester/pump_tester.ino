#include <EEPROM.h>
#include <SPI.h>
#include <Controllino.h>    //Controllino lib

#define PUMP_1_PIN CONTROLLINO_D11
#define PUMP_2_PIN CONTROLLINO_D7

/* Target for 12.15g cont per liter
 *  Time      ml      pwm guess
 *  0-5s      65      70
 *  5-10s     140     140
 *  10-15s    100     100
 *  15-20s    50      65
 *  20-25s    35      50
 *  25-30s    25      
 *  30-35s    20
 *  35-40s    20
 *  40-45s    20
 *  45-50s    20
 *  50-55s    20
 *  55-60     20
 * 
 * WATER 5sec (1-5 runs are averaged)
 * pwm   ml          
 * 42   25
 * 45   29
 * 50   36          
 * 75   69
 * 100  100
 * 125  126
 * 135  136
 * 140  140
 * 
 */

//Serial reading
const byte numChars = 32;
char receivedChars[numChars]; // an array to store the received data
int receivedInt;
boolean newData = false;

uint32_t currentMillis = 0;
uint32_t pumpStartedMillis = 0;
bool pumpIsOn = false;

uint32_t pumpTime = 5000;

void setup() {
  pinMode(PUMP_1_PIN, OUTPUT);
  pinMode(PUMP_2_PIN, OUTPUT);

  //Start serial communication
  Serial.begin(9600);
  Serial.println("...booting");

  

}//end setup

void loop() {
  currentMillis = millis();
 
  recvWithEndMarker();  //Read serial
  doStuffWithData();    //Do things with received data, if you want to add a command, edit this func 

  if (currentMillis >= pumpStartedMillis + pumpTime && pumpIsOn) {
    analogWrite(PUMP_1_PIN, 0);
    analogWrite(PUMP_2_PIN, 0);
    pumpIsOn = false;
    
    //Serial.print("Pump ");Serial.println(currentMillis);
  }
  
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
   
    if( receivedInt > 0 && receivedInt <= 255) {
      analogWrite(PUMP_1_PIN, receivedInt);
      analogWrite(PUMP_2_PIN, receivedInt); 
      pumpStartedMillis = currentMillis;
      pumpIsOn = true;
      
      Serial.print("PWM: ");Serial.println(receivedInt);    
      //Serial.print("Start millis: ");Serial.println(pumpStartedMillis);
    }
    else if (receivedInt == 0) {
      analogWrite(PUMP_1_PIN, 0);
      analogWrite(PUMP_2_PIN, 0);
      pumpIsOn = false;
      
    }
    else {
      Serial.println("Not a legal command");
    }
    newData = false;
  }
}
