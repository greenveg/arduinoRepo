#include <Controllino.h>
#include <AccelStepper.h>

//Stepper pins
#define DIR_PIN CONTROLLINO_D3
#define STEP_PIN CONTROLLINO_D4
#define ENDSTOP_PIN CONTROLLINO_D5
//Valve pin
#define VALVE_PIN = CONTROLLINO_D19

//General vars
unsigned int state = 1;
const int maxPos = 2700;      //Stepper housing is 315 deg, with gears every step is 0.1166 deg. 315/0.1166 = 2700
const int minPos = 20;

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




void setup(){
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(ENDSTOP_PIN, INPUT);
   
  Serial.begin(9600);

  stepper.setMaxSpeed(1000.0);
  stepper.setAcceleration(200.0);
  
}

void loop(){
  
  switch (state) {
    case 1:
      Serial.println("Inside case 1");
      digitalWrite(DIR_PIN, LOW);
      while(digitalRead(ENDSTOP_PIN) == 1){
        takeOneStep();   
      }
      stepper.setCurrentPosition(0);
      
      state = 2;
      break;
      
    case 2:
      stepper.moveTo(maxPos);                 
      //stepper.setSpeed(500);

      state = 3;
      break;
      
    case 3:
      if (stepper.distanceToGo() == 0) {
        stepper.moveTo(minPos);
        //stepper.setSpeed(500);
        state = 4;
      }
      
      break;          
    case 4:
      if (stepper.distanceToGo() == 0) {
        stepper.moveTo(maxPos);
        //stepper.setSpeed(500);
        state = 3;
      }
      
  }//end switch
  stepper.run();
}//end main loop




