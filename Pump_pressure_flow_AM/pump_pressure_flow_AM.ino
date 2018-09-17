#include <Controllino.h>    //Controllino lib

//Serial reading
const byte numChars = 32;
char receivedChars[numChars]; // an array to store the received data
int receivedInt;
boolean newData = false;

//Motor control pins
const int inaPin = CONTROLLINO_D1;        //CTRL4
const int inbPin = CONTROLLINO_D3;        //CTRL5
const int pwmPin = CONTROLLINO_D5;        //CTRL6

//Sensor pins
const int pressureSensor1Pin = CONTROLLINO_A0;
const int flowSensorPin = CONTROLLINO_A2;

//HOW OFTEN PROGRAM READS IN MILLISECONDS
int timer = 2000;

//General vars
int stateVar = 1;
int pressure1 = 0;
int flow = 0;
unsigned long previousMillis = 0; 
unsigned long startTime = 0;

int mBar = 0;
int mLMin = 0;
bool runProgram = false;


void setup() {
  
  //Motor pin setup
  pinMode(inaPin, OUTPUT);
  pinMode(inbPin, OUTPUT);
  pinMode(pwmPin, OUTPUT);
  
  //Analog sensor pin setup
  pinMode(pressureSensor1Pin, INPUT); 
  pinMode(flowSensorPin, INPUT);

  //Start serial communication
  Serial.begin(9600);


  //Boot sequence
  runProgram = false;

  Serial.println("...booting");
  Serial.println("PLEASE NOTE: line ending should be New Line only!");
  Serial.println("Available commands are as follows:");
  Serial.println("start = Starts reading pressure and flow every 2 seconds.");
  Serial.println("stop = Stops reading pressure, flow and stops the pump");
  Serial.println("[Any number between 0 and 100] = Sets the % pwm for the pump");
  Serial.println("readPressure = Reads the pressure and prints it over serial");
  Serial.println("readFlow = Reads the flow and prints it over serial.");
  Serial.println("---------------------------------------------------------");
}//end setup

void loop() {
  unsigned long currentMillis = millis();  
  recvWithEndMarker();  //Read serial
  doStuffWithData();    //Do things with received data

  //Safety check
  if (readPressure() >= 5000 ) {
    Serial.println("Pressure exceeded 5 bar, program was stopped");  
    runPump(0);
    runProgram = false;
    delay(500);
  }

  if (runProgram) {
    //Print pressure over serial if 2sec has passed
    runPump(40);
    if (currentMillis - previousMillis >= timer) {
      Serial.print("Pressure: ");
      Serial.print(readPressure());
      Serial.println(" mBar");
      Serial.print("Flow: ");
      Serial.print(readFlow());
      Serial.println(" mL/min");
      Serial.println("---------------------------------------------------------");
      previousMillis = currentMillis;
    }
  }
  
}//end loop

void runPump(int pwmValue) {
  if( pwmValue <= 100 ) {
    digitalWrite(inaPin, LOW);
    digitalWrite(inbPin, HIGH);
    pwmValue = map(pwmValue, 0, 100, 0, 255);
    analogWrite(pwmPin, pwmValue);
    /*
    Serial.print("runPump(");
    Serial.print(pwmValue);
    Serial.println(")");
    Serial.println("---------------------------------------------------------");
    */
  }
  else {
    analogWrite(pwmPin, 0);
    Serial.println("PWM was set to high");
  }
}

int readPressure() {
  //Serial.println("readPressure()");
  int measurementResult;
  int measurementResultmVolt;

  // read out value from A0 screw terminal input
  measurementResult = analogRead(pressureSensor1Pin);
  //map to mV, 30400 because the voltage dividers has some overhead safety
  measurementResultmVolt = map(measurementResult, 0, 1023, 0, 30400); 
  mBar = map(measurementResultmVolt, 0, 10000, 0, 6000);
  return mBar;

}

int OLDreadFlow() {
  int measurementResult;
  int measurementResultmVolt;

  measurementResult = analogRead(flowSensorPin);
  // calculate mV to litre/minute: ((mV/150)-0,004)*0,938
  measurementResultmVolt = map(measurementResult, 0, 1023, 0, 30400); 
  
  return measurementResultmVolt;
  
  //Lmin = ((measurementResultmVoltage/150)-0.004)*938;
  //return Lmin;
  
}

int readFlow() {
  int measurementResultArr[9];
  int measurementResultInt;
  float measurementResultFloat;
  int measurementResultmVolt;
  float sum = 0;

  for (int i = 0 ; i <= 9 ; i++) {
    measurementResultArr[i] = analogRead(flowSensorPin);
  }

  for (int i = 0; i <= 9 ; i++) {
   sum = sum + measurementResultArr[i];
  }

  measurementResultFloat = sum/10;
  measurementResultInt = round(measurementResultFloat);
  measurementResultmVolt = map(measurementResultInt, 0, 1023, 0, 30400); 
  
  //return measurementResultmVolt;

  mLMin = ((measurementResultmVolt/160)-0.004)*938;
  return mLMin;
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

 receivedInt = atoi(receivedChars);
}

void doStuffWithData() {
  if (newData == true) {

    if(strcmp(receivedChars, "readPressure") == 0) {
      Serial.print(readPressure());
      Serial.println(" mBar");
    }
    
   else if(strcmp(receivedChars, "readFlow") == 0) {
      Serial.print(readFlow());
      Serial.println(" ml/minute");
    }
    
    else if(strcmp(receivedChars, "start") == 0) {
      runProgram = true;
      startTime = millis();
    }
    
    else if(strcmp(receivedChars, "stop") == 0) {
      runProgram = false;
      runPump(0);
      Serial.println("stop");
    }  
  
   else if( receivedInt >= 0 && receivedInt <= 100) {
    runPump(receivedInt); 
   }

  
   newData = false;
  }
}




