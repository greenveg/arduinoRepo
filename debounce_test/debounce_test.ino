#include <Controllino.h>  
#include <Bounce2.h>


#define BUTTON_PIN CONTROLLINO_D22
#define LED_PIN 13

// Instantiate a Bounce object
Bounce debouncer = Bounce(); 

void setup() {
  Serial.begin(9600);
  Serial.println("boot");

  pinMode(LED_PIN,OUTPUT);
  pinMode(BUTTON_PIN,INPUT_PULLUP);

  debouncer.attach(BUTTON_PIN);
  debouncer.interval(8); // interval in ms
  
  
}

void loop() {
  // Update the Bounce instance :
  debouncer.update();

  if (debouncer.fell()) {Serial.println("Fell");}

}
