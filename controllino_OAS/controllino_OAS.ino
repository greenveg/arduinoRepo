#include <Controllino.h>

#define PUMP_1_PIN CONTROLLINO_D1
#define PUMP_2_PIN CONTROLLINO_D3
//#define PUMP_3_PIN CONTROLLINO_D5
#define STATUS_LED_PIN CONTROLLINO_D4

void setup() {
  pinMode(PUMP_1_PIN, OUTPUT);
  pinMode(PUMP_2_PIN, OUTPUT);
  //pinMode(PUMP_3_PIN, OUTPUT);

  Serial.begin(115200);
  
  
  delay(10000);

  
  Serial.write("SHOWER ON\n");
  digitalWrite(STATUS_LED_PIN, HIGH);
  delay(60000);
  analogWrite(PUMP_1_PIN, 50);
  delay(300);
  analogWrite(PUMP_1_PIN, 0);
  delay(60000);
  analogWrite(PUMP_2_PIN, 50);
  delay(300);
  analogWrite(PUMP_2_PIN, 0);
  delay(60000);
  Serial.write("OPEN ENCLOSURE\n");
  //Serial.write("SHOWER OFF\n");
  digitalWrite(STATUS_LED_PIN, LOW);
  
}

void loop() {
}


/*
  analogWrite(PUMP_1_PIN, 50);
  delay(5000);
  analogWrite(PUMP_1_PIN, 0);
  delay(2000);

  analogWrite(PUMP_2_PIN, 50);
  delay(5000);
  analogWrite(PUMP_2_PIN, 0);
  delay(2000);
  */
