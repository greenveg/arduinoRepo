#include <Bounce2.h>


#define RADIO_PIN A2


Bounce debouncer = Bounce();

void setup() {
  Serial.begin(9600);
  Serial.println("booting");
  pinMode(RADIO_PIN, INPUT_PULLUP);
  debouncer.attach(RADIO_PIN, INPUT_PULLUP); // Attach the debouncer to a pin with INPUT_PULLUP mode
  debouncer.interval(25); // Use a debounce interval of 25 milliseconds

  pinMode(LED_BUILTIN, OUTPUT);
  delay(500);



}

void loop() {
    if ( debouncer.rose()) {
    digitalWrite(LED_BUILTIN, HIGH);
  }

  else if ( debouncer.fell()) {
    digitalWrite(LED_BUILTIN, LOW);
  }

  debouncer.update();

  Serial.println(digitalRead(RADIO_PIN));
}
