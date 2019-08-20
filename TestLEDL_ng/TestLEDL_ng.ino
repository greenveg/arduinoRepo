#include <FastLED.h>

#define NUM_LEDS 280
#define DATA_PIN 3 // grön

uint32_t currentMillis;
uint32_t startMillis;
uint8_t counter;

CRGBArray<NUM_LEDS> leds;

CRGBSet front1(leds(1, 20));
CRGBSet front2(leds(41, 60));
CRGBSet front3(leds(81, 100));
CRGBSet front4(leds(121, 140));
CRGBSet front5(leds(161, 180));
CRGBSet front6(leds(201, 220));
CRGBSet front7(leds(241, 260));

CRGBSet frontAll [7] = {front1, front2, front3, front4, front5, front6, front7};



// uint16_t pos = 0; // Förmodligen trubbel om flera använder samma pos parallellt, undersök!
// uint8_t gHue = 0; // Basfärg för rainbow


void setup() {
  delay(1000);
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS); // 12 eller 13?
  FastLED.setBrightness(100);

  Serial.begin(9600);
  Serial.println("booting...");
}

void loop() {
  currentMillis = millis();
  
  if (counter < 100) {
    if (counter == 0) {
      startMillis = currentMillis;
    }
    
    for (CRGBSet & set : frontAll) {
      fadingPixelSin(set, 50, 100, CRGB::Orange);
      addGlitter(set, 5);
    }
   
  }//end fake loop
  
  if (counter == 100) {
    Serial.print("currentMillis - startMillis = ");
    Serial.println(currentMillis - startMillis);
    
  }

  counter++;
  Serial.println(counter);
}//end loop


void fadingPixelSin(CRGBSet strip, uint8_t bpm, uint8_t fadeFactor, CRGB color) {
  // a colored dot sweeping back and forth over the strip in a sin-wave, with fading trails
  static uint16_t pos;
  static uint16_t length = strip.size();
  
  strip[pos] = color;
  pos = beatsin16(bpm, 0, length);
  FastLED.show();

  EVERY_N_MILLISECONDS(20) {
    fadeToBlackBy(strip, length, fadeFactor);
  }
}


void wanderingPixel(CRGBSet strip, uint8_t width, uint16_t ms, CRGB color) {
  static uint16_t pos = 0;
  static uint16_t length = strip.size();
  
  strip(pos, pos + width - 1) = color;
  FastLED.show();
  FastLED.delay(ms);
  strip(pos, pos + width - 1) = CRGB::Black;
  
  pos++;
  if (pos == length) {
    pos = 0;
  }
}

void fadingPixel(CRGBSet strip, uint16_t ms, uint8_t fadeFactor, CRGB color) {
  // a colored dot sweeping over the strip, with fading trails
  static uint16_t pos = 0;
  static uint16_t length = strip.size();
  
  EVERY_N_MILLISECONDS(ms) {
    fadeToBlackBy(strip, length, fadeFactor);
    strip[pos] = color;

    pos++;
    if (pos == length) {
      pos = 0;
    }
    
    FastLED.show();
  }
}

void addGlitter(CRGBSet strip, fract8 chanceOfGlitter) {
  if(random8() < chanceOfGlitter) {
    strip[random16(strip.size())] += CRGB::White;
  }
}

void rainbow(CRGBSet strip, uint16_t ms, uint8_t dHue) {
  // Om den är statisk deklareras den inte om varje gång. uints rullar över ner till 0 så behöver inte resetta manuellt.
  static uint8_t hue = 0;
  EVERY_N_MILLISECONDS(ms) {
    strip.fill_rainbow(hue++, dHue);
    FastLED.show();
  }

  //  fill_rainbow(strip, strip.size(), gHue, dHue);
  //  FastLED.show();
  //  EVERY_N_MILLISECONDS(ms) {
  //    gHue++;
  //  }
  //  if (gHue == 256) {
  //    gHue = 0;
  //  }
}
