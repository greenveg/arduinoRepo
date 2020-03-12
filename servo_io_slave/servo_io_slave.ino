#include <Servo.h>
#define SERVO_PIN 8


const uint8_t openPos = 80;
const uint8_t closePos = 0;

Servo lockservo; 

void setup() {
  lockservo.attach(SERVO_PIN);
  pinMode(4, INPUT);
  lockservo.write(openPos);
  delay(5000);

  
}

void loop() {

  Serial.println(digitalRead(4));
  
  if (digitalRead(4) == HIGH) {
    lockservo.write(openPos);  
  }
  else {
    lockservo.write(closePos);   
  }
    
}

