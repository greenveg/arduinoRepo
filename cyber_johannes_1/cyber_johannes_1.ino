#include <Bounce2.h>            //Includes a library with pre-written debounce functions 
#include <Adafruit_NeoPixel.h>  //Includes the Adafruit Neopixel lib

//"#define" just replaces all instances with another text.
#define BUTTON1_PIN   2     //Pin that button1 is connected to
#define BUTTON2_PIN   3
#define BUTTON3_PIN   4
#define BUTTON4_PIN   5
#define PIXEL_PIN     6     //Pin that is sends data to the strip.
#define PIXEL_COUNT   32    //Number of pixels on the strip

//Initialize a neopixel object
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

//Initialize debounce objects for each buttons
Bounce debounced1 = Bounce(); 
Bounce debounced2 = Bounce(); 
Bounce debounced3 = Bounce(); 
Bounce debounced4 = Bounce(); 

//Array with booleans to store current value
bool currentLedState[4] = {0, 0, 0, 0};


void setup() {
  //Setup all button pins as input pullup. Pullup means that they will read HIGH unless you connect them to GND
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  pinMode(BUTTON3_PIN, INPUT_PULLUP);
  pinMode(BUTTON4_PIN, INPUT_PULLUP);

  //Attach pin and set debounce interval (ms) for each debounce object (and physical button)
  debounced1.attach(BUTTON1_PIN);
  debounced1.interval(8);
  debounced2.attach(BUTTON2_PIN);
  debounced2.interval(8);
  debounced3.attach(BUTTON3_PIN);
  debounced3.interval(8);
  debounced4.attach(BUTTON4_PIN);
  debounced4.interval(8);
 
  //Initialize strip and set it to off
  strip.begin();
  strip.show(); 
}

void loop() {
  /*
  currentLedState is an array with 4 boolean (= either 1 or 0) values.
  currentLedState[0] is for button1. currentLedState[1] is for button2 etc
  
  If you follow an if statement with else if after it
  the code does not need to check all. As soon as 1 is true it skips the rest.
  They are "connected" and basicly become a switch case function. 
  
  If you follow and if statement with more ifs code has to check each an everyone, regardless
  of what happened earlier

  Pseudo code:
  IF button1 goes from high to low AND its current state is OFF {
    1. set all pixels to one specified color
    2. update the current state to ON, so we know next time...
  }
  ELSE IF button1 goes from high to low AND it's currently ON  {
    Set all pixels to off. This function also resets current state to 0
  }
  
  */

  
  //Button 1
  if (debounced1.fell() && !currentLedState[0]){
    colorWipe(255, 80, 0);  // Orange
    currentLedState[0] = 1;
  }
  else if(debounced1.fell() && currentLedState[0]) {
     setStripToOff();
  }

  //Button 2
  else if (debounced2.fell() && !currentLedState[1]){
    colorWipe(0, 255, 150);  // Turqoise
    currentLedState[1] = 1;
  }
  else if(debounced2.fell() && currentLedState[1]) {
     setStripToOff();
  }

  //Button 3
  else if (debounced3.fell() && !currentLedState[2]){
    colorWipe(255, 0, 150);  // Cerise
    currentLedState[2] = 1;
  }
  else if(debounced3.fell() && currentLedState[2]) {
     setStripToOff();
  }

  //Button 4
  else if (debounced4.fell() && !currentLedState[3]){
    colorWipe(255, 0, 0);  // Red
    currentLedState[3] = 1;
  }
  else if(debounced4.fell() && currentLedState[3]) {
     setStripToOff();
  }
  

  //Update all buttons
  debounced1.update();
  debounced2.update();
  debounced3.update();
  debounced4.update(); 
}

//Functions that sets all pixels to black
void setStripToOff() {
  colorWipe(0, 0, 0);
  for (int i=0 ; i<4 ; i++) {
    currentLedState[i] = 0;
  }
}

//Function that fills all pixel with one color
void colorWipe(uint8_t r, uint8_t g, uint8_t b) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, r, g, b);
  }
  strip.show();
}

