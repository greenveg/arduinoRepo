#include <Servo.h>

#define servo1Pin 9
Servo lockservo; 

//ints...
int openPos = 80;
int closePos = 0;

//for serial read
const byte numChars = 32;
char receivedChars[numChars]; // an array to store the received data
int receivedInt;
boolean newData = false;


void setup() {
 Serial.begin(9600);
 
 Serial.println("<Arduino is ready>");
 
 lockservo.attach(servo1Pin);  // attaches the servo on pin 9 to the servo object
 lockservo.write(80); // go to open position
}

void loop() {
 recvWithEndMarker();
 doStuffWithData();

  
}

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
 Serial.print("This just in ... ");
 Serial.println(receivedChars);

 if (receivedInt >= closePos && receivedInt <= openPos) {
  lockservo.write(receivedInt); 
 }
 else {Serial.println("Invalid position input");}
 newData = false;
 }
}



