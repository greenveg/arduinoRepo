#include <Controllino.h>

#define DIR_PIN CONTROLLINO_D3
#define STEP_PIN CONTROLLINO_D2
#define ENDSTOP_PIN CONTROLLINO_D4

void setup() {
  Serial.begin(57600);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(ENDSTOP_PIN, INPUT);

  digitalWrite(DIR_PIN, HIGH);
   

}

void loop() {
  // put your main code here, to run repeatedly:

}

void takeOneStep() {
  digitalWrite(STEP_PIN, HIGH);
  delay(2);
  digitalWrite(STEP_PIN, LOW);
}
