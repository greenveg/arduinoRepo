/*
  * TO-DO:
  * 
*/
#include <Controllino.h>  
#include <Encoder.h>
#include <Bounce2.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>


/*
   * Mega 2560 interrupt pins:
   * 2, 3, 18, 19, 20, 21
   * Use Controllino Maxi pin Dout 0 & 1, must be on pinheader!
*/

//Pins
#define ENCODER_1_A_PIN 0           //Pinheader interrupt
#define ENCODER_1_B_PIN 1           //Pinheader interrupt
#define ENCODER_1_BUTTON_PIN 2      //Pinheader "digital out"
#define ONE_WIRE_BUS_PIN A11        //OnewWire has to be on analog pin
#define L1_SSR_PIN 3                //Use 24V screw terminal for SSR pins
#define L2_SSR_PIN 4
#define L3_SSR_PIN 5


//User settings
const uint32_t ssrPwmPeriod = 1000;
unsigned int tempTimer = 1000; //cannot be smaller than 187 for 10 bit resoliton on sensors
unsigned int logTimer = 10000;
uint8_t enclosureOvertemp = 65;

//General vars
uint8_t state;
int8_t power;
double settemp = 67;
unsigned long currentMillis = 0; 
unsigned long previousMillis = 0; 
unsigned long tempLastMillis;
unsigned long regTimer = 500;
unsigned long regLastMillis = 0;
String flashDate = __DATE__;
String flashName = __FILE__; 
double probe01Temp;
double probe02Temp;
double probe03Temp;


uint8_t a;
unsigned long minute = 60000;
unsigned long waitStartedMillis = 0;
int userMinute;   
bool countDownMinutesHasRun = false;
bool waitForMsHasRun = false;
bool runProgram = false;
bool runPump = false;


//Encoder vars
long oldPosition1;
long newPosition1;
long actPosition1;
long oldActPosition1;
long oldPosition2;
long newPosition2;
long actPosition2;
long oldActPosition2;

unsigned long button1LastMillis;
unsigned long button1RoseMillis;
unsigned long button1FellMillis;
unsigned long button1Diff;
bool button1ShortPressDetected = false;
bool button1LongPressDetected = false;



//////////////// PID PARAMETERS ////////////////
#define KP 0
#define KI 0
#define KA 0

double y = 0;
double e = 0;
double u = 0;
double v = 0;

double P = 0;
double I = 0;


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


//////////////// INITS ////////////////
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS_PIN);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
//Specific adresses
DeviceAddress probe01Address = { 0x28, 0x47, 0x67, 0xFB, 0x08, 0x00, 0x00, 0x6E };
DeviceAddress probe02Address = { 0x28, 0xBA, 0x16, 0xB8, 0x09, 0x00, 0x00, 0x35 };
DeviceAddress probe03Address = { 0x28, 0xFF, 0x8A, 0x61, 0xA0, 0x17, 0x05, 0x6A };

//Setup LCD screen on I2C
LiquidCrystal_I2C lcd(0x3F,20,4); // set the LCD address to 0x27 for a 16 chars and 2 line display

//Setup encoders
Encoder encoder1(ENCODER_1_A_PIN, ENCODER_1_B_PIN);

//Setup debounce objects
Bounce debounced1 = Bounce();
Bounce debounced2 = Bounce();

SSR_Control L1(L1_SSR_PIN, ssrPwmPeriod);
SSR_Control L2(L2_SSR_PIN, ssrPwmPeriod);
SSR_Control L3(L3_SSR_PIN, ssrPwmPeriod);



void setup() {
  //Start serial communication
  Serial.begin(9600);
  
  Controllino_RTC_init(0);
  Serial.println("...booting");

  lcd.init();
  lcd.backlight();
  lcd.clear();

  //attach encoder pushbutton
  debounced1.attach(ENCODER_1_BUTTON_PIN);
  debounced1.interval(8);

  Serial.print("Number of Devices found on oneWire bus = ");
  Serial.println(sensors.getDeviceCount());

  //Init ds18b20 sensors
  sensors.begin();
  sensors.setResolution(10); //Sets global resolution
  sensors.setWaitForConversion(false);
  sensors.requestTemperatures();
  Serial.print("Number of Devices found on oneWire bus = ");
  Serial.println(sensors.getDeviceCount());

  
  //Format filename at compilation and write it over serial
  String filename;
  filename = flashName.substring(flashName.lastIndexOf('/')+1);

  Serial.println(flashDate);
  Serial.println(filename);


  //Write filename and flash date on LCD
  LcdPrint(0, 0, "booting");
  delay(1000);
  ClearLcdRow(0);
  LcdPrint(0, 0, "Flash date");
  LcdPrint(1, 0, flashDate);
  LcdPrint(2, 0, "File Name");
  LcdPrint(3,0,filename);
  delay(2000);

}

void loop() {
  currentMillis = millis();

  //Temp read and reg timer
  if (currentMillis - tempLastMillis >= tempTimer){
    tempLastMillis = currentMillis;

    probe01Temp = getTemperature(probe01Address);
    probe02Temp = getTemperature(probe02Address);
    probe03Temp = getTemperature(probe03Address);

    if (probe03Temp > enclosureOvertemp) {
      SetAllDutCycles(0);
      lcd.clear();
      LcdPrint(0, 0, "ENCLOSURE OVERTEMP");
      LcdPrint(1, 0, "PLEASE REBOOT");
      LcdPrint(2, 0, "Enc T: ");
      lcd.setCursor(6, 2);
      lcd.print(probe03Temp);
      
    }

    if (probe01Temp > 105 || probe02Temp > 105) {
      SetAllDutCycles(0);
      lcd.clear();
      LcdPrint(0, 0, "HEATER OVERTEMP");
      LcdPrint(1, 0, "PLEASE SHUT DOWN");
      LcdPrint(2, 0, "T1 or T2: ");
      lcd.setCursor(10, 2);
      lcd.print(probe01Temp);      
      lcd.setCursor(16, 2);
      lcd.print(probe02Temp);
    }
    

    lcd.setCursor(4, 1);
    lcd.print(probe01Temp, 1);
    lcd.print('C');
    lcd.print(' ');
    lcd.setCursor(14, 1);
    lcd.print(probe02Temp, 1);
    lcd.print('C');
    lcd.print(' ');
    sensors.requestTemperatures();

    runTempReg();
  }//temp read and control timer

  
  
    
 
  recvWithEndMarker();  //Read serial
  doStuffWithData();    //Do things with received data, if you want to add a command, edit this function
  
}
