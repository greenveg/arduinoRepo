// Arduino standard libraries
#include "Wire.h"
#include <EEPROM.h>

// Third party libraries
#include "MenuSystem.h"
#include "Adafruit_LiquidCrystal.h"
#include "Encoder.h" // (pjrc encoder)
#include "Adafruit_MCP23017.h"

#define NUMBER_OF_PUSHERS 17
#define NUMBER_OF_BUTTONS 16
#define NUMBER_OF_SLOTS 17
#define NUMBER_OF_COINS 6

// Pin assignments
const int ENCODER_BUTTON_PIN = 18;
const int MOTOR_SENSOR_PIN = 19;
const int ENCODER_PIN_1 = 3;
const int ENCODER_PIN_2 = 2;

const int PUSHER_1 = 52;
const int PUSHER_2 = 53;
const int PUSHER_3 = 51;
const int PUSHER_4 = 49;
const int PUSHER_5 = 47;
const int PUSHER_6 = 45;
const int PUSHER_7 = 43;
const int PUSHER_8 = 41;
const int PUSHER_9 = 39;
const int PUSHER_10 = 37;
const int PUSHER_11 = 35;
const int PUSHER_12 = 33;
const int PUSHER_13 = 31;
const int PUSHER_14 = 29;
const int PUSHER_15 = 27;
const int PUSHER_16 = 25;

const int MOTOR_PIN = 23;




// I2C addresses

const int EXPANDER_3_I2C_ADDRESS = 5; //Buttons
const int EXPANDER_4_I2C_ADDRESS = 7; //Buttons
const int BUTTON_LOCK_I2C_ADDRESS = 2;
const int COIN_HATCH_I2C_ADDRESS = 3;
const int LCD_I2C_ADDRESS = 6;

// Hardware library instances
Adafruit_MCP23017 expander3;
Adafruit_MCP23017 expander4;
Encoder encoder(ENCODER_PIN_1, ENCODER_PIN_2);
Adafruit_LiquidCrystal lcd(LCD_I2C_ADDRESS);

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

//Buttons
const int BUTTONS_LOCKED_SERVO_ANGLE = 60;
const int BUTTONS_UNLOCKED_SERVO_ANGLE = 90;
bool buttonsAreLocked = false;

//Motor
int motorRunTimes = 0;

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


struct ExpanderPin {
  Adafruit_MCP23017* expander;
  int pin;
};

ExpanderPin lockButtonsPin = {&expander4, 0};

MenuSystem ms(my_renderer);

void setup() {
  pinMode(PUSHER_1, OUTPUT);
digitalWrite(52, LOW);
for (int i=53; i>=25; i=i-2) {
  pinMode(i, OUTPUT);
  digitalWrite(i, LOW);
  }
  setupLcd();
  setupEncoderButton(ENCODER_BUTTON_PIN);
  buildMenu();
  ms.display();
  setupExpanders();
  loadFromEeprom();
  loadValuesIntoMenu();

  Serial.begin(9600);
}

void loop() {
  if (currentState != nextState) {
    currentState = nextState;
    latestStateChange = millis();
    //clearSerialBuffer();
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
        nextState = STATE_RESET_EVERYTHING;
      }

      break;
    case STATE_RESET_EVERYTHING:
      showDisplay("Resetting...");
      stateTimeOut = 3000;
      if (!motorIsInParkingPosition()) {
        runMotorOneRevolution();
      }
      openReturnHatch();
      delay(2000);
      closeReturnHatch();
      rejectAllCoins();

      nextState = STATE_WAIT_FOR_BUTTON_PRESS;

      break;

    case STATE_WAIT_FOR_BUTTON_PRESS:
      showDisplay("Ready.");
      //unlockButtons();
      rejectAllCoins();

      pushedButton = readButtons();
      selectedSlot = pushedButton - 1;

      if (pushedButton && stash[selectedSlot] > 0) {
        nextState = STATE_WAIT_FOR_MONEY;
      }

      break;

    case STATE_WAIT_FOR_MONEY:
      Serial.println("state: WAIT FOR MONEY");
      showDisplay("Waiting for money...");
      stateTimeOut = 30000;

      if (buttonsAreLocked == false) {
        lockButtons();
        Serial.println("lockButtons()");
      }
      else {Serial.println("buttons already locked");}
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
  
}

long stateRunTime() {
  return millis() - latestStateChange;
}

void clearSerialBuffer() {
  while (Serial.available()) {
    Serial.read();
  }
}

void i2cServo(int address, int angle) {
  Wire.beginTransmission(address);
  Wire.write(angle);
  Wire.endTransmission();
}


//BUTTONS
void lockButtons() {
  i2cServo(BUTTON_LOCK_I2C_ADDRESS, BUTTONS_LOCKED_SERVO_ANGLE);
  //lockButtonsPin.expander->digitalWrite(lockButtonsPin.pin, HIGH);
  buttonsAreLocked = true;
}

void unlockButtons() {
  i2cServo(BUTTON_LOCK_I2C_ADDRESS, BUTTONS_UNLOCKED_SERVO_ANGLE);
  //lockButtonsPin.expander->digitalWrite(lockButtonsPin.pin, LOW);
  buttonsAreLocked = false;
  
}

int readButtons() {
  for (int i = 0; i < NUMBER_OF_BUTTONS; i++) {
    if (buttons[i].expander->digitalRead(buttons[i].pin) == LOW) {
      return i + 1;
    }
  }

  return 0;
}

//MOTOR
void runMotorOneRevolution() {
  Serial.println("runMotorOneRevolution()");
  startMotor();
  delay(200); // Wait until the sensor isn't triggering anymore
  while (!motorIsInParkingPosition());
  stopMotor();

}

void startMotor() {
  digitalWrite(MOTOR_PIN, HIGH);
}

void stopMotor() {
  digitalWrite(MOTOR_PIN, LOW);
}

bool motorIsInParkingPosition() {
  return digitalRead(MOTOR_SENSOR_PIN) == LOW;
}

