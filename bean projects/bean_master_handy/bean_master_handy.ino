/*
  Handy BLE - https://pdshelley.com Created 9 June 2015 by Paul Shelley
  LightBlueBean - https://punchthrough.com

  Uses adafruit motorshield v2.3 via i2c and adafruit motorshield library

  Uses wire.h library to handle all i2c communications, both to motorshield and to Uno/Genuino

  Cobbled togeteher 24th of may 2016 by Johannes Holm at Malmö University
*/

//Go fetch some libraries
#include <Wire.h>     //add wire library for i2c                           
#include <Adafruit_MotorShield.h>       //add adafruit motorshield library
#include "utility/Adafruit_MS_PWMServoDriver.h"

//Adafruit motorshield interface follows
Adafruit_MotorShield AFMS = Adafruit_MotorShield();     //create motorshield object
Adafruit_StepperMotor *electrokitMotor = AFMS.getStepper(200, 2);     //add motor

//Handy BLE interface follows
String command;
boolean commandStarted = false;

//var to control Uno
byte x = 0;

//Remember with your brain. Let the robot remember the arm and grip position as long as it is on.
boolean armIsUp = false;
boolean gripIsOpen = false;
int stepsToGo = 7000;


void setup() {
  //Set output
  pinMode(1, OUTPUT);
  digitalWrite(1, HIGH);

  //Begin serial communication (virtual serial over BLE)
  Wire.begin(); // join i2c bus (address optional for master)
  Serial.begin(57600);      //begin virtual serial over bluetooth

  //Initiate the motorshield and set rpm for the stepper motor
  AFMS.begin();  // initiate motorshield object with the default frequency 1.6KHz
  electrokitMotor->setSpeed(200);  // 10 rpm
}

void loop() {
  getCommand();
}

/*
  This function reads the serial port and checks for the start character '#'
  if the start character if found it will add all received characters to
  the command buffer until it receives the end command ';' When the end
  command is received the commandCompleted() function is called.
  if a second start character is found before an end character then the buffer
  is cleared and the process starts over.
*/
void getCommand() {
  while (Serial.available()) {
    char newChar = (char)Serial.read();
    if (newChar == '#') {
      commandStarted = true;
      command = "\0";
    } else if (newChar == ';') {
      commandStarted = false;
      commandCompleted();
      command = "\0";
    } else if (commandStarted == true) {
      command += newChar;
    }
  }
}

/*
  This code is from the example and worked fine. A switch would be neater though /Johannes
    
  This function takes the completed command and checks it against a list
  of available commands and executes the appropriate code.  Add extra 'if'
  statements to add commands with the code you want to execute when that
  command is received. It is recommended to create a function for a command
  if there are more than a few lines of code for as in the 'off' example.
*/


void commandCompleted() {
  if (command == "armup") {
    armUp();
  }
  if (command == "armdown") {
    armDown();
  }
  if (command == "opengrip") {
    openGrip();
  }
  if (command == "closegrip") {
    closeGrip();
  }
  if (command == "drive") {
    drive();
  }
  if (command == "stop") {
    stay();
  }
  if (command == "off") {
    off();
  }
}


//All the functions go here
void armUp() {
  Serial.print("armUp - red");
  Bean.setLed(255, 0, 0);

  if (armIsUp == false) {
    Serial.println("FW single coil steps bw");
    electrokitMotor->step(stepsToGo, FORWARD, SINGLE);
    armIsUp = true;
  }
  else {
    Serial.println("arm is already up!");
    blinkBeanLed(255, 0, 0, 10);
  }

}

void armDown() {
  Serial.print("armDown - green");
  Bean.setLed(0, 255, 0);

  if (armIsUp == true) {
    Serial.println("BW single coil steps");
    electrokitMotor->step(stepsToGo, BACKWARD, SINGLE);
    armIsUp = false;
  }
  else {
    Serial.println("Arm is already down!");
    blinkBeanLed(0, 255, 0, 10);
  }

}

void openGrip() {
  Serial.print("openGrip - blue");
  Bean.setLed(0, 0, 255);
  if (gripIsOpen == false) {
    x = 1;
    Serial.println("x was set to 1");
    Wire.beginTransmission(8); // transmit to device #8
    Wire.write(x);              // sends one byte
    Serial.println("x was written as 1");
    Wire.endTransmission();
    gripIsOpen = true;
  }
  else {
    blinkBeanLed(0, 0, 255, 10);
  }
}

void closeGrip() {
  Serial.print("closeGrip - purple");
  Bean.setLed( 128, 0, 128 );

  if (gripIsOpen == true) {
    x = 2;
    Serial.println("x was set to 2");
    Wire.beginTransmission(8);
    Wire.write(x);
    Serial.println("x was written as 2");
    Wire.endTransmission();    // stop transmitting
    gripIsOpen = false;
  }
  else {
    blinkBeanLed(128, 0, 128, 10);
  }
}

void drive() {
  Serial.println("Drive!");
  digitalWrite(1, LOW);
}

void stay() {
  Serial.println("Stop!");
  digitalWrite(1, HIGH);
}

void blinkBeanLed(int r, int g, int b, int i) {
  for (i = 0; i < 10; i++) {
    Bean.setLed(r, g, b);
    delay(100);
    Bean.setLed(0, 0, 0);
    delay(100);
  }
}

void off(){
  x = 0;
  Wire.beginTransmission(8);
  Wire.write(x);
  Serial.println("x was written as 2");
  Wire.endTransmission();    // stop transmitting
  
}
