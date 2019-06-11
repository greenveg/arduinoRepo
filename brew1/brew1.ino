#include <Encoder.h>
#include <Bounce2.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>

#define ENCODER_1_A_PIN 3         //Encoder1 get on of the interrupt pins
#define ENCODER_1_B_PIN 5
#define ENCODER_1_BUTTON_PIN 4
#define ENCODER_2_A_PIN 2         //Encoder2 gets the other interrupt pin
#define ENCODER_2_B_PIN 8
#define ENCODER_2_BUTTON_PIN 7
#define PUMP_PIN  6               //Has to be on pwm capable pin
#define ONE_WIRE_BUS_PIN A3       //OnewWire has to be on analog pin


// =============== Controller variables
// PID parameters
#define KP 0
#define KI 0
#define KA 0

double y = 0:
           double e = 0;
double u = 0;
double v = 0;

double P = 0;
double I = 0;

// ===============

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS_PIN);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
//Specific adresses
DeviceAddress probe01Address = { 0x28, 0xFF, 0x94, 0x19, 0xA2, 0x17, 0x04, 0x11 };
DeviceAddress probe02Address = { 0x28, 0xFF, 0x8A, 0x61, 0xA0, 0x17, 0x05, 0x6A };


//Setup LCD screen on I2C
LiquidCrystal_I2C lcd(0x3F, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display


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
bool button1ShortPressDetected = false;
bool button1LongPressDetected = false;

unsigned long button2LastMillis = 0;
unsigned long button2RoseMillis = 0;
unsigned long button2FellMillis = 0;
unsigned long button2Diff = 0;
bool button2ShortPressDetected = false;
bool button2LongPressDetected = false;

unsigned long currentMillis = 0;
unsigned long tempLastMillis = 0;
unsigned int tempTimer = 1000; //cannot be smaller than 187 for 10 bit resoliton on sensors
double probe01Temp = 0;
double probe02Temp = 0;

uint8_t state = 1;
uint8_t lastState = 5;
bool lcdWasFlashed = false;
bool runTempReg = false;
double settemp = 0;
bool enableSettempControl = true;
bool enableDirectPumpControl = true;
uint8_t pumpPWM = 0;
int8_t pumpPercentage = 0;
uint8_t defaultMashingPumpPercentage = 50;
bool pumpIsOn = false;
int8_t power = 0;
bool a = 0;
unsigned long regTimer = 500;
unsigned long regLastMillis = 0;




double getTemperature(DeviceAddress deviceAddress) {
  double tempC = sensors.getTempC(deviceAddress);
  if (tempC == -127.00) {
    Serial.print("Error getting temperature");
  }
  else {
    return tempC;
  }

}

void updateEncoders() {
  //Encoder 1
  newPosition1 = encoder1.read();
  oldActPosition1 = actPosition1;
  if (newPosition1 >= oldPosition1 + 4) {
    actPosition1++;
    oldPosition1 = newPosition1;
  }
  else if (newPosition1 <= oldPosition1 - 4) {
    actPosition1--;
    oldPosition1 = newPosition1;
  }

  //Encoder 2
  newPosition2 = encoder2.read();
  oldActPosition2 = actPosition2;
  if (newPosition2 >= oldPosition2 + 4) {
    actPosition2++;
    oldPosition2 = newPosition2;
  }
  else if (newPosition2 <= oldPosition2 - 4) {
    oldActPosition2 = actPosition2;
    actPosition2--;
    oldPosition2 = newPosition2;
  }

  //Button 1
  if (debounced1.rose()) {
    button1RoseMillis = currentMillis;
  }
  if (debounced1.fell()) {
    button1FellMillis = currentMillis;
    button1Diff = button1FellMillis - button1RoseMillis;
  }

  if ( button1Diff != 0 && button1Diff < 500) {
    Serial.println("Button 1 short press");
    button1ShortPressDetected = true;
    button1Diff = 0;
    button1RoseMillis = 0;
  }
  else if (button1RoseMillis + 1000 >= currentMillis - 10 && button1RoseMillis + 1000 <= currentMillis + 10 && digitalRead(ENCODER_1_BUTTON_PIN)) {
    Serial.println("Button 1 long press");
    button1LongPressDetected = true;
    button1RoseMillis = 0;
  }
  //Button 2
  if (debounced2.rose()) {
    button2RoseMillis = currentMillis;
  }
  if (debounced2.fell()) {
    button2FellMillis = currentMillis;
    button2Diff = button2FellMillis - button2RoseMillis;
  }

  if ( button2Diff != 0 && button2Diff < 500) {
    Serial.println("Button 2 short press");
    button2ShortPressDetected = true;
    button2Diff = 0;
    button2RoseMillis = 0;
  }
  else if (button2RoseMillis + 1000 >= currentMillis - 10 && button2RoseMillis + 1000 <= currentMillis + 10 && digitalRead(ENCODER_2_BUTTON_PIN)) {
    Serial.println("Button 2 long press");
    button2LongPressDetected = true;
    button2RoseMillis = 0;
  }

}

void ClearLcdForNewState() {
  lcd.setCursor(6, 0);
  lcd.print(' ');
  lcd.print(' ');
  lcd.print(' ');
  lcd.print(' ');
  lcd.print(' ');
  lcd.print(' ');
  lcd.print(' ');
  lcd.print(' ');
  lcd.print(' ');
  lcd.print(' ');
  lcd.print(' ');
  lcd.print(' ');
  lcd.print(' ');
  lcd.print(' ');
}

void ClearLcdRow(int row) {
  lcd.setCursor(0, row);
  lcd.print("                    ");
}

void ProgressStateChecker() {
  if (button1ShortPressDetected) {
    button1ShortPressDetected = false;
    state++;
    if (state > lastState) {
      state = 1;
    }
    lcdWasFlashed = false;
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

  Serial.print("Number of Devices found on bus = ");
  Serial.println(sensors.getDeviceCount());

  lcd.setCursor(0, 0);
  lcd.print("State: ");
  lcd.setCursor(7, 0);
  lcd.print("booting");

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

  /*

    --------------------
    State: Standby
    T1: 20.7C   T2:21.1C
    Settemp: 67C
    Pump: 0%
    --------------------

    --------------------
    State: Standby
    T1: 20.7C   T2:21.1C
    Power: 60%
    Pump: default
    --------------------

  */

  delay(1000);
}//end setup


//////////////// MAIN LOOP ////////////////
void loop() {
  currentMillis = millis();

  //Temp read timer
  if (currentMillis - tempLastMillis >= tempTimer) {
    tempLastMillis = currentMillis;

    probe01Temp = getTemperature(probe01Address);
    probe02Temp = getTemperature(probe02Address);

    Serial.print(probe01Temp);
    Serial.print("\t");
    Serial.println(probe02Temp);

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

  //Main switch
  switch (state) {
    case 1: //STANDBY STATE
      if (!lcdWasFlashed) {
        runTempReg = false;
        enableDirectPumpControl = true;
        enableSettempControl = true;
        //setAllDutyCycles(0);

        ClearLcdForNewState();
        lcd.setCursor(7, 0);
        lcd.print("STANDBY");
        lcd.setCursor(0, 2);
        lcd.print("Settemp: ");
        lcd.print(settemp, 1);
        lcd.print('C');
        lcd.setCursor(6, 3);
        lcd.print(pumpPercentage);
        lcd.print("%  ");
        lcdWasFlashed = true;
      }


      ProgressStateChecker();
      break;

    case 2: //WATER HEATING STATE
      if (!lcdWasFlashed) {
        runTempReg = true;
        enableDirectPumpControl = true;
        enableSettempControl = true;

        ClearLcdForNewState();
        lcd.setCursor(7, 0);
        lcd.print("WATER HEATING");
        lcdWasFlashed = true;
      }


      ProgressStateChecker();
      break;

    case 3: //MASH IN STATE
      if (!lcdWasFlashed) {
        runTempReg = true;
        enableDirectPumpControl = false;
        enableSettempControl = true;
        pumpIsOn = false;

        ClearLcdForNewState();
        lcd.setCursor(7, 0);
        lcd.print("MASH IN");
        lcdWasFlashed = true;
      }


      ProgressStateChecker();
      break;
    case 4: //MASHING STATE
      if (!lcdWasFlashed) {
        runTempReg = true;
        enableDirectPumpControl = false;
        enableSettempControl = true;
        pumpIsOn = true;
        analogWrite(PUMP_PIN, map(pumpPercentage, 0, 100, 0, 255));

        ClearLcdForNewState();
        lcd.setCursor(7, 0);
        lcd.print("MASHING");
        lcd.setCursor(6, 3);
        lcd.print(defaultMashingPumpPercentage);
        lcd.print('%');
        lcdWasFlashed = true;
      }

      if (button2LongPressDetected) {
        button2LongPressDetected = false;
        enableDirectPumpControl = !enableDirectPumpControl;
        if (!enableDirectPumpControl) {
          analogWrite(PUMP_PIN, map(defaultMashingPumpPercentage, 0, 100, 0, 255));
        }
      }


      ProgressStateChecker();
      break;

    case 5: //BOIL STATE
      if (!lcdWasFlashed) {
        runTempReg = false;
        enableDirectPumpControl = false;
        enableSettempControl = false;
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
        lcd.print("0% ");
        lcdWasFlashed = true;
      }

      if (actPosition1 != oldActPosition1) {
        if (actPosition1 - oldActPosition1 < 0 && power > 0) power -= 10;
        else if (actPosition1 - oldActPosition1 > 0 && power < 100) power += 10;
        Serial.println(power);
        lcd.setCursor(7, 2);
        lcd.print(power);
        lcd.print("%   ");
      }
      //setAllDutCycles(power);


      ProgressStateChecker();
      break;
  }//end switch


  if (enableSettempControl) {
    if (actPosition1 != oldActPosition1) {
      if (actPosition1 - oldActPosition1 < 0 && settemp > 0) settemp -= 1;
      else if (actPosition1 - oldActPosition1 > 0 && settemp < 100) settemp += 1;
      lcd.setCursor(9, 2);
      lcd.print(settemp, 1);
      lcd.print("C   ");
    }

  }

  if (enableDirectPumpControl) {
    //Update pumpPercentage to encoder2 position. Constained to between 0 & 100 and encoder fix.
    if (actPosition2 != oldActPosition2) {
      if (actPosition2 - oldActPosition2 < 0 && pumpPercentage > 0) pumpPercentage -= 10;
      else if (actPosition2 - oldActPosition2 > 0 && pumpPercentage < 100) pumpPercentage += 10;
      lcd.setCursor(6, 3);
      lcd.print(pumpPercentage);
      lcd.print("%   ");
    }
    if (button2ShortPressDetected) {
      button2ShortPressDetected = false;
      pumpIsOn = !pumpIsOn;
    }

  }
  if (pumpIsOn) {
    analogWrite(PUMP_PIN, map(pumpPercentage, 0, 100, 0, 255));
  }
  else {
    analogWrite(PUMP_PIN, 0);
  }

  if (runTempReg) {
    //MAGIC PID FUCKERY LIES HERE

    if (currentMillis - regLastMillis >= regTimer) {

      y = (probe01Temp + probe02Temp) / 2; // Lazy sensor fusion
      e = settemp - y;

      //TODO: INSERT ANTI-WINDUP
      P = KP * e
          I = I + KI * regTimer * e;

      // Create unbounded control signal
      v = P + I;

      // Create truncated control signal
      if (v > 1.0) {
        u = 1.0;
      } else if (v < 0.0) {
        u = 0.0;
      } else {
        u = v;
      }


      regLastMillis = currentMillis;
      a = !a;
    }
    lcd.setCursor(19, 2);
    lcd.print(a);
  }
  else {
    lcd.setCursor(19, 2);
    lcd.print(' ');
    //setAlldutyCycles(0)
  }


  updateEncoders();
  debounced1.update();
  debounced2.update();

}//end main loop



/* PSEUDO CODE INBOUND
  STANDBY
  Actuate Encoder1 = change settemp
  Long press = N/A
  Short press = next state
  WATER HEATING
  Actuate Encoder1 = change settemp
  Long press = Start temp reg, Encoder1 is now off
  Long press again = stop temp reg, Encoder1 is no on
  Short press = next state
  MASHING
  Pump is now on by default at default mashing pwm
  Long press = Pump power can now be directly set via Encoder2
  Long press again = pump power can no longer be set.
  Short press = next state
  BOIL
  Actuate Encoder1 = changes power (heater is still off)
  Long press = start heater at specified power level. Encoder1 now changes power directly
  Long press again = stop heater
  Short press = go to Standby
  ---------
  PUMP LOOP
  Actuate Encoder2 = changes pump power
  Short press = pump starts at pump power
  Short press again = pump stops
  Long press = N/A (unless in mashing state)



*/
