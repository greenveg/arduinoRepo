/* TO DO LIST
 * I-have-fixed-the-valves button functionality 
 */

#include "Arduino.h"
#include <Controllino.h>    //Controllino lib
#include <SPI.h>
#include <EEPROMex.h>
#include <Bounce2.h>

//Setup debounce objects
Bounce debounced1 = Bounce(); 

//Pins
const int PUMP_PWM_PIN = CONTROLLINO_D16;
const int BUTTON_PIN = CONTROLLINO_D22;
const int POT_PIN = CONTROLLINO_D18;


//User settings
unsigned long evacuateRestMs = 2000;

//Serial reading
const byte numChars = 32;
char receivedChars[numChars]; // an array to store the received data
int receivedInt;
boolean newData = false;


//Global vars used in functions, mainly flags
int userMinute;
unsigned long previousMillis; 
bool countDownMinutesHasRun = false;
bool waitForMsHasRun = false;
bool pumpOnFlag = false;
unsigned long waitStartedMillis = 0;
bool errorMessageSent = false;

//Button handling
unsigned long button1LastMillis;
unsigned long button1RoseMillis;
unsigned long button1FellMillis;
unsigned long button1Diff;
bool button1ShortPressDetected = false;
bool button1LongPressDetected = false;


//General vars
uint8_t state = 0;
uint32_t currentMillis;
uint32_t minute = 60000;
bool runProgram = false;
bool error = false;

unsigned long count;
uint32_t eepromCount;
uint32_t eepromStartTime;
uint32_t eepromEndTime;
const unsigned int eepromCountAddr = 350;
const unsigned int eepromStartTimeAddr = 400;
const unsigned int eepromEndTimeAddr = 450;
int pumpPwm = 100;




void countDownMinutes(int m, String message) {
  if (!countDownMinutesHasRun) {
    userMinute = m;
    waitStartedMillis = currentMillis;
    previousMillis = waitStartedMillis;
    Serial.print(userMinute);
    Serial.println(message);
    userMinute--;
    countDownMinutesHasRun = true;
  }
  
  if (currentMillis >= waitStartedMillis + m*minute) {
    countDownMinutesHasRun = false;
    Serial.println("state++");
    state++;
  }
  else if (userMinute > 0 && currentMillis >= previousMillis + minute) {
    previousMillis = currentMillis;
    Serial.print(userMinute);
    Serial.println(message);
    userMinute--;
  }
}//end countDownMinutes()


void saveRTC(uint32_t *ptr) {
  ptr[0] = Controllino_GetYear();
  ptr[1] = Controllino_GetMonth();
  ptr[2] = Controllino_GetDay();
  ptr[3] = Controllino_GetHour();
  ptr[4] = Controllino_GetMinute();
}

void buttonHandling() {
  debounced1.update();
  if (debounced1.fell()) {
    button1FellMillis = currentMillis;
  }

  if (debounced1.rose()) {
    button1RoseMillis = currentMillis;
  }
  
  button1Diff = button1RoseMillis - button1FellMillis;
  
  if ( button1Diff != 0 && button1Diff < 500) {
    //Serial.println("Button 1 short press");
    button1ShortPressDetected = true;
    button1Diff = 0;
    button1RoseMillis = 0;
  }
  else if (button1RoseMillis + 1000 >= currentMillis-10 && button1RoseMillis + 1000 <= currentMillis+10 && digitalRead(BUTTON_PIN)) {   
    //Serial.println("Button 1 long press"); 
    button1LongPressDetected = true;
    button1Diff = 0;
    button1RoseMillis = 0;
  }
}

void printDateAndTime() {
  int n = 0;
  Serial.print("Date and time: ");
  n = Controllino_GetDay();
  Serial.print(n);
  Serial.print("/");
  n = Controllino_GetMonth();
  Serial.print(n);
  Serial.print("-");
  n = Controllino_GetYear();
  Serial.print(n);
  Serial.print("   ");
  n = Controllino_GetHour();
  if (n < 10) {
    Serial.print("0");
  }
  Serial.print(n);
  Serial.print(":");
  n = Controllino_GetMinute();
  if (n < 10) {
    Serial.print("0");
  }
  Serial.println(n);
}

void printListOfCommands() {
  Serial.println(' ');
  Serial.println( __FILE__ );
  Serial.println(' ');
  Serial.println("PLEASE NOTE: line ending should be CR only!");
  Serial.println("Available commands:");
  Serial.println(' ');
  Serial.println("list = list all commands");
  Serial.println("eeprom = reads EEPROM");
  Serial.println("zero = writes 0 to EEPROM");
  Serial.println("save = writes count to EEPROM");
  Serial.println("start = starts test script");
  Serial.println("reset = stops program and resets state");
  Serial.println("rtc = reads current time and date");
  Serial.println("off = all relays set to off");
  Serial.println("relays1 = relays 0 through 3 set to on");
  Serial.println("relays2 = relays 4 through 7 set to on");
  Serial.println("[any number between 0 and 255] = sets pwm. Does not set relays");
  Serial.println(' ');
}

void waitForMs(int m, uint8_t *ptr) {
  if (!waitForMsHasRun) {
    waitStartedMillis = currentMillis;
    waitForMsHasRun = true;
  }
  if (currentMillis >= waitStartedMillis + m) {
    waitForMsHasRun = false;
    (*ptr)++;
  }
}//end countDownMinutes()

void setAllRelaysTo(bool n) {
  digitalWrite(CONTROLLINO_R2, n);
  digitalWrite(CONTROLLINO_R3, n);
  digitalWrite(CONTROLLINO_R4, n);
  digitalWrite(CONTROLLINO_R5, n);
  digitalWrite(CONTROLLINO_R6, n);
  digitalWrite(CONTROLLINO_R7, n);
  digitalWrite(CONTROLLINO_R8, n);
  digitalWrite(CONTROLLINO_R9, n);
}

void setup() {

  Controllino_RTC_init(0);

  eepromCount = EEPROM.readLong(eepromCountAddr);

  pinMode(PUMP_PWM_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(CONTROLLINO_R2, OUTPUT);
  pinMode(CONTROLLINO_R3, OUTPUT);  
  pinMode(CONTROLLINO_R4, OUTPUT);
  pinMode(CONTROLLINO_R5, OUTPUT); 
  pinMode(CONTROLLINO_R6, OUTPUT);
  pinMode(CONTROLLINO_R7, OUTPUT);
  pinMode(CONTROLLINO_R8, OUTPUT);
  pinMode(CONTROLLINO_R9, OUTPUT);
  

  //Start serial communication
  Serial.begin(9600);

  debounced1.attach(BUTTON_PIN);
  debounced1.interval(8);

  Serial.println("...booting");
  printDateAndTime();
  printListOfCommands();

  delay(1000);
}//end setup


void loop() {
  currentMillis = millis();
  recvWithEndMarker();  //Read serial
  doStuffWithData();    //Do things with received data, if you want to add a command, edit this func    
  buttonHandling();

  if (button1ShortPressDetected) {
    button1ShortPressDetected = false;
    analogWrite(PUMP_PWM_PIN, 0);
    setAllRelaysTo(0);
    state = 0;
    if(runProgram) {
      Serial.println("Emergency stop");
    }
    runProgram = false;
  }
  else if (button1LongPressDetected) {
    button1LongPressDetected = false;
    runProgram = true;
    Serial.println("Program started");
  }

  
  if (runProgram) {
    switch (state) {
      case 0:
        state++;
        break;
      case 1:
        digitalWrite(CONTROLLINO_R2, HIGH);
        digitalWrite(CONTROLLINO_R3, HIGH);
        digitalWrite(CONTROLLINO_R4, HIGH);
        digitalWrite(CONTROLLINO_R5, HIGH);
        digitalWrite(CONTROLLINO_R6, LOW);
        digitalWrite(CONTROLLINO_R7, LOW);
        digitalWrite(CONTROLLINO_R8, LOW);
        digitalWrite(CONTROLLINO_R9, LOW);
        waitForMs(500, &state);
        break;
  
      case 2:
        for (int i=0 ; i<=pumpPwm ; i++) {
          analogWrite(PUMP_PWM_PIN, i);
          delay(10);
        }
        
        Serial.println("Running pumps 1-4");
        state++;
        break;

      case 3:
        countDownMinutes(30, " minutes of first pump run left");
        break;
        
      case 4: 
        analogWrite(PUMP_PWM_PIN, 0);
        Serial.println("Pumps stopped");
        state++;
        break;
  
      case 5:
        digitalWrite(CONTROLLINO_R2, LOW);
        digitalWrite(CONTROLLINO_R3, LOW);
        digitalWrite(CONTROLLINO_R4, LOW);
        digitalWrite(CONTROLLINO_R5, LOW);
        digitalWrite(CONTROLLINO_R6, HIGH);
        digitalWrite(CONTROLLINO_R7, HIGH);
        digitalWrite(CONTROLLINO_R8, HIGH);
        digitalWrite(CONTROLLINO_R9, HIGH);
        waitForMs(500, &state);
        break;
  
     case 6:
        for (int i=0 ; i<=pumpPwm ; i++) {
          analogWrite(PUMP_PWM_PIN, i);
          delay(10);
        }
        Serial.println("Running pumps 5-8");
        state++;
        break;
     case 7:
      countDownMinutes(30, " minutes of second pump run left");
      break;
      
     case 8: 
      analogWrite(PUMP_PWM_PIN, 0);
      setAllRelaysTo(0);
      Serial.println("Program finished");
      runProgram = false;
      state = 0;
      
    }//end switch
  }//end runProgram if

  
}//end loop



void recvWithEndMarker() {
  static byte ndx = 0;
  char endMarker = '\r';
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

    if (strcmp(receivedChars, "list") == 0) {
      printListOfCommands();
    }


    else if (strcmp(receivedChars, "zero") == 0) {
      EEPROM.writeLong(eepromCountAddr, 0);
      count = 0;
      Serial.print("Address ");
      Serial.print(eepromCountAddr);
      Serial.println(" at EEPROM was zeroed");
    }

    else if (strcmp(receivedChars, "eeprom") == 0) {
      Serial.print("count = ");
      Serial.println(EEPROM.readLong(eepromCountAddr));
    }

    else if (strcmp(receivedChars, "save") == 0) {
      EEPROM.writeLong(eepromCountAddr, count);
      Serial.println(EEPROM.readLong(eepromCountAddr));
    }

    else if (strcmp(receivedChars, "start") == 0) {
      runProgram = true;
    }


    else if (strcmp(receivedChars, "reset") == 0 || strcmp(receivedChars, "stop") == 0) {
      pumpPwm = 0;
      analogWrite(PUMP_PWM_PIN, pumpPwm);
      runProgram = false;
      countDownMinutesHasRun = false;
 
      error = false;
      errorMessageSent = false;
      Serial.println("Program stopped and state resetted");
      Serial.println("------------------------------");
    }

    else if (strcmp(receivedChars, "rtc") == 0) {
      printDateAndTime();
    }

    else if (strcmp(receivedChars, "off") == 0) {
      Serial.println("All relays set to off and pwm set to 0");
      pumpPwm = 0;
      analogWrite(PUMP_PWM_PIN, pumpPwm);
      setAllRelaysTo(0);
    }

    else if (strcmp(receivedChars, "relays1") == 0) {
        Serial.println("Relay 0 through 3 set to on");
        digitalWrite(CONTROLLINO_R2, HIGH);
        digitalWrite(CONTROLLINO_R3, HIGH);
        digitalWrite(CONTROLLINO_R4, HIGH);
        digitalWrite(CONTROLLINO_R5, HIGH);
        digitalWrite(CONTROLLINO_R6, LOW);
        digitalWrite(CONTROLLINO_R7, LOW);
        digitalWrite(CONTROLLINO_R8, LOW);
        digitalWrite(CONTROLLINO_R9, LOW);
    }

        else if (strcmp(receivedChars, "relays2") == 0) {
        Serial.println("Relay 4 through 7 set to on");
        digitalWrite(CONTROLLINO_R2, LOW);
        digitalWrite(CONTROLLINO_R3, LOW);
        digitalWrite(CONTROLLINO_R4, LOW);
        digitalWrite(CONTROLLINO_R5, LOW);
        digitalWrite(CONTROLLINO_R6, HIGH);
        digitalWrite(CONTROLLINO_R7, HIGH);
        digitalWrite(CONTROLLINO_R8, HIGH);
        digitalWrite(CONTROLLINO_R9, HIGH);
    }
    
    
    else if ( receivedInt >= 0 && receivedInt <= 255) {
      pumpPwm = receivedInt;
      Serial.print("pumpPwm set to: ");
      Serial.println(pumpPwm);
      analogWrite(PUMP_PWM_PIN, pumpPwm);
    }

    newData = false;
  }
}
