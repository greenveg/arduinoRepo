#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 3
#define NUM_LEDS 24

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  colorWipe(strip.Color(255, 0, 0), 50); // Red
  colorWipe2(strip.Color(0, 255, 0), 50);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }

    for(uint16_t k=strip.numPixels() ; k >= 0 ; k-- ) {
    strip.setPixelColor(k, 0,0,0);
    strip.show();
    delay(wait);
  }
}

void colorWipe2(uint32_t c, uint8_t wait) {
  for( uint16_t i=0; i<NUM_LEDS; i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }

  for( uint16_t i=NUM_LEDS; i>0; i--) {
    strip.setPixelColor(i, 0, 0, 0);
    strip.show();
    delay(wait);
  }
  
}
