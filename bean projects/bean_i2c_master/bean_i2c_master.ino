#include <Wire.h>

void setup() {
  Wire.begin(); // join i2c bus (address optional for master)
  Serial.begin(57600);

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
  
  delay(5000);
  
  Bean.setLed(0, 0, 255);
  x = 2;
  Serial.println("x was set to 2");
  Wire.beginTransmission(8);
  Wire.write(x);
  Serial.println("x was written as 2");
  Wire.endTransmission();    // stop transmitting

  delay(5000);

 
}
