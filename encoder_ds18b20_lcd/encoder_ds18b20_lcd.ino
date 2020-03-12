#include <Encoder.h>
#include <Bounce2.h>
#include <Wire.h> 
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>

#define ENCODER_1_A_PIN 3
#define ENCODER_1_B_PIN 5
#define ENCODER_1_BUTTON_PIN 4
#define ENCODER_2_A_PIN 2
#define ENCODER_2_B_PIN 8
#define ENCODER_2_BUTTON_PIN 7

#define ONE_WIRE_BUS_PIN A3         //OnewWire has to be on analog pin


// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS_PIN);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
//Specific adresses
DeviceAddress Probe01Add = { 0x28, 0xFF, 0x94, 0x19, 0xA2, 0x17, 0x04, 0x11 }; 
DeviceAddress Probe02Add = { 0x28, 0xFF, 0x8A, 0x61, 0xA0, 0x17, 0x05, 0x6A };


//Setup LCD screen on I2C
LiquidCrystal_I2C lcd(0x3F,20,4); // set the LCD address to 0x27 for a 16 chars and 2 line display


//Setup encoders
Encoder encoder1(ENCODER_1_A_PIN, ENCODER_1_B_PIN);
Encoder encoder2(ENCODER_2_A_PIN, ENCODER_2_B_PIN);

//Setup debounce objects
Bounce debounced1 = Bounce(); 
Bounce debounced2 = Bounce(); 


//////////////// GLOBAL VARIABLES ////////////////
long oldPosition1 = 0;
long newPosition1 = 0;
long actPosition1 = 0;
long oldActPosition1 = 0;
long oldPosition2 = 0;
long newPosition2 = 0;
long actPosition2 = 0;
long oldActPosition2 = 0;

unsigned long button1LastMillis = 0;
unsigned long button1RoseMillis = 0;
unsigned long button1FellMillis = 0;
unsigned long button1Diff = 0;

unsigned long button2LastMillis = 0;

unsigned long currentMillis = 0;
unsigned long tempLastMillis = 0;
unsigned int tempTimer = 2000; //cannot be smaller than 187 for 10 bit resoliton on sensors
double probe01Temp = 0;
double probe02Temp = 0;

uint8_t state = 1;


double getTemperature(DeviceAddress deviceAddress){
  double tempC = sensors.getTempC(deviceAddress);

  if (tempC == -127.00) {
    Serial.print("Error getting temperature");
  } 
  else {
    return tempC;
  }
  
}

void updateEncoders() {
  newPosition1 = encoder1.read();
  Serial.println(newPosition1);
  if (newPosition1 >= oldPosition1 + 4) {
    oldActPosition1 = actPosition1;
    actPosition1++;
    oldPosition1 = newPosition1;
  }
  else if (newPosition1 <= oldPosition1 - 4) {
    oldActPosition1 = actPosition1;
    actPosition1--;
    oldPosition1 = newPosition1; 
  }
  
  newPosition2 = encoder2.read();
  if (newPosition2 >= oldPosition2 + 4) {
    oldActPosition2 = actPosition2;
    actPosition2++;
    oldPosition2 = newPosition2;
  }
  else if (newPosition2 <= oldPosition2 - 4) {
    oldActPosition2 = actPosition2;
    actPosition2--;
    oldPosition2 = newPosition2; 
  }
}


//////////////// SETUP ////////////////
void setup() {
  Serial.begin(115200);
  Serial.println("booting");

  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  debounced1.attach(ENCODER_1_BUTTON_PIN);
  debounced1.interval(8);

  debounced2.attach(ENCODER_2_BUTTON_PIN);
  debounced2.interval(8);
  
  pinMode(ENCODER_1_BUTTON_PIN, INPUT_PULLUP);
  pinMode(ENCODER_2_BUTTON_PIN, INPUT_PULLUP);

  sensors.begin();
  sensors.setResolution(10); //Sets global resolution
  sensors.setWaitForConversion(false);
  sensors.requestTemperatures(); 

  lcd.setCursor(0, 0);
  lcd.print(probe01Temp, 1);
  lcd.setCursor(11, 0);
  lcd.print(probe01Temp, 1);
  lcd.setCursor(0, 2);
  lcd.print(actPosition1);
  lcd.setCursor(11, 2);
  lcd.print(actPosition2);
  
  delay(1000);
}//end setup


//////////////// MAIN LOOP ////////////////
void loop() {
  
  currentMillis = millis();
  
  if (currentMillis - tempLastMillis >= tempTimer){
    tempLastMillis = currentMillis;
     
    probe01Temp = getTemperature(Probe01Add);
    probe02Temp = getTemperature(Probe02Add);
    /* 
    Serial.print(probe01Temp);
    Serial.print("\t");
    Serial.println(probe02Temp);
    */
    lcd.setCursor(0, 0);  
    lcd.print(probe01Temp, 1);
    lcd.setCursor(11, 0);
    lcd.print(probe02Temp, 1);

    sensors.requestTemperatures(); 
  }//temp read and control timer


  if (debounced1.rose()) {
    button1RoseMillis = currentMillis;
  }
  if (debounced1.fell()) {
    button1FellMillis = currentMillis;
    button1Diff = button1FellMillis - button1RoseMillis;
  }
  
  if ( button1Diff != 0 && button1Diff < 500) {
    //Serial.println("short press");
    button1Diff = 0;
    button1RoseMillis = 0;
  }
  else if (button1RoseMillis + 1000 >= currentMillis-10 && button1RoseMillis + 1000 <= currentMillis+10 && digitalRead(ENCODER_1_BUTTON_PIN)) {
    //Serial.println("long press");
    button1RoseMillis = 0;
  }  

  
  
  
  
  updateEncoders();
  
  if (actPosition1 != oldActPosition1) {
    oldActPosition1 = actPosition1;
    
    lcd.setCursor(0, 2);
    lcd.print("    ");
    lcd.setCursor(0, 2);
    lcd.print(actPosition1); 
  }
  if (actPosition2 != oldActPosition2) {
    oldActPosition2 = actPosition2;
    
    lcd.setCursor(11, 2);
    lcd.print("    ");
    lcd.setCursor(11, 2);
    lcd.print(actPosition2); 
  }

  
  debounced1.update();
  debounced2.update();
  

}//end main loop
