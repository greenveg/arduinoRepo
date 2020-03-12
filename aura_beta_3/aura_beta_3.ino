#include <FastLED.h>

#define NUM_LEDS 280
#define STRIP_LENGTH 20
#define BRIGHTNESS 70

#define BUTTON_PIN A2

CRGBArray<NUM_LEDS> leds; // main array

#define front1 leds(0, 19)
#define front2 leds(40, 59)
#define front3 leds(80, 99)
#define front4 leds(120, 139)
#define front5 leds(160, 179)
#define front6 leds(200, 219)
#define front7 leds(240, 259)

CRGBSet frontAll [] = {front1, front2, front3, front4, front5, front6, front7};

#define back1 leds(20, 39)
#define back2 leds(60, 79)
#define back3 leds(100, 119)
#define back4 leds(149, 159)
#define back5 leds(180, 199)
#define back6 leds(220, 239)
#define back7 leds(260, 279)

CRGBSet backAll [] = {back1, back2, back3, back4, back5, back6, back7};

uint8_t gHue; // rotating base color
CRGB tempColors[14]; // color array for whatever
bool tempBools[14]; // bool array for whatever
uint8_t c;
bool runShowProgram = false;




void setup() {
  delay(1000);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  Serial.begin(9600);
  Serial.println("booting");

  FastLED.addLeds<WS2812, 3, GRB>(leds(0, 39), 40);
  FastLED.addLeds<WS2812, 4, GRB>(leds(40, 79), 40);
  FastLED.addLeds<WS2812, 5, GRB>(leds(80, 119), 40);
  FastLED.addLeds<WS2812, 6, GRB>(leds(120, 159), 40);
  FastLED.addLeds<WS2812, 7, GRB>(leds(160, 199), 40);
  FastLED.addLeds<WS2812, 8, GRB>(leds(200, 239), 40);
  FastLED.addLeds<WS2812, 9, GRB>(leds(240, 279), 40);

  FastLED.setBrightness(BRIGHTNESS);

  for(int i = 0; i < 7; i += 2) {
    tempBools[i] = true;
  }
  for(int i = 1; i < 8; i += 2) {
    tempBools[i] = false;
  }
}//end setuo




void loop() {
  if (!runShowProgram) {
    if(!analogRead(BUTTON_PIN)) {
      runShowProgram = true;
      c++;
    }
  }
 
  
  switch(c) {
    
    case 0:
      break;
      
    case 1:
      fill_rainbow(tempColors, 7, gHue, 36);
      rainbowWheel(frontAll, true);
      break;
      
    case 2:
      fill_rainbow(tempColors, 7, gHue, 36);
      rainbowWheel(frontAll, true);
      for(CRGBSet & strip : frontAll) {
        addGlitter(strip, 4);
      }
      break;

    case 3:
      fill_rainbow(tempColors, 7, gHue, 36);
      fadeSinMulti(frontAll, 50, 50, tempColors, 30, 7);
      break;

    case 4:
      fill_rainbow(tempColors, 7, gHue, 36);
      bpmMulti(frontAll, 80, 80, tempColors, tempBools, 7);
      break;

    case 5:
      fill_rainbow(tempColors, 7, gHue, 36);
      bpmMidMulti(frontAll, 100, 100, tempColors, tempBools, 7);
      break;
    case 6: 
      //end of show, fill black;
      
      led=CRGB(black);
      
  }
  
  
  FastLED.show();
  EVERY_N_MILLISECONDS(30) {
    gHue++;
  }


}//end loop




void addGlitter(CRGBSet strip, fract8 chanceOfGlitter) {
  if (random8() < chanceOfGlitter) {
    strip[random8(STRIP_LENGTH)] += CRGB::White;
  }
}

void confetti() {
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy(leds, NUM_LEDS, 10);
  uint16_t pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void fadeSin(CRGBSet strip, uint8_t bpm, uint8_t fadeFactor, CRGB color) {
  // a colored dot sweeping back and forth over the strip in a sin-wave, with fading trails
  //static uint8_t pos;

  uint8_t pos = beatsin8(bpm, 0, STRIP_LENGTH - 1);
  strip[pos] = color;

  EVERY_N_MILLISECONDS(20) {
    fadeToBlackBy(strip, STRIP_LENGTH, fadeFactor);
  }
}

void fadeSinMulti(CRGBSet strips[], uint8_t bpm, uint8_t fadeFactor, CRGB colors[], uint8_t phase, uint8_t amount) {

  if (phase == 0) {
    static uint8_t pos;

    pos = beatsin8(bpm, 0 , STRIP_LENGTH - 1);

    for (int i = 0; i < amount; i++) {
      strips[i][pos] = colors[i];
    }
  } else {
    static uint8_t pos[14];
    for (int i = 0; i < amount; i++) {
      pos[i] = beatsin8(bpm, 0, STRIP_LENGTH - 1, 0, i * phase);
      strips[i][pos[i]] = colors[i];
    }
  }


  EVERY_N_MILLISECONDS(20) {
    for (int i = 0; i < amount; i++) {
      fadeToBlackBy(strips[i], STRIP_LENGTH, fadeFactor);
    }
  }

}

void rainbow(CRGBSet strip, uint16_t ms, uint8_t dHue) {
  EVERY_N_MILLISECONDS(ms) {
    strip.fill_rainbow(gHue, dHue);
  }
}

void fire(CRGBSet strip, uint8_t spark, uint8_t cool) {
  // För blå eld: CRGBPalette16( CRGB::Black, CRGB::Blue, CRGB::Aqua,  CRGB::White) istället för HeatColor

  // Array of tempColorserature readings at each simulation cell
  static byte heat[STRIP_LENGTH];

  EVERY_N_MILLISECONDS(30) {
    // Step 1.  Cool down every cell a little
    for ( int i = 0; i < STRIP_LENGTH; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((cool * 10) / STRIP_LENGTH) + 2));
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for ( int k = STRIP_LENGTH - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if ( random8() < spark ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160, 255) );
    }
  }


  // Step 4.  Map from heat cells to LED colors
  for ( int j = 0; j < STRIP_LENGTH; j++) {
    CRGB color = HeatColor( heat[j]); // = ColorFromPalette(BlåEld, scale8(heat[j], 240));
    strip[j] = color;
  }
}

void pulse(CRGBSet strip, uint8_t bpm, uint8_t high) {
  uint8_t value = beatsin8(bpm, 0, high);
  strip.fadeToBlackBy(value);
  // strip[0].v = value; // vill skriva så här
}

void bpm(CRGBSet strip, uint8_t bpm, uint8_t fadeFactor, CRGB color, bool in, uint8_t phase) {
  uint8_t pos = beatsin8(bpm, 0, STRIP_LENGTH - 1, 0, phase);
  if(in) {
    strip[pos] = color;
    
    for (int i = pos + 1; i < STRIP_LENGTH; i++) {
      strip[i].fadeToBlackBy(fadeFactor);
    }
  } else {
    strip(19, 0)[pos] = color;
    
    for (int i = pos + 1; i < STRIP_LENGTH; i++) {
      strip(19, 0)[i].fadeToBlackBy(fadeFactor);
    }
    // bpm(strip(STRIP_LENGTH - 1, 0, bpm, fadeFactor, color, true) //testa detta!
  }
}

void bpmMulti(CRGBSet strips[], uint8_t bpm, uint8_t fadeFactor, CRGB colors[], bool in[], uint8_t amount) {
  uint8_t pos = beatsin8(bpm, 0, STRIP_LENGTH - 1);
  for (int i = 0; i < amount; i++) {
    if(in[i]) {
      strips[i][pos] = colors[i];
    } else {
      strips[i](19, 0)[pos] = colors[i];
    }
  }
  for (int i = 0; i < amount; i++) {
    for (int j = pos + 1; j < STRIP_LENGTH; j++) {
      if(in[i]) {
        strips[i][j].fadeToBlackBy(fadeFactor);
      } else {
        strips[i](19, 0)[j].fadeToBlackBy(fadeFactor);
      }
    }
  }
}

void bpmMid(CRGBSet strip, uint8_t bpm, uint8_t fadeFactor, CRGB color, bool in) {
  uint8_t pos = beatsin8(bpm, 0, 9);
  if(in) {
    strip(10, 19)[pos] = color;
  
    for (int i = pos + 1; i < 10; i++) {
      strip(10, 19)[i].fadeToBlackBy(fadeFactor);
    }
  } else {
    strip(19, 10)[pos] = color;
  
    for (int i = pos + 1; i < 10; i++) {
      strip(19, 10)[i].fadeToBlackBy(fadeFactor);
    }
  }
  

  strip(0, 9) = strip(19, 10);
}

void bpmMidMulti(CRGBSet strips[], uint8_t bpm, uint8_t fadeFactor, CRGB colors[], bool in[], uint8_t amount) {
  uint8_t pos = beatsin8(bpm, 0, 9);
  for (int i = 0; i < amount; i++) {
    if(in[i]) {
      strips[i](10, 19)[pos] = colors[i];
    } else {
      strips[i](19, 10)[pos] = colors[i];
    }
  }
  for (int i = 0; i < amount; i++) {
    for (int j = pos + 1; j < 10; j++) {
      if(in[i]) {
        strips[i](10, 19)[j].fadeToBlackBy(fadeFactor);
      } else {
        strips[i](19, 10)[j].fadeToBlackBy(fadeFactor);
      }
    }
    strips[i](0, 9) = strips[i](19, 10);
  }
}

void rainbowWheel(CRGBSet strips[], bool left) {
  CRGB temp[14];
  fill_rainbow(temp, 14, gHue, 18);
  int a;
  if (left) {
    a = 0;
  } else {
    a = 7;
  }
  for(int i = 0; i < 7; i++) {
    strips[i] = tempColors[i + a];
  }
}
