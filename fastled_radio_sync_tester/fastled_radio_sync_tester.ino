#include <FastLED.h>
#include <Bounce2.h>


FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define BUTTON_PIN 11


#define NUM_STRIPS 7
#define NUM_LEDS_PER_STRIP 40
#define NUM_LEDS NUM_LEDS_PER_STRIP * NUM_STRIPS
CRGB leds[NUM_STRIPS * NUM_LEDS_PER_STRIP];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

uint8_t state;

Bounce debouncer = Bounce();

void setup() {
  delay(3000); // 3 second delay for recovery

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  debouncer.attach(BUTTON_PIN,INPUT_PULLUP); // Attach the debouncer to a pin with INPUT_PULLUP mode
  debouncer.interval(25); // Use a debounce interval of 25 milliseconds
  

  
  FastLED.addLeds<WS2812, 2, GRB>(leds, 0, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 3, GRB>(leds, NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 4, GRB>(leds, 2 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 5, GRB>(leds, 3 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 6, GRB>(leds, 4 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 7, GRB>(leds, 5 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 8, GRB>(leds, 6 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  
  FastLED.setBrightness(BRIGHTNESS);
}

  
void loop()
{
  switch (state) {
    case 0:
      fill_solid(leds, NUM_LEDS, CRGB::Red);
      break;
    case 1:
      fill_solid(leds, NUM_LEDS, CRGB::Green);
      break;
    case 2:     
      fill_solid(leds, NUM_LEDS, CRGB::Blue);
      break;
    case 3:     
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      break;
    
  }//end switch
  
  FastLED.show();  
  //FastLED.delay(1000/FRAMES_PER_SECOND); 

  if ( debouncer.fell() || debouncer.rose() ) {
    state++;
    if(state > 3) {state=0;}
  }
  
  debouncer.update();
}//end loop
