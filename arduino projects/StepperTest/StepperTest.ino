/* 
This is a test sketch for the Adafruit assembled Motor Shield for Arduino v2
It won't work with v1.x motor shields! Only for the v2's with built in PWM
control

For use with the Adafruit Motor Shield v2 
---->	http://www.adafruit.com/products/1438
*/


#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61); 

// Connect a stepper motor with 200 steps per revolution (1.8 degree)
// to motor port #2 (M3 and M4)
Adafruit_StepperMotor *electrokitMotor = AFMS.getStepper(200, 2);
Adafruit_StepperMotor *printerMotor = AFMS.getStepper(400, 1);

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Stepper test!");

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
  
  electrokitMotor->setSpeed(100);  // 10 rpm   
  //printerMotor->setSpeed(50);  // 10 rpm   

    Bean.setLed(255, 255, 0);
}

void loop() {
  
  Serial.println("Single coil steps");
  //electrokitMotor->step(400, FORWARD, SINGLE); 
  electrokitMotor->step(400, BACKWARD, SINGLE); 
  Bean.setLed(0, 255, 0);

  delay(3000);

  Bean.setLed(0, 255, 0);
  delay(500);
  Bean.setLed(0,0,0);
  delay(100);
  Bean.setLed(0,255,0);
  delay(500);
  Bean.setLed(0,0,0);
  
  /*
  printerMotor->step(100, FORWARD, SINGLE); 
  printerMotor->step(100, BACKWARD, SINGLE); 
  */

  /*
  Serial.println("Double coil steps");
  electrokitMotor->step(100, FORWARD, DOUBLE); 
  electrokitMotor->step(100, BACKWARD, DOUBLE);
  
  /*
  Serial.println("Interleave coil steps");
  electrokitMotor->step(100, FORWARD, INTERLEAVE); 
  electrokitMotor->step(100, BACKWARD, INTERLEAVE); 
  
  Serial.println("Microstep steps");
  electrokitMotor->step(50, FORWARD, MICROSTEP); 
  electrokitMotor->step(50, BACKWARD, MICROSTEP);
  */
}
