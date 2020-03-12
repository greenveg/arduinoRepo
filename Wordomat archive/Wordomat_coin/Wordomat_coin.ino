// Arduino standard libraries
#include "Wire.h"
#include "EEPROM.h"
#include "Servo.h"

// Third party libraries
#include "MenuSystem.h"
#include "Adafruit_LiquidCrystal.h"
#include "Encoder.h" // (pjrc encoder)
// #include "Adafruit_MCP23017.h"

#define NUMBER_OF_PUSHERS 16
#define NUMBER_OF_BUTTONS 16
#define NUMBER_OF_SLOTS 16
#define NUMBER_OF_COINS 6


// Pin assignments
const int MOTOR_PIN = 12;
const int NO_USE_PIN = 11;
const int MOTOR_ENCODER_PIN = 19;
const int ENCODER_BUTTON_PIN = 18;
const int COIN_HATCH_SERVO_PIN = 7;
const int COIN_SLOT_PIN = 16;
const int ENCODER_PIN_1 = 3;
const int ENCODER_PIN_2 = 2;
const int BUTTON_LOCK_SERVO_PIN = 17;

const int PUSHER_PINS[] = {
  53,
  51,
  49,
  47,
  45,
  43,
  41,
  39,
  
  37,
  35,
  33,
  31,
  29,
  27,
  25,
  23
};

const int BUTTON_PINS[] = {
  A0, 
  A1,
  A2,
  A3,
  A4,
  A5,
  A6,
  A7,
  A8,
  A9,
  A10,
  A11,
  A12,
  A13,
  A14,
  A15
};
  


// I2C addresses
const int LCD_I2C_ADDRESS = 6;
const int COIN_SLOT_I2C_ADDRESS = 4;

Encoder encoder(ENCODER_PIN_1, ENCODER_PIN_2);
Adafruit_LiquidCrystal lcd(LCD_I2C_ADDRESS);
Servo coinHatchServo;
Servo buttonLockServo;

// System data variables
float prices[NUMBER_OF_SLOTS];
int stash[NUMBER_OF_SLOTS];
float coinValues[NUMBER_OF_COINS];

// User interface variables
String latestMessage = "";

// User input variables
int latestReceivedCoin = 0;
volatile bool encoderPushed = false;
int selectedSlot = 0;
int pushedButton = 0;
float paidAmount = 0.0;


// States
const int STATE_ERROR_HALT = 0;
const int STATE_RESET_EVERYTHING = 1;
const int STATE_WAIT_FOR_BUTTON_PRESS = 2;
const int STATE_WAIT_FOR_MONEY = 3;
const int STATE_BEGIN_DELIVERY = 4;
const int STATE_WAIT_FOR_MOTOR = 7;
const int STATE_END_DELIVERY = 8;
const int STATE_FINISH_SALE = 9;
const int STATE_HANDLE_COIN = 10;
const int STATE_STARTUP_DELAY = 11;

// State variables
int currentState = STATE_STARTUP_DELAY;
int nextState = STATE_STARTUP_DELAY;
int stateTimeOut = 30000;
long latestStateChange = 0;

bool motorEncoderHasTriggered = false;

unsigned long previousMillis = 0;
unsigned long lastTimeViktor = 0;

int k = 51;
//int k = 53;

// MENU SYSTEM
class LcdRenderer : public MenuComponentRenderer
{
  public:
    virtual void render(Menu const& menu) const
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      menu.get_current_component()->render(*this);
    }
    virtual void render_menu_item(MenuItem const& menu_item) const
    {
      lcd.write(byte(0));
      lcd.print(menu_item.get_name());
    }
    virtual void render_back_menu_item(BackMenuItem const& menu_item) const
    {
      lcd.print(menu_item.get_name());
    }
    virtual void render_numeric_menu_item(NumericMenuItem const& menu_item) const
    {
      if (!menu_item.has_focus()) {
        lcd.write(byte(0));
      }
      lcd.print(menu_item.get_name());
      lcd.setCursor(0, 1);
      if (menu_item.has_focus()) {
        lcd.write(byte(0));
      }
      lcd.print(menu_item.get_value_string());
    }
    virtual void render_menu(Menu const& menu) const
    {
      lcd.print(menu.get_name());
    }
};
LcdRenderer my_renderer;
MenuSystem ms(my_renderer);


//SETUP
void setup() {
  setupLcd();
  setupEncoderButton(ENCODER_BUTTON_PIN);
  buildMenu();
  ms.display();
  loadFromEeprom();
  loadValuesIntoMenu();

  // Setup pusher pins
  for (int i = 0; i < NUMBER_OF_PUSHERS; i++) {
    pinMode(PUSHER_PINS[i], OUTPUT);
    digitalWrite(PUSHER_PINS[i], HIGH);
  }
  //Setup buttons pins
  for (int i = 0; i < NUMBER_OF_BUTTONS; i++) {
    pinMode(BUTTON_PINS[i], INPUT_PULLUP);
  }

  //Setup motor pins
  pinMode(MOTOR_PIN, OUTPUT);
  digitalWrite(MOTOR_PIN, LOW);
  pinMode(MOTOR_ENCODER_PIN, INPUT);

  //Setup no use right now pin
  pinMode(NO_USE_PIN, OUTPUT);
  digitalWrite(NO_USE_PIN, LOW);
  
  //Servo
  coinHatchServo.attach(COIN_HATCH_SERVO_PIN);
  buttonLockServo.attach(BUTTON_LOCK_SERVO_PIN);

  // For debugging & control
  Serial.begin(9600);

  // For coin slot (Serial 3 RX is on pin 15)
  Serial3.begin(4800);
}

void loop() {
  if((millis() - lastTimeViktor) > 1000) {
    Serial.println("A Live");

    lastTimeViktor = millis();
    }
  if (currentState != nextState) {
    currentState = nextState;
    latestStateChange = millis();

    onStateChange();
  }

  if (stateRunTime() > stateTimeOut) {
    switch (currentState) {
      case STATE_WAIT_FOR_BUTTON_PRESS: // Waiting state, can't time out
        break;
      case STATE_RESET_EVERYTHING: // If resetting times out, halt everything
        currentState = STATE_ERROR_HALT;
        break;
      default:
        currentState = STATE_RESET_EVERYTHING;
    }
  }

  switch (currentState) {
    case STATE_STARTUP_DELAY:
      showDisplay("Starting...");

      if (stateRunTime() > 1000) {
        //assureMotorIsInParkingPosition();
        nextState = STATE_RESET_EVERYTHING;
      }

      break;
    case STATE_RESET_EVERYTHING:
      showDisplay("Resetting...");
      stateTimeOut = 3000;

      unlockButtons();
      openReturnHatch();
      delay(2000);
      closeReturnHatch();
      rejectAllCoins();

      nextState = STATE_WAIT_FOR_BUTTON_PRESS;

      break;

    case STATE_WAIT_FOR_BUTTON_PRESS:
      showDisplay("Ready.");
      unlockButtons();
      rejectAllCoins();

      pushedButton = readButtons();
      selectedSlot = pushedButton - 1;

      if (pushedButton && stash[selectedSlot] > 0) {
        nextState = STATE_WAIT_FOR_MONEY;
      }

      break;

    case STATE_WAIT_FOR_MONEY:
      showDisplay("Waiting for money...");
      stateTimeOut = 30000;

      lockButtons();
      acceptCoins();

      if (paidAmount >= prices[selectedSlot]) {
        nextState = STATE_BEGIN_DELIVERY;
      }

      if (coinReceived()) {
        nextState = STATE_HANDLE_COIN;
      }

      break;

    case STATE_HANDLE_COIN:
      showDisplay("Handling coin...");
      stateTimeOut = 500;

      paidAmount += coinValues[latestReceivedCoin];

      Serial.print("Paid amount: ");
      Serial.print(paidAmount);
      Serial.print(" Price: ");
      Serial.println(prices[selectedSlot]);

      nextState = STATE_WAIT_FOR_MONEY;

      break;

    case STATE_BEGIN_DELIVERY:
      showDisplay("Beginning delivery...");
      stateTimeOut = 5000;

      rejectAllCoins();
      setPusher(selectedSlot, HIGH);
      delay(1000);
      startMotor();

      nextState = STATE_WAIT_FOR_MOTOR;

      break;

    case STATE_WAIT_FOR_MOTOR:
      showDisplay("Waiting for motor...");
      stateTimeOut = 10000;

      if (stateRunTime() > 100 && motorIsInParkingPosition()) {
        nextState = STATE_END_DELIVERY;
      }

      break;

    case STATE_END_DELIVERY:
      showDisplay("Ending delivery...");
      stateTimeOut = 500;

      stopMotor();
      setPusher(selectedSlot, LOW);

      nextState = STATE_FINISH_SALE;

      break;

    case STATE_FINISH_SALE:
      showDisplay("Finishing sale...");
      stateTimeOut = 500;

      collectPayment();
      stash[selectedSlot] -= 1;
      writeToEeprom();
      loadValuesIntoMenu();

      nextState = STATE_WAIT_FOR_BUTTON_PRESS;
      break;

    case STATE_ERROR_HALT:
      showDisplay("Error, full stop.");
      stopMotor();

      while (1);
  }

  //  encoderHandler(encoder);

  /*
     CODE MOVED from encoderHandler(), something is messed up with encoder.write() when called from a function
  */

  long encoderPosition = encoder.read() / 4;


  if (encoderPosition != 0) {
    encoder.write(0);
  }

  if (encoderPosition > 0) {
    for (int ie = 0; ie < encoderPosition; ie++) {
      ms.next();
    }
  } else if (encoderPosition < 0) {
    for (int ie = 0; ie > encoderPosition; ie--) {
      ms.prev();
    }
  }

  if (encoderPosition != 0) {
    ms.display();
  }

  if (encoderPushed) {
    encoderPushed = false;
    ms.select();
    ms.display();
  }

  /*
     End of code from encoderHandler()
  */
}

void showDisplay(String message) {
  if (message != latestMessage) {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print(message);

    Serial.println(message);
    latestMessage = message;
  }
}

void setPusher(int number, int state) {
  digitalWrite(PUSHER_PINS[number], state);
  //pushers[number].expander->digitalWrite(pushers[number].pin, state);
}

void setAllPushersToLow() {
  for (int i = 0; i < NUMBER_OF_PUSHERS; i++) {
    setPusher(i, LOW);
  }
}

long stateRunTime() {
  return millis() - latestStateChange;
}

void clearSerialBuffer() {
  while (Serial.available()) {
    Serial.read();
  }
}

void clearSerial3Buffer() {
  while (Serial3.available()) {
    Serial3.read();
  }
}

/*
  void i2cServo(int address, int angle) {
  Wire.beginTransmission(address);
  Wire.write(angle);
  Wire.endTransmission();
  }
*/

void onStateChange() {
  clearSerialBuffer();
  stopMotor(); // Just in case
}

void onMotorEncoder() {
  stopMotor();
  motorEncoderHasTriggered = true;
}

void updateAllPushers() {
  for (int i = 0; i < NUMBER_OF_PUSHERS; i++) {
    if (pushedButton && selectedSlot == i) {
      setPusher(i, HIGH);
    } else {
      setPusher(i, LOW);
    }
  }
}

