#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Bounce2.h>
#include <Encoder.h>

#define HEATER_STATUS_PIN 12
#define SSR_1_PIN 13                //SSR has to be on PWM capable pin
#define ONE_WIRE_BUS_PIN A3         //OnewWire has to be on analog pin
#define PUMP_PIN 6                  //Pump has to be on PWM capable pin
#define ENC_CLK_PIN  3              //For best performance, encoder has to be on interrupt capable pins
#define ENC_DT_PIN 2                //For best performance, encoder has to be on interrupt capable pins
#define ENC_BUTTON_PIN 4



//////////////// GLOBAL VARIABLES ////////////////
unsigned int state = 1;

unsigned long currentMillis = 0;
unsigned long tempLastMillis = 0;
unsigned int tempTimer = 2000;
double probe01Temp = 0;
double probe02Temp = 0;
int oldPosition  = 1;
int newPosition = 0;
int actPosition = 0;

double setpoint = 0;
double power = 0;
int powerInt = 0;

int pumpPWM = 125;                 // Can not be below 70 (pump wont start)

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

//Setup the encoder on 2 pins
Encoder encoder1(2, 3);

// Instantiate Bounce object
Bounce debouncer = Bounce(); 



//////////////// SSR CONTROL CLASS ////////////////
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

double getTemperature(DeviceAddress deviceAddress){

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

void updateEncoder() {
  newPosition = encoder1.read();
  if (newPosition != oldPosition) {
    if (newPosition % 4 == 0 || newPosition == 0) {
      actPosition = newPosition / 4;
      Serial.println(actPosition); 
    }
    oldPosition = newPosition;
  }
}


//Instantiate SSR_Control object on pin 4 and with a windowlength of tempTimer = 2000ms
SSR_Control instans0(SSR_1_PIN, tempTimer);

void setAllDutyCycles(double dutyCycle) {
   instans0.setDutyCycle(dutyCycle);
}


//////////////// SETUP ////////////////
void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();

  //lcd.setCursor(0, 0);
  //lcd.print("...BOOTING");
  
  pinMode(ONE_WIRE_BUS_PIN, INPUT);
  pinMode(SSR_1_PIN, OUTPUT);
  pinMode(ENC_BUTTON_PIN, INPUT_PULLUP);
  pinMode(HEATER_STATUS_PIN, OUTPUT);
  pinMode(PUMP_PIN, OUTPUT);
  
  Serial.begin(9600);
  sensors.begin();

  debouncer.attach(ENC_BUTTON_PIN);
  debouncer.interval(8);

  //Zero encoder and turn off pump mosfet
  encoder1.write(0);
  digitalWrite(PUMP_PIN, 0);
  
  // set the resolution to 10 bit (Can be 9 to 12 bits .. lower is faster)
  sensors.begin();
  sensors.setResolution(10); //Sets global resolution
  sensors.setWaitForConversion(false);
  sensors.requestTemperatures(); 

  Serial.println();
  Serial.print("Number of Devices found on bus = ");  
  Serial.println(sensors.getDeviceCount());   
  sensors.requestTemperatures();  
  delay(1000);
  probe01Temp = getTemperature(Probe01Add);
  probe02Temp = getTemperature(Probe02Add);

  /*  
  Serial.print("Probe 01 temp: ");
  Serial.println(probe01Temp);
  Serial.print("Probe 02 temp: ");
  Serial.println(probe02Temp);
  Serial.println(' ');
  */
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
  
  delay(100);

} //end setup


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
    sensors.requestTemperatures();  
  }//temp read and control timer
  
  switch (state) {

    case 1: //start state
      //digitalWrite(HEATER_STATUS_PIN, LOW);
      digitalWrite(PUMP_PIN, 0);
      setAllDutyCycles(0);
      
      setpoint = actPosition;
      if (setpoint < 0) {setpoint = 0;}
      lcd.setCursor(0, 1);
      lcd.print("SP:");
      lcd.setCursor(4, 1);
      lcd.print(setpoint, 1);
      
      //Button press = next state
      if( debouncer.fell() ) {
        encoder1.write(0);
        state++;
      }
      break;
      
    case 2: //on/off reg state, pump is at pumpPWM
      digitalWrite(HEATER_STATUS_PIN, HIGH);
      analogWrite(PUMP_PIN, pumpPWM);
      
      if (probe02Temp < setpoint) {
        setAllDutyCycles(1.0);
      }
      else {
        instans0.setDutyCycle(0.0); 
      }
      
      //Button press = next state
       if( debouncer.fell() ) {
        encoder1.write(0);
        state++;
      }
      break;
      
    case 3: //power state, pump is at pumpPWM
      analogWrite(PUMP_PIN, pumpPWM);
      powerInt = actPosition;
      
      //If power is below zero, set to zero
      if (powerInt < 0) {powerInt = 0;}
      
      //Turn on heater status led if heater is on
      if (powerInt > 0) {digitalWrite(HEATER_STATUS_PIN, 1);}
      else {digitalWrite(HEATER_STATUS_PIN, 0);}
      
      power = powerInt;
      power = power/100;
      setAllDutyCycles(power);
      
     
      lcd.setCursor(0, 1);
      lcd.print("PW: ");
      lcd.setCursor(4, 1);
      lcd.print(power);
      lcd.setCursor(9, 1);
      lcd.print("  ");
      
      //Button press = next state
      if( debouncer.fell() ) {
        encoder1.write(0);
        state = 1;
      }
      break;
  }

  //SAFETY TEMP CHECK  
  if(probe01Temp > 90 || probe02Temp > 90) {
    instans0.setDutyCycle(0.0);   
  }
  
  updateLCD();
  updateEncoder();
  debouncer.update();
  instans0.updateSSR();


}//end main loop

