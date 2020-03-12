

#include <Wire.h> //inclyde i2c lib
#include <Adafruit_MotorShield.h> //include adafruit motorshield lib v2
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); //create motorshield object w/ default address

Adafruit_StepperMotor *electrokitMotor = AFMS.getStepper(200, 2); //connect to a stepper with 200step/rev = 1.6deg

void setup() {
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output

  AFMS.begin();  // create with the default frequency 1.6KHz

  electrokitMotor->setSpeed(100);  // 10 rpm  
}

void loop() {
  delay(100);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  
  int x = Wire.read();    // receive byte as an integer
  Serial.println(x);         // print the integer

//MAIN SWITCH FOLLOWS

 switch (x) {
    case 1:
      Serial.println("Now inside case 1");
      Serial.println("Single coil steps");
      electrokitMotor->step(200, FORWARD, SINGLE); 

    break;
 
    case 2:
      Serial.println("Now inside case 2");
      Serial.println("Single coil steps");
      electrokitMotor->step(200, BACKWARD, SINGLE); 
    break;
    
    }


}
