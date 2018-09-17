#include <Controllino.h>    //Controllino lib
#include <EEPROM.h>         //eeprom lib


//Serial reading
const byte numChars = 32;
char receivedChars[numChars]; // an array to store the received data
int receivedInt;
boolean newData = false;

//Motor control pins
const int inaPin = CONTROLLINO_D7;        //CTRL4
const int inbPin = CONTROLLINO_D9;        //CTRL5
const int pwmPin = CONTROLLINO_D11;        //CTRL6

//Pressure sensor pins
const int pressureSensor1Pin = CONTROLLINO_A0;

//Valve pins
const int valve1Pin = CONTROLLINO_D7;

//General vars
int stateVar = 1;
int pressure1 = 0;
bool valveIsOpen = false;
int cycleNumber;
unsigned long previousMillis = 0;
int mBar;
bool runProgram;
const int addr1 = 4;
int lastSave;
int saveNumber;


void setup() {
  //EEPROM
  cycleNumber = EEPROM.read(addr1) * 100;
  saveNumber = EEPROM.read(addr1);
  lastSave = cycleNumber;

  //Motor pin setup
  pinMode(inaPin, OUTPUT);
  pinMode(inbPin, OUTPUT);
  pinMode(pwmPin, OUTPUT);

  //Valve pin setup
  pinMode(valve1Pin, OUTPUT);
  // digitalWrite(valve1Pin, LOW);

  //Analog sensor pin setup
  pinMode(pressureSensor1Pin, INPUT);

  //Start serial communication
  Serial.begin(9600);


  //Boot sequence
  runProgram = false;
  digitalWrite(valve1Pin, HIGH);
  valveIsOpen = true;
  Serial.println("...booting");
  Serial.println("Available commands are as follows:");
  Serial.println("start = Starts the filter test cycle");
  Serial.println("stop = Stops the filter test cycle");
  Serial.println("openValve = Opens the valve");
  Serial.println("closeValve = Closes the valve");
  //Serial.println("[Any number between 0 and 255] = Sets the pwm for the pump");
  Serial.println("no = Sets the pump at 0");
  Serial.println("low = Sets the pump at 10%");
  Serial.println("med = Sets the pump at 12%");
  Serial.println("high = Sets the pump 18%");
  Serial.println("readPressure = Reads the pressure and prints it over serial");
  Serial.println("cycleNumber = Prints the cycle number over serial");
  Serial.println("eepromSaveNumber = reads the stored value from eeprom");
  Serial.println("---------------------------------------------------------");
}//end setup

void loop() {
  unsigned long currentMillis = millis();
  recvWithEndMarker();  //Read serial
  doStuffWithData();    //Do things with received data

  //Safety check
  if (readPressure() >= 5000 ) {
    Serial.println("Pressure exceeded 5 bar, program was stopped");
    openValve();
    runPump(0);
    runProgram = false;
    delay(500);
  }

  //Print pressure over serial if 2sec has passed
  if (currentMillis - previousMillis >= 2000) {
    readPressure();
    Serial.print(mBar);
    Serial.println(" mBar");
    Serial.println("---------------------------------------------------------");
    previousMillis = currentMillis;
  }


  if (runProgram) {
    Serial.println("------------------");
    runPump(40);
    delay(1000);
    closeValve();
    Serial.print(readPressure());
    Serial.println("mBar");
    delay(500);
    if (readPressure() >= 3500 ) {
      Serial.print("cycleNumber: ");
      Serial.println(cycleNumber);
      Serial.print(readPressure());
      Serial.println("mBar");
      //runPump(60);
      //delay(500);
      openValve();
      delay(1000);
      cycleNumber++;
    }
    if (cycleNumber == lastSave + 100 ) {
      saveNumber++;
      EEPROM.write(addr1, saveNumber);
      lastSave = cycleNumber;
    }

  }//end of program


}//end loop

void runPump(int pwmValue) {
  if ( pwmValue <= 100 ) {
    Serial.print("runPump(");
    Serial.print(pwmValue);
    Serial.println(")");
    digitalWrite(inaPin, LOW);
    digitalWrite(inbPin, HIGH);
    analogWrite(pwmPin, pwmValue);
  }
  else {
    analogWrite(pwmPin, 0);
    Serial.println("PWM was set to high");
  }
}

void openValve() {
  if (!valveIsOpen) {
    Serial.println("openValve()");
    digitalWrite(valve1Pin, HIGH);
    delay(200);
    valveIsOpen = true;
  }
  else {
    Serial.println("Valve already open");
  }
}

void closeValve() {
  if (valveIsOpen) {
    Serial.println("closeValve()");
    digitalWrite(valve1Pin, LOW);
    delay(200);
    valveIsOpen = false;
  }
  else {
    Serial.println("Valve already closed");
  }
}

int readPressure() {
  //Serial.println("readPressure()");
  int measurementResult;
  int measurementResultmVoltage;

  // read out value from A0 screw terminal input
  measurementResult = analogRead(pressureSensor1Pin);
  //map to mV, 30400 because the voltage dividers has some overhead safety
  measurementResultmVoltage = map(measurementResult, 0, 1023, 0, 30400);
  mBar = map(measurementResultmVoltage, 0, 10000, 0, 6000);
  return mBar;

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

  //receivedInt = atoi(receivedChars);
}

void doStuffWithData() {
  if (newData == true) {

    if (strcmp(receivedChars, "openValve") == 0) {
      openValve();
    }
    else if (strcmp(receivedChars, "closeValve") == 0) {
      closeValve();
    }
    else if (strcmp(receivedChars, "readPressure") == 0) {
      Serial.print(readPressure());
      Serial.println("mBar");
    }

    else if (strcmp(receivedChars, "start") == 0) {
      runProgram = true;
    }

    else if (strcmp(receivedChars, "stop") == 0) {
      runProgram = false;
      runPump(0);
    }

    else if (strcmp(receivedChars, "no") == 0) {
      runPump(0);
    }
    else if (strcmp(receivedChars, "low") == 0) {
      runPump(25);
    }
    else if (strcmp(receivedChars, "med") == 0) {
      runPump(38);
    }
    else if (strcmp(receivedChars, "high") == 0) {
      runPump(46);
    }

    else if (strcmp(receivedChars, "eepromSaveNumber") == 0) {
      Serial.print("EEPROM save number: ");
      Serial.println(EEPROM.read(addr1));
    }
    else if (strcmp(receivedChars, "cycleNumber") == 0) {
      Serial.print("cycleNumber: ");
      Serial.println(cycleNumber);
    }


    newData = false;
  }
}
