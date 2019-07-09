/*
   * TO DO:
   * Print to screen if temp values are weird
   * Shift to lcdPrint() or Johnnys fancy solution
   * Insert windup/down for pump on/off
   * Better overall error handling
   * Minimum pump pwm is 20
   * Write ON/OFF for pump on LCD
   * 
*/

  #include <Encoder.h>
  #include <Bounce2.h>
  #include <Wire.h>
  #include <OneWire.h>
  #include <DallasTemperature.h>
  #include <LiquidCrystal_I2C.h>
  
  //Pins
  #define ENCODER_1_A_PIN 3         //Encoder1 get on of the interrupt pins
  #define ENCODER_1_B_PIN 5
  #define ENCODER_1_BUTTON_PIN 4
  #define ENCODER_2_A_PIN 2         //Encoder2 gets the other interrupt pin
  #define ENCODER_2_B_PIN 8
  #define ENCODER_2_BUTTON_PIN 7
  #define PUMP_PIN  6               //Has to be on pwm capable pin
  #define ONE_WIRE_BUS_PIN A3       //OnewWire has to be on analog pin
  #define L1_SSR_PIN 9
  #define L2_SSR_PIN 10
  #define L3_SSR_PIN 11
  
  //States
  #define STANDBY_STATE 0
  #define WATER_HEATING_STATE 1
  #define PAUSE_STATE 2
  #define MASHING_STATE 3
  #define BOIL_STATE 4
  
  
  
  
  //////////////// GLOBAL VARIABLES ////////////////
  //Settings
  const uint32_t ssrPwmPeriod = 1000;
  unsigned int tempTimer = 1000; //cannot be smaller than 187 for 10 bit resoliton on sensors
  unsigned int logTimer = 10000;
  uint8_t defaultMashingPumpPercentage = 50;
  uint8_t enclosureOvertemp = 65;

  //General vars
  uint8_t state;
  bool lcdWasFlashed = false;
  //bool runTempReg = false;
  double settemp = 67;
  bool enableSettempControl = true;
  bool enableDirectPumpControl = true;
  uint8_t pumpPWM;
  int8_t pumpPercentage;
  bool pumpIsOn = false;
  int8_t power;
  bool a = 0; //is for displaying a blinking 1 to let user know temp reg function is running as intended
  unsigned long regTimer = 500;
  unsigned long regLastMillis = 0;
  String flashDate = __DATE__;
  String flashName = __FILE__; 


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
  
  unsigned long button2LastMillis;
  unsigned long button2RoseMillis;
  unsigned long button2FellMillis;
  unsigned long button2Diff;
  bool button2ShortPressDetected = false;
  bool button2LongPressDetected = false;
  
  unsigned long currentMillis;
  unsigned long tempLastMillis;
  unsigned long logLastMillis;
  double probe01Temp;
  double probe02Temp;
  double probe03Temp;

  
  
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
  Encoder encoder2(ENCODER_2_A_PIN, ENCODER_2_B_PIN);
  
  //Setup debounce objects
  Bounce debounced1 = Bounce();
  Bounce debounced2 = Bounce();
  
  SSR_Control L1(L1_SSR_PIN, ssrPwmPeriod);
  SSR_Control L2(L2_SSR_PIN, ssrPwmPeriod);
  SSR_Control L3(L3_SSR_PIN, ssrPwmPeriod);
  
  

  
  
  //////////////// SETUP ////////////////
  void setup() {
    Serial.begin(9600);
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
    pinMode(L1_SSR_PIN, OUTPUT);
    pinMode(L2_SSR_PIN, OUTPUT);
    pinMode(L3_SSR_PIN, OUTPUT);

    //Init
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

    SerialWriteCsvHeader();
  
    //Write filename and flash date on LCD
    LcdPrint(0, 0, "booting");
    delay(1000);
    ClearLcdRow(0);
    LcdPrint(0, 0, "Flash date");
    LcdPrint(1, 0, flashDate);
    LcdPrint(2, 0, "File Name");
    LcdPrint(3,0,filename);
    delay(2000);
    lcd.clear();

    //Write base setup on LCD
    lcd.setCursor(0, 1);
    lcd.print("T1: ");
    lcd.setCursor(4, 1);
    lcd.print('-');
    lcd.setCursor(11, 1);
    lcd.print("T2: ");
    lcd.setCursor(14, 1);
    lcd.print('-');
  
    lcd.setCursor(0, 2);
    lcd.print("Settemp: ");
    lcd.setCursor(9, 2);
    lcd.print("0.0C");
    lcd.setCursor(0, 3);
    lcd.print("Pump: ");
    lcd.setCursor(6, 3);
    lcd.print("0%");
  
    delay(1000);
  }//end setup
  
  
  //////////////// MAIN LOOP ////////////////
  void loop() {
    currentMillis = millis();
  
    //Temp read timer
    if (currentMillis - tempLastMillis >= tempTimer){
      tempLastMillis = currentMillis;
  
      probe01Temp = getTemperature(probe01Address);
      probe02Temp = getTemperature(probe02Address);
      probe03Temp = getTemperature(probe03Address);

      if (probe03Temp > enclosureOvertemp) {
        state = STANDBY_STATE;
        SetAllDutCycles(0);
        analogWrite(PUMP_PIN, 0);
        lcd.clear();
        LcdPrint(0, 0, "ENCLOSURE OVERTEMP");
        LcdPrint(1, 0, "PLEASE REBOOT");
        LcdPrint(2, 0, "Enc T: ");
        lcd.setCursor(6, 2);
        lcd.print(probe03Temp);
        
      }

      if (probe01Temp > 105 || probe02Temp > 105) {
        state = STANDBY_STATE;
        SetAllDutCycles(0);
        analogWrite(PUMP_PIN, 0);
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
    }//temp read and control timer

    
    //Log timer
    if (currentMillis - logLastMillis >= logTimer) {
      logLastMillis = currentMillis;

      //Add proper csv logging (state, settemp, temp1, temp2, eclosure temp e, output, pwm, timer)
      Serial.print(state);   
      Serial.print(",\t");
      Serial.print(settemp);
      Serial.print(",\t");
      Serial.print(probe01Temp);
      Serial.print(",\t");
      Serial.print(probe02Temp);
      Serial.print(",\t");
      Serial.print(probe03Temp);
      Serial.print(",\t");
      Serial.print(e);
      Serial.print(",\t");
      Serial.print(u);
      Serial.print(",\t");
      Serial.println(pumpPWM);
        
    }
    
    
    //Main switch
    switch (state) {
      case STANDBY_STATE:
        if(!lcdWasFlashed) {
          ClearLcdForNewState();
          LcdPrint(0,0, "STATE: STANDBY");
          lcd.setCursor(0, 2);
          lcd.print("Settemp: ");
          lcd.print(settemp, 1);
          lcd.print('C');
          lcd.setCursor(6, 3);
          lcd.print(pumpPercentage);
          lcd.print("%  ");
          lcdWasFlashed = true;
        }
  
        enablePumpUI();
        enableTempUI();
        break;
  
      case WATER_HEATING_STATE:
        if(!lcdWasFlashed) {
          ClearLcdForNewState();
          lcd.setCursor(7, 0);
          lcd.print("WATER HEATING");
          lcdWasFlashed = true;
        }
        
        enablePumpUI();
        enableTempUI();
        runTempReg();
        break;
  
      case PAUSE_STATE:
        if(!lcdWasFlashed) {
          ClearLcdForNewState();
          lcd.setCursor(7, 0);
          lcd.print("PAUSE");
          pumpIsOn = false;
          analogWrite(PUMP_PIN, 0);
          lcdWasFlashed = true;
        }
        enableTempUI();
        enablePumpUI();
        break;
        
      case MASHING_STATE:
        if(!lcdWasFlashed) {
          pumpIsOn = true;
          analogWrite(PUMP_PIN, map(pumpPercentage, 0, 100, 0, 255));
  
          ClearLcdForNewState();
          lcd.setCursor(7, 0);
          lcd.print("MASHING");
          lcd.setCursor(6, 3);
          lcd.print(defaultMashingPumpPercentage);
          lcd.print("%   ");
          lcdWasFlashed = true;
        }
        
        enablePumpUI();
        enableTempUI();
        runTempReg();
        break;
  
        case BOIL_STATE:
          if(!lcdWasFlashed) {
          pumpIsOn = false;
          power = 0;
  
          ClearLcdForNewState();
          lcd.setCursor(7, 0);
          lcd.print("BOIL");
          ClearLcdRow(2);
          lcd.setCursor(0, 2);
          lcd.print("Power:");
          lcd.setCursor(7, 2);
          lcd.print(power);
          lcd.print("% ");
          lcd.setCursor(6, 3);
          lcd.print("0%   ");
          lcdWasFlashed = true;
          }
  
          if (actPosition1 != oldActPosition1) {
            if(actPosition1 - oldActPosition1 < 0 && power > 0) power-=10;
            else if(actPosition1 - oldActPosition1 > 0 && power < 100) power+=10;
            lcd.setCursor(7, 2);
            lcd.print(power);
            lcd.print("%     ");       
          }
          SetAllDutCycles((double)power/100);

          enablePumpUI();
          break;
    }//end switch
    //if new states are added, ProgressStateChecker function must be updated
    ProgressStateChecker();
  
  
    updateEncoders();
    updateAllSSR();
    debounced1.update();
    debounced2.update();
  
  }//end main loop
  
  
  
