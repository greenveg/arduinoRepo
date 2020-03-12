#include "FastLED.h"
//Setting global vars
#define NUM_LEDS 26
#define DATA_PIN 11
#define CLOCK_PIN 12

//Initialise the LED array, the LED Hue (ledh) array, and the LED Brightness (ledb) array.
CRGB leds[NUM_LEDS];
byte ledh[NUM_LEDS];
byte ledb[NUM_LEDS];


void setup() { 
  delay(2000); //delay for safety
  //FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS); 
  FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);
}

void loop() {
  setAllRGB(0, 100, 0);

}

void setAllRGB(int r, int g, int b){
    for (int i = 0 ; i <= NUM_LEDS ; i++) {
    leds[i].setRGB( r, g, b); 
    }
    FastLED.show(); 
  } 

void setAllHSV(int h, int s, int v){
    for (int i = 0 ; i <= NUM_LEDS ; i++) {
    leds[i].setHSV( h, s, v);
    }
    FastLED.show();   
  }

void allBlack() {
  for (int i = 0 ; i <= NUM_LEDS ; i++) {
    leds[23] = CRGB::Black;   
  }
  FastLED.show(); 
}
void drawFractionalBar(int width, uint8_t hue){
  //Fake setup
  int     F16pos = 0; // position of the "fraction-based bar"
  int     F16delta = 1; // how many 16ths of a pixel to move the Fractional Bar

  int InterframeDelay = 40; //ms

  
  int i = pos16 / 16; // convert from pos to raw pixel number
  uint8_t frac = pos16 & 0x0F; // extract the 'factional' part of the position
  uint8_t firstpixelbrightness = 255 - (frac * 16);
 

  uint8_t lastpixelbrightness  = 255 - firstpixelbrightness;
 
  uint8_t bright;
  for( int n = 0; n <= width; n++) {
    if( n == 0) {
      // first pixel in the bar
      bright = firstpixelbrightness;
    } else if( n == width ) {
      // last pixel in the bar
      bright = lastpixelbrightness;
    } else {
      // middle pixels
      bright = 255;
    }
   
    leds[i] += CHSV( hue, 255, bright);
    i++;
    if( i == NUM_LEDS) i = 0; // wrap around
  }
}
 }


