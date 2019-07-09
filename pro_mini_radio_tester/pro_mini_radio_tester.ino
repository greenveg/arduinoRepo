#include <FastLED.h>

#define RADIO_PIN 11



#define NUM_STRIPS 7
#define NUM_LEDS_PER_STRIP 40
#define NUM_LEDS NUM_LEDS_PER_STRIP * NUM_STRIPS
CRGB leds[NUM_STRIPS * NUM_LEDS_PER_STRIP];

uint32_t counter;
uint32_t currentMillis;
uint32_t lastMillis;

void radioLedChecker() {
  if (!digitalRead(RADIO_PIN) ) {
    digitalWrite(LED_BUILTIN, HIGH); 
    Serial.println("radio was pressed");    
  }
  else {
      digitalWrite(LED_BUILTIN, LOW);
  }
 }

void setup() {
  delay(1000);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RADIO_PIN, INPUT_PULLUP);

  Serial.begin(9600);
  Serial.println("booting");  
  
  FastLED.addLeds<WS2812, 2, GRB>(leds, 0, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 3, GRB>(leds, NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 4, GRB>(leds, 2 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 5, GRB>(leds, 3 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 6, GRB>(leds, 4 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 7, GRB>(leds, 5 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 8, GRB>(leds, 6 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  
}//end setup


void loop() {
  currentMillis = millis();

  if(currentMillis > lastMillis + 50 ) {
      lastMillis = currentMillis;
      
      leds[counter-1] = CRGB::Black;
      leds[counter] = CRGB::Red;
      Serial.println(counter);
      counter++;
      if (counter == 280 ) {counter = 0;}

  }
  
  FastLED.show();
  radioLedChecker();
}//end loop
