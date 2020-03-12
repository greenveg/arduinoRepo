#include <Encoder.h>
#include <LiquidCrystal_I2C.h>

#define ENCODER_DT_PIN 2
#define ENCODER_CLK_PIN 3

//Setup encoder
Encoder encoder1(ENCODER_DT_PIN, ENCODER_CLK_PIN);

//Setup the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27,20,4); 

long oldPosition  = -999;
long newPosition = 0;
long actPosition = 0;
bool buttonState = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Encoder Test:");

  pinMode(ENCODER_BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  newPosition = encoder1.read();
  if (newPosition != oldPosition) {
    if (newPosition % 4 == 0 || newPosition == 0) {
      actPosition = newPosition / 4;
      Serial.println(actPosition); 
    }
    oldPosition = newPosition;
  }

  

}//end main loop
