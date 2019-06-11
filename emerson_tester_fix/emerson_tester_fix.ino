#include <Controllino.h>

#define COLD_OUT_PIN CONTROLLINO_D2
#define HOT_OUT_PIN CONTROLLINO_D1
#define COLD_IN_PIN CONTROLLINO_D3
#define HOT_IN_PIN CONTROLLINO_D0
#define PUMP_PWM_PIN CONTROLLINO_D6
#define EMERSON_1 CONTROLLINO_A0
#define EMERSON_2 CONTROLLINO_A1
#define EMERSON_3 CONTROLLINO_A2
#define EMERSON_4 CONTROLLINO_A3

int count_hours = 0;
int count_minutes = 0;
int count_seconds = 0;

unsigned long cycleCount = 0;

unsigned long currentMillis;
unsigned long oldMillis;
unsigned long oldCountMillis;
unsigned long oldPrintMillis;
int loopInterval = 1;
int CountInterval = 1000;
int printCSV = 500;
int waitMS = 0;

boolean flagPrintCSVHeader = true;

enum STATE {
  IDLE,
  RESET,
  WAIT,
  COLDWATER,
  BLEEDHOTWATER,
  HOTWATER,
  BLEEDCOLDWATER
};
STATE currentState = IDLE;
STATE oldState = IDLE;

void goToState(STATE new_state);

void setup() {
  pinMode(PUMP_PWM_PIN, OUTPUT);
  pinMode(COLD_OUT_PIN, OUTPUT);
  pinMode(HOT_OUT_PIN, OUTPUT);
  pinMode(COLD_IN_PIN, OUTPUT);
  pinMode(HOT_IN_PIN, OUTPUT);
  pinMode(EMERSON_1, INPUT);
  pinMode(EMERSON_2, INPUT);
  pinMode(EMERSON_3, INPUT);
  pinMode(EMERSON_4, INPUT);

  Serial.begin(115200);
}

void loop() {
  currentMillis = millis();

  CheckForSerialCommands();

  if (currentMillis - oldMillis >= loopInterval) {
    StateUpdate();
    oldMillis = currentMillis;
  }

  if (currentMillis - oldCountMillis >= CountInterval) {
    if (currentState != IDLE) {
      updateCountTime();
    }
    oldCountMillis = currentMillis;
  }

  if (currentMillis - oldPrintMillis >= printCSV) {
    if (currentState != IDLE) {
      if (flagPrintCSVHeader){
        printCSVHeader();
        flagPrintCSVHeader = false;
      }
      printCSVLine();
    }
    oldPrintMillis = currentMillis;
  }
}
