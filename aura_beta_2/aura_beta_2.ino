#include <FastLED.h>
#include <Bounce2.h>



#define BUTTON_PIN A2


#define NUM_STRIPS 7
#define NUM_LEDS_PER_STRIP 40
#define NUM_LEDS NUM_LEDS_PER_STRIP * NUM_STRIPS
CRGB leds[NUM_STRIPS * NUM_LEDS_PER_STRIP];

#define BRIGHTNESS          50
#define FRAMES_PER_SECOND  120

uint8_t state;

Bounce debouncer = Bounce();

void setup() {
  delay(3000); // 3 second delay for recovery

  Serial.begin(9600);
  Serial.println("booting");
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  debouncer.attach(BUTTON_PIN,INPUT_PULLUP); // Attach the debouncer to a pin with INPUT_PULLUP mode
  debouncer.interval(25); // Use a debounce interval of 25 milliseconds
  

  
  FastLED.addLeds<WS2812, 3, GRB>(leds, 0, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 4, GRB>(leds, 1 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 5, GRB>(leds, 2 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 6, GRB>(leds, 3 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 7, GRB>(leds, 4 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 8, GRB>(leds, 5 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 9, GRB>(leds, 6 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  
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
    Serial.println("button press");
    if(state > 3) {state=0;}
  }
  
  debouncer.update();
}//end loop
