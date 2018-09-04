#include <Controllino.h>    //Controllino lib

#define PUMP_PIN CONTROLLINO_D3
#define LOW_TEMP_OUT_VALVE_PIN CONTROLLINO_D6
#define HIGH_TEMP_OUT_VALVE_PIN CONTROLLINO_D7
#define LOW_TEMP_IN_VALVE_PIN CONTROLLINO_D2
#define HIGH_TEMP_IN_VALVE_PIN CONTROLLINO_D1
#define EMERSON_1 CONTROLLINO_A0
#define EMERSON_2 CONTROLLINO_A1
#define EMERSON_3 CONTROLLINO_A2
#define EMERSON_4 CONTROLLINO_A3

void setup() {
  digitalWrite(HIGH_TEMP_IN_VALVE_PIN, HIGH);

}

void loop() {
  // put your main code here, to run repeatedly:

}
