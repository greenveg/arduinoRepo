#include <Controllino.h>    //Controllino lib

//UI vars
int timer = 2000;                   //How often program reads values.
int programTimer = 0;               //How long program runs for (on start command). If 0, program runs indefinitly. Value in millisec.
int programPump = 20;               //Pump pwm percentage to use in program
int shutOffPressure = 5000;         //Safety shut off pressure.int programPump = 40;       


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

//General vars
int stateVar = 1;
int pressure1 = 0;
unsigned long previousMillis = 0; 
unsigned long startTime = 0; 
int mLmin = 0;
int mBar = 0;
bool runProgram = false;
int g_pwmValue = 0;

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

  //Pressuer safety check
  if (readPressure() >= shutOffPressure ) {
    Serial.print("Pressure exceeded ");
    Serial.print(shutOffPressure);
    Serial.println("mBar, program was stopped");  
    runPump(0);
    runProgram = false;
    delay(500);
  }

  //Program timer
  if (programTimer && runProgram){
      if (currentMillis - startTime >= programTimer) {
        runProgram = false;
        runPump(0);
        startTime= 0;
        Serial.println("Program stop");
        Serial.println("---------------------------------------------------------");    
      }
  }

  //Program
  if (runProgram) {
    //runPump(programPump);
    //Print pressure over serial if 2sec has passed
    if (currentMillis - previousMillis >= timer) {
      Serial.print("Pressure: ");
      Serial.print(readPressure());
      Serial.println(" mBar");
      Serial.print("Flow: ");
      Serial.print(readFlow());
      Serial.println(" mL/min");
      Serial.print("Pump: ");
      Serial.print(receivedInt);
      Serial.println("%");
      Serial.println("---------------------------------------------------------");
      previousMillis = currentMillis;
    } 
  }//end of program


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
  int result;
  int resultmVoltage;

  result = analogRead(pressureSensor1Pin);
  //map to mV, 30400 because the voltage dividers has some overhead safety
  resultmVoltage = map(result, 0, 1023, 0, 30400); 
  //map to mBar
  mBar = map(resultmVoltage, 0, 10000, 0, 6000);
  return mBar;

}

int readFlow() {
  int result;
  int resultmVolt;

  result = analogRead(flowSensorPin);
  //map to mV, 30400 because the voltage dividers has some overhead safety
  resultmVolt = map(result, 0, 1023, 0, 30400); 
  //map to mL/min
  mLmin = ((resultmVolt/150)-4)*938;
  
  if (mLmin >= 900) {
    return mLmin;
  }
  else {
    return 0;
  }
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
      Serial.println("mBar");
    }
    
   else if(strcmp(receivedChars, "readFlow") == 0) {
      Serial.print(readFlow());
      Serial.println("mV");
    }
    
    else if(strcmp(receivedChars, "start") == 0) {
      runProgram = true;
      startTime = millis();
      //Serial.print("startTime: ");
      //Serial.println(startTime);
      Serial.println("Program start");
      Serial.println("---------------------------------------------------------"); 
    }
    
    else if(strcmp(receivedChars, "stop") == 0) {
      runProgram = false;
      runPump(0);
      Serial.println("Program stop");
      Serial.println("---------------------------------------------------------"); 
    }  
  
   else if( receivedInt >= 0 && receivedInt <= 100) {
    g_pwmValue = receivedInt;
    runPump(receivedInt); 
   }

  
   newData = false;
  }
}




