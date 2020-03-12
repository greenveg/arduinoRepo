#include "FastLED.h"
#define NUM_LEDS 26
#define DATA_PIN 11
#define CLOCK_PIN 12

CRGB leds[NUM_LEDS];

void setup() { 
  //FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS); 
  FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);
}
 
void loop() {
  for (int i = 0 ; i <= NUM_LEDS ; i++) {
    leds[i] = CRGB::Blue;  
    //leds[i].setRGB( 255, 68, 221); 
  }
  FastLED.show(); 

}
