#include <Servo.h>
#include <Adafruit_NeoPixel.h>

#define SERVO_PIN 8
#define NEO_PIN 10
#define NUM_LEDS 5


//for serial read
const byte numChars = 32;
char receivedChars[numChars]; // an array to store the received data
int receivedInt;
boolean newData = false;

//ints...
const uint8_t openPos = 75;
const uint8_t closePos = 0;
uint8_t blinkTimes = 0;
uint8_t blinkDelay = 0;
uint8_t rollTimes = 0;


//general vars
boolean lockIsOpen;
unsigned long ledPreviousMillis = 0; 
unsigned long previousMillis = 0; 
uint32_t savedColor[NUM_LEDS];


Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, NEO_PIN, NEO_RGB + NEO_KHZ800);
Servo lockservo; 

void setup() {
  Serial.begin(9600);
  strip.begin();

  turnOffAllLeds();
  
  lockservo.attach(SERVO_PIN);  // attaches the servo on pin 9 to the servo object
  openLock();
  lockIsOpen = true;

  Serial.println("...booting");
  Serial.println("The following commands are available:");
  Serial.println("openLock = sets the servo in the open position");
  Serial.println("closeLock = sets the servo in the close position");
  Serial.println("off = turns off all leds in the attached strip");
  Serial.println("red = sets all leds to red in the attached strip");
  Serial.println("[any number between 0 and 5] = sets the number of green leds");
  Serial.println("blinkAll = runs the blink function");
  Serial.println("roll = rolls the leds back and forth");
}

void loop() {
  unsigned long currentMillis = millis();  
  recvWithEndMarker();
  doStuffWithData();
  blinkAll();
  rollLeds();
}

void openLock() {
  if (lockIsOpen == false) {
    lockservo.write(openPos);   
    lockIsOpen = true;
  }
  else {
    Serial.println("Lock is already open");
  }
}
void closeLock() {
  if (lockIsOpen == true) {
    lockservo.write(closePos);
    lockIsOpen = false;
  }
  else {
    Serial.println("Lock is already open");
  }
}


void setAllLeds(int red, int green, int blue) {
  for (int i=0 ; i < NUM_LEDS ; i++) {
    strip.setPixelColor(i, red, green, blue);
  }    
  strip.show();  
}
void turnOffAllLeds() {
  setAllLeds(0, 0, 0);
}

void setAllLedsToRed() {
  setAllLeds(255, 0, 0);
}
void setNumberOfGreenLeds(int i) {
  if (i <= 5) {
  setAllLedsToRed();
    for (int k=0 ; k<i ; k++) {
      strip.setPixelColor(k, 0, 255, 0); 
    }
  }
  strip.show(); 
}

void setBlinkAll(int times, int k) {
  saveLeds();
  blinkTimes =  times;
  blinkDelay = k;
}
void blinkAll() {
  unsigned long currentMillis = millis(); 
  if(blinkTimes > 1){ 
    if (currentMillis - ledPreviousMillis >= blinkDelay) {
      if(blinkTimes & 0x01) {
        setAllLeds(0, 255, 0);
      }
      else {
        setAllLeds(0, 0, 0);
      }
      blinkTimes--;
      ledPreviousMillis = currentMillis;
    }
  }
  else if (blinkTimes == 1) {
    Serial.println("inside if");;
    setLedsToSaved();
    blinkTimes = 0;
  }
}

void setRollLeds(int k) {
  Serial.println("inside setRollLeds()");
  saveLeds();
  rollTimes = k*10+1;    
}
void rollLeds() {
  unsigned long currentMillis = millis();  
    if (currentMillis - ledPreviousMillis >= 50) {
      if(rollTimes > 1) {
        setAllLeds(0, 0, 0);
        if((rollTimes+3)/5 &1) { 
          strip.setPixelColor((rollTimes+3)%NUM_LEDS , 0, 0, 255);
        }
        else {
          strip.setPixelColor(NUM_LEDS-1-(rollTimes+3)%NUM_LEDS , 0, 0, 255);
        }
        strip.show();  
        rollTimes--;
        ledPreviousMillis = currentMillis;
      }
      else if (rollTimes == 1) {
        setLedsToSaved();
        rollTimes = 0;
      }
  }
}


void saveLeds() {
  for (int i=0 ; i<NUM_LEDS ; i++) {
    savedColor[i] = strip.getPixelColor(i);  
  }
}
void setLedsToSaved() {
  for (int i=0 ; i<NUM_LEDS ; i++) {
    strip.setPixelColor(i, savedColor[i]);  
  }
  strip.show();
}

void recvWithEndMarker() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();
  
    if (rc != endMarker) {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars) {
        ndx = numChars - 1;
      }
    }
   else {
    receivedChars[ndx] = '\0'; // terminate the string
    ndx = 0;
    newData = true;
    }
  }

 receivedInt = atoi(receivedChars);
}

void doStuffWithData() {
 if (newData == true) {
 //Serial.print("rec: ");
 //Serial.println(receivedChars);

  if(strcmp(receivedChars, "off") == 0) {
    turnOffAllLeds();
  }
  else if(strcmp(receivedChars, "red") == 0) {
    setAllLedsToRed();
  }
  else if(strcmp(receivedChars, "openLock") == 0) {
    openLock();
  }
  else if(strcmp(receivedChars, "closeLock") == 0) {
    closeLock();
  }
  else if(strcmp(receivedChars, "blinkAll") == 0) {
    setBlinkAll(8, 500);
  }
    else if(strcmp(receivedChars, "roll") == 0) {
    setRollLeds(8);
  }
  else if(isDigit(receivedChars) == 0) {
    //Serial.println("iside isDigit");
    //Serial.println(receivedInt);
    setNumberOfGreenLeds(receivedInt);
  }
  
 newData = false;
 }
}




