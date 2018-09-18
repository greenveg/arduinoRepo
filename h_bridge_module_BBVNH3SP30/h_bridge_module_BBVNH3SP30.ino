#include "Arduino.h"
#include <Controllino.h>    //Controllino lib


//Pins
#define PUMP_PWM_PIN CONTROLLINO_D11      //5V level has to be pwm capable
#define PUMP_INB_PIN CONTROLLINO_D9       //5V level
#define PUMP_INA_PIN CONTROLLINO_D7       //5V level


//H-bridge module ENA and ENB pins are not used
//https://www.olimex.com/Products/Robot-CNC-Parts/MotorDrivers/BB-VNH3SP30/open-source-hardware
//For Controllino Mega  D11, 9 and 7 are recommended, they are on the same column on the pinheader

//Serial reading
const byte numChars = 32;
char receivedChars[numChars]; // an array to store the received data
int receivedInt;
boolean newData = false;

bool pumpOnFlag = false;
uint8_t pumpPwm = 0;


void printListOfCommands() {
  Serial.println(' ');
  Serial.println("PLEASE NOTE: line ending should be CR only!");
  Serial.println("Available commands:");
  Serial.println(' ');
  Serial.println("list = list all commands");
  //Serial.println("pump = turns pump on or off");
  Serial.println("[number between 0 & 255] sets pump PWM");
  Serial.println(' ');
}

void setup() {
  pinMode(PUMP_INA_PIN, OUTPUT);
  pinMode(PUMP_INB_PIN, OUTPUT);
  pinMode(PUMP_PWM_PIN, OUTPUT);

  // CW
  digitalWrite(PUMP_INA_PIN, LOW);
  digitalWrite(PUMP_INB_PIN, HIGH);

  //Start serial communication
  Serial.begin(9600);
  Serial.println("...booting");
  printListOfCommands();

}

void loop() {
  recvWithEndMarker();  //Read serial
  doStuffWithData();    //Do things with received data, if you want to add a command, edit this func

}


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

    /*
      else if (strcmp(receivedChars, "pump") == 0) {
        if (pumpOnFlag == false) {
          analogWrite(PUMP_PWM_PIN, pumpPwm);
          Serial.println("Pump turned ON");
          pumpOnFlag = true;
        }
        else if (pumpOnFlag == true) {
          analogWrite(PUMP_PWM_PIN, 0);
          Serial.println("Pump turned OFF");
          pumpOnFlag = false;
        }
      }
    */

    else if ( receivedInt >= 0 && receivedInt <= 255) {
      pumpPwm = receivedInt;
      analogWrite(PUMP_PWM_PIN, pumpPwm);
      Serial.print("PWM set to: ");
      Serial.println(receivedInt);
    }
    else {
      Serial.println("Not a valid command");
    }

    newData = false;
  }
}
