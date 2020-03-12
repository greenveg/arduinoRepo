#include <Adafruit_NeoPixel.h>

#define NEO_PIN 10
#define NUM_LEDS 5

uint32_t color = 0;


Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, NEO_PIN, NEO_RGB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
  strip.begin();

  setAllLeds(255, 0, 200);
  color = strip.getPixelColor(1);
  Serial.println(color);

}

/*
COLOR LIST
  values should be stored as uint_32_t
  red = 16711680
  green = 65280
  blue = 255
  yellow = 16776960
  magenta = 16711880
  cyan = 65535
  better cyan (less blue) = 65480
  white = 16777160
  

  



 */


void loop() {
  // put your main code here, to run repeatedly:

}

void setAllLeds(int red, int green, int blue) {
  for (int i=0 ; i < NUM_LEDS ; i++) {
    strip.setPixelColor(i, red, green, blue);
  }    
  strip.show();  
}


