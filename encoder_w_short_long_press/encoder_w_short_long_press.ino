#include <Encoder.h>
#include <Bounce2.h>


#define ENCODER_1_A_PIN 5
#define ENCODER_1_B_PIN 3
#define ENCODER_1_BUTTON_PIN 4
#define ENCODER_2_A_PIN 2
#define ENCODER_2_B_PIN 8
#define ENCODER_2_BUTTON_PIN 7

//Setup encoders
Encoder encoder1(ENCODER_1_A_PIN, ENCODER_1_B_PIN);
Encoder encoder2(ENCODER_2_A_PIN, ENCODER_2_B_PIN);

//Setup debounce objects
Bounce debounced1 = Bounce(); 
Bounce debounced2 = Bounce(); 


//////////////// GLOBAL VARIABLES ////////////////
long oldPosition1 = 0;
long newPosition1 = 0;
long actPosition1 = 0;
long oldPosition2 = 0;
long newPosition2 = 0;
long actPosition2 = 0;

unsigned long button1LastMillis = 0;
unsigned long button1RoseMillis = 0;
unsigned long button1FellMillis = 0;
unsigned long button1Diff = 0;

unsigned long currentMillis = 0;


//////////////// FUNCTIONS ////////////////
void updateEncoders() {
  newPosition1 = encoder1.read();
  if (newPosition1 == oldPosition1 + 4) {
    actPosition1++;
    Serial.print("Encoder1 = ");
    Serial.println(actPosition1);
    oldPosition1 = newPosition1;
  }
  if (newPosition1 == oldPosition1 - 4) {
    actPosition1--;
    Serial.print("Encoder1 = ");
    Serial.println(actPosition1);
    oldPosition1 = newPosition1; 
  }
  
  newPosition2 = encoder2.read();
  if (newPosition2 == oldPosition2 + 4) {
    actPosition2++;
    Serial.print("Encoder2 = ");
    Serial.println(actPosition2);
    oldPosition2 = newPosition2;
  }
  if (newPosition2 == oldPosition2 - 4) {
    actPosition2--;
    Serial.print("Encoder2 = ");
    Serial.println(actPosition2);
    oldPosition2 = newPosition2; 
  }
}


//////////////// SETUP ////////////////
void setup() {
  Serial.begin(9600);
  Serial.println("booting");
  
  debounced1.attach(ENCODER_1_BUTTON_PIN);
  debounced1.interval(8);

  debounced2.attach(ENCODER_2_BUTTON_PIN);
  debounced2.interval(8);
  
  pinMode(ENCODER_1_BUTTON_PIN, INPUT_PULLUP);
  pinMode(ENCODER_2_BUTTON_PIN, INPUT_PULLUP);

}//end setup


//////////////// MAIN LOOP ////////////////
void loop() {
  currentMillis = millis();

  if (debounced1.rose()) {
    button1RoseMillis = currentMillis;
  }
  if (debounced1.fell()) {
    button1FellMillis = currentMillis;
    button1Diff = button1FellMillis - button1RoseMillis;
  }
  
  if ( button1Diff != 0 && button1Diff < 500) {
    Serial.println("short press");
    button1Diff = 0;
    button1RoseMillis = 0;
  }
  else if (button1RoseMillis + 1000 >= currentMillis-10 && button1RoseMillis + 1000 <= currentMillis+10 && digitalRead(ENCODER_1_BUTTON_PIN)) {
    Serial.println("long press");
    button1RoseMillis = 0;
  }  

  updateEncoders();
  debounced1.update();
  debounced2.update();
  

}//end main loop
