#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Bounce2.h>

#define ONE_WIRE_BUS_PIN 3
#define SSR_1_PIN 4
#define SSR_2_PIN 5
#define SSR_3_PIN 6
#define POT_1_PIN A0
#define BUTTON_PIN 12
#define STATUS_LED_PIN 9

unsigned int state = 1;

unsigned long currentMillis = 0;
unsigned long tempLastMillis = 0;
unsigned int tempTimer = 2000;
double probe01Temp = 0;
double probe02Temp = 0;
volatile int sensorVal = 0;


double setpoint = 0;
double power = 0;
int powerInt = 0;

unsigned long ssrLastMillis = 0;
unsigned int ssrFreq = 10;
unsigned int ssrDutyCycle = 50;
unsigned int count = 0;



// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS_PIN);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
//Specific adresses
DeviceAddress Probe01Add = { 0x28, 0x5D, 0xD8, 0x3D, 0x06, 0x00, 0x00, 0x1E }; 
DeviceAddress Probe02Add = { 0x28, 0xFF, 0x1D, 0x62, 0x61, 0x15, 0x03, 0xB0 };

//Setup LCD screen on I2C
LiquidCrystal_I2C lcd(0x27,20,4); // set the LCD address to 0x27 for a 16 chars and 2 line display

// Instantiate Bounce object
Bounce debouncer = Bounce(); 

class SSR_Control {
  private:  
  unsigned int _pin;
  unsigned int _windowLength;
  unsigned long _windowStartTime;
  double _dutyCycle;

  public:
  SSR_Control(int pin, int windowLength) {
    _pin = pin;
    _windowLength = windowLength;
    
    pinMode(_pin, OUTPUT);
  }    
  void updateSSR(){
    //Check if it's time to turn on
    if(currentMillis - _windowStartTime <= _windowLength * _dutyCycle) {
      digitalWrite(_pin, HIGH);  
    }
    else {
      digitalWrite(_pin, LOW);
    }
  
    //Check if windowsLength has passed, if so, set new windowStartTime
    if (currentMillis - _windowStartTime > _windowLength) {
      _windowStartTime += _windowLength;
    }
  } 
  void setDutyCycle(double newDutyCycle) {
    _dutyCycle = newDutyCycle;
  }
  double getDutyCycle(){
    return _dutyCycle;
  }
 
};//end SSR class

double getTemperature(DeviceAddress deviceAddress)
{

  double tempC = sensors.getTempC(deviceAddress);

  if (tempC == -127.00) {
    Serial.print("Error getting temperature  ");
  } 
  else {
    return tempC;
  }
}

void updateLCD() {
  lcd.setCursor(3, 0);
  lcd.print(probe01Temp, 1);
  lcd.setCursor(12, 0);
  lcd.print(probe02Temp, 1);
  lcd.setCursor(15, 1);
  lcd.print(state); 
}


//Instantiate SSR_Control object on pin 4 and with a windowlength of tempTimer = 2000ms
SSR_Control instans0(SSR_1_PIN, tempTimer);
SSR_Control instans1(SSR_2_PIN, tempTimer);
SSR_Control instans2(SSR_3_PIN, tempTimer);

void setAllDutyCycles(double dutyCycle) {
   instans0.setDutyCycle(dutyCycle);
   instans1.setDutyCycle(dutyCycle);
   instans2.setDutyCycle(dutyCycle);

}

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();

  //lcd.setCursor(0, 0);
  //lcd.print("...BOOTING");
  
  pinMode(ONE_WIRE_BUS_PIN, INPUT);
  pinMode(SSR_1_PIN, OUTPUT);
  pinMode(SSR_2_PIN, OUTPUT);
  pinMode(SSR_3_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(STATUS_LED_PIN, OUTPUT);
  
  Serial.begin(9600);
  sensors.begin();

  debouncer.attach(BUTTON_PIN);
  debouncer.interval(8);
  
  // set the resolution to 10 bit (Can be 9 to 12 bits .. lower is faster)
  sensors.setResolution(Probe01Add, 10);
  sensors.setResolution(Probe02Add, 10);

  Serial.println();
  Serial.print("Number of Devices found on bus = ");  
  Serial.println(sensors.getDeviceCount());   
  sensors.requestTemperatures();  
  probe01Temp = getTemperature(Probe01Add);
  probe02Temp = getTemperature(Probe02Add);

  
  Serial.print("Probe 01 temp: ");
  Serial.println(probe01Temp);
  Serial.print("Probe 02 temp: ");
  Serial.println(probe02Temp);
  Serial.println(' ');

  lcd.setCursor(0, 0);
  lcd.print("t1:");
  lcd.print(probe01Temp, 1);
  lcd.setCursor(9, 0);
  lcd.print("t2:"); 
  lcd.print(probe02Temp, 1);
  lcd.setCursor(0, 1);
  lcd.print("SP: ");
  lcd.setCursor(11, 1);
  lcd.print("St: ");
  
  delay(1000);

}//end setup

void loop() {
  currentMillis = millis();
  
  
  if (currentMillis - tempLastMillis >= tempTimer){
    tempLastMillis = currentMillis;
    
    sensors.requestTemperatures();  
    //probe01Temp = getTemperature(Probe01Add);
    probe02Temp = getTemperature(Probe02Add);
     
    Serial.print(probe01Temp);
    Serial.print("\t");
    Serial.println(probe02Temp);
    
  }//temp read and control timer
  
  switch (state) {

    case 1: //start state
      digitalWrite(STATUS_LED_PIN, LOW);
      setAllDutyCycles(0);
      //Serial.println("state 1");
      
      setpoint = map(analogRead(POT_1_PIN), 0, 1023, 90, 0);
      lcd.setCursor(0, 1);
      lcd.print("SP:");
      lcd.setCursor(4, 1);
      lcd.print(setpoint, 1);
      //Button press = next state
      if( debouncer.fell() ) {state++;}
      break;
      
    case 2: //on/off reg state
      digitalWrite(STATUS_LED_PIN, HIGH);
      //Serial.println("state 2");
      
      if (probe02Temp < setpoint) {
        setAllDutyCycles(1.0);
      }
      else {
        instans0.setDutyCycle(0.0); 
      }
      
      //Button press = next state
       if( debouncer.fell() ) {state++;}
      break;
      
    case 3: //power state
      //Serial.println("state 3");
      powerInt = map(analogRead(POT_1_PIN), 0, 1023, 100, 0);
      //analogWrite(STATUS_LED_PIN, map(power, 0, 100, 0, 255));
      
     power = powerInt/100;
     Serial.println(power);
     setAllDutyCycles(power);
     

      lcd.setCursor(0, 1);
      lcd.print("PW:");
      lcd.setCursor(4, 1);
      lcd.print(power);
      lcd.setCursor(9, 1);
      lcd.print("  ");
      //Button press = next state
       if( debouncer.fell() ) {state = 1;}
      break;
  }
  updateLCD();
  debouncer.update();
  instans0.updateSSR();
  instans1.updateSSR();
  instans2.updateSSR();
}//end main loop

