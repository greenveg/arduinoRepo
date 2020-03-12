/*
 * TO DO
 * Add flag check for led turn off
 * Add double press for reset/ooops function
 * 
 */


#include <Bounce2.h>

#define BUTTON_PIN 18
#define LED_PIN 19

//Setup debounce objects
Bounce debounced1 = Bounce(); 

uint32_t currentMillis;
uint32_t  delayStartedAtMs;
uint32_t minute = 60000;


void setup() {
  Serial.begin(9600);
  Serial.println("booting");
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  
  debounced1.attach(BUTTON_PIN);
  debounced1.interval(8);

  digitalWrite(LED_PIN, HIGH);
}

void loop() {
  currentMillis = millis();
  
  if( debounced1.fell() ) {
    Serial.println("Button was pressed"); 
    //Serial.print("currentMillis = ");Serial.println(currentMillis);
    digitalWrite(LED_PIN, LOW);
    delayStartedAtMs = currentMillis;
  }
  
  if (currentMillis >=  delayStartedAtMs+minute*60*8) {
    digitalWrite(LED_PIN, HIGH);
  }

  debounced1.update();
}


