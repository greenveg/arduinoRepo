#include <EEPROM.h>
#include <SPI.h>
#include <Controllino.h>    //Controllino lib

#define PUMP_1_PIN CONTROLLINO_D10
#define PUMP_2_PIN CONTROLLINO_D11

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




void setup() {
  pinMode(PUMP_1_PIN, OUTPUT);
  pinMode(PUMP_2_PIN, OUTPUT);

  //Start serial communication
  Serial.begin(9600);
  Serial.println("...booting");

  

}//end setup

void loop() {
 
  recvWithEndMarker();  //Read serial
  doStuffWithData();    //Do things with received data, if you want to add a command, edit this func 
  
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
   
    if( receivedInt >= 0 && receivedInt <= 255) {
      analogWrite(PUMP_1_PIN, receivedInt);
      analogWrite(PUMP_2_PIN, receivedInt);
      Serial.println(receivedInt);    
    }
    newData = false;
  }
}




