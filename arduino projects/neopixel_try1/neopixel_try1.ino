#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define NEOPIN 10


Adafruit_NeoPixel strip = Adafruit_NeoPixel(5, NEOPIN, NEO_RGB + NEO_KHZ800);


void setup() {
  
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

}

void loop() {
  strip.setPixelColor(0, 255, 0, 0);    //red
  strip.setPixelColor(1, 0, 255, 0);    //blue
  strip.setPixelColor(2, 0, 0, 255);    //green
  strip.setPixelColor(3, 0, 0, 0);      //off
  strip.setPixelColor(4, 255, 0, 0);    //red
  strip.show();
  
}
