#include <Controllino.h>
#include <AccelStepper.h>

//Stepper pins
#define DIR_PIN CONTROLLINO_D3
#define STEP_PIN CONTROLLINO_D4
#define ENDSTOP_PIN CONTROLLINO_D5

//General vars
unsigned int state = 1;
const int maxPos = 2700;      //Stepper housing is 315 deg, with gears every step is 0.1166 deg. 315/0.1166 = 2700
const int minPos = 0;
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;

// Initialize AccelStepper class object
AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);


//Functions
void takeOneStep() {
  digitalWrite(STEP_PIN, HIGH);
  delayMicroseconds(25);
  digitalWrite(STEP_PIN, LOW);  
  delayMicroseconds(25);
  Serial.println("Took one step");
}

void printCurrentPosition() {
    Serial.print("Current position: ");
    Serial.println(stepper.currentPosition());
}


//PROGRAM

void setup(){
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(ENDSTOP_PIN, INPUT);
   
  Serial.begin(9600);

  stepper.setMaxSpeed(1200.0);
  stepper.setAcceleration(300.0);
  
}

void loop(){
  currentMillis = millis();
 
  switch (state) {
    case 1:
      Serial.println("Inside case 1");
      digitalWrite(DIR_PIN, LOW);
      printCurrentPosition();
      while(digitalRead(ENDSTOP_PIN)){
        takeOneStep();   
      }
      stepper.setCurrentPosition(0);
      printCurrentPosition();
      
      state = 2;
      break;
      
    case 2:
      Serial.println("Inside case 2");
      previousMillis = millis();
      stepper.moveTo(minPos);      
      for (int i=0 ; i<10) {
        Serial.print("For loop iteration #: ");
        Serial.println(i);
        if (!stepper.distanceToGo()) {
          stepper.moveTo(maxPos);
        }
        if (!stepper.distanceToGo()){
          stepper.moveTo(minPos);
        }
        stepper.run();
      }
      
      currentMillis = millis();
      Serial.print("Time to run 10 loops: ");
      Serial.println(currentMillis - previousMillis);
      
      break;
           
     
      
  }//end switch
  stepper.run();
}//end main loop




