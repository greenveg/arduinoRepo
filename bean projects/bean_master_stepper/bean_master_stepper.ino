#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

Adafruit_StepperMotor *electrokitMotor = AFMS.getStepper(200, 2);

void setup() {
  Wire.begin(); // join i2c bus (address optional for master)
  Serial.begin(57600);
  
  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
  
  electrokitMotor->setSpeed(100);  // 10 rpm   

}

byte x = 0;

void loop() {

  Bean.setLed(0, 255, 0);
  x = 1;
  Serial.println("x was set to 1");
  Wire.beginTransmission(8); // transmit to device #8
  Wire.write(x);              // sends one byte
  Serial.println("x was written as 1");
  Wire.endTransmission();
  Serial.println("FW single coil steps bw");
  electrokitMotor->step(400, FORWARD, SINGLE); 
  
  delay(5000);
  
  Bean.setLed(0, 0, 255);
  x = 2;
  Serial.println("x was set to 2");
  Wire.beginTransmission(8);
  Wire.write(x);
  Serial.println("x was written as 2");
  Wire.endTransmission();    // stop transmitting
  Serial.println("BW single coil steps");
  electrokitMotor->step(400, BACKWARD, SINGLE); 

  delay(5000);

 
}
