#include <Controllino.h>    //Controllino lib
#include <SPI.h>
#include <PID_v1.h>

#define PRESSURE_INPUT_PIN CONTROLLINO_A0
#define PWM_OUTPUT_PIN CONTROLLINO_D11            //5V level
#define PUMP_INA_PIN CONTROLLINO_D7               //5V level
#define PUMP_INB_PIN CONTROLLINO_D9               //5V level

//PID vars
double Setpoint, Input, Output;
double Kp=1.5, Ki=6, Kd=0;

//General vars
uint32_t currentMillis;
uint32_t prevMillis;

double pressureReadings[4];
double runningAvg;

PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

void setup(){
  Serial.begin(9600);
  
  pinMode(PRESSURE_INPUT_PIN, INPUT);
  pinMode(PWM_OUTPUT_PIN, OUTPUT);
  pinMode(PUMP_INA_PIN, OUTPUT);
  pinMode(PUMP_INB_PIN, OUTPUT);
  
  //Set-and-forget direction to CW
  digitalWrite(PUMP_INA_PIN, LOW);
  digitalWrite(PUMP_INB_PIN, HIGH);
  

  Setpoint = 5;

  //turn the PID on
  myPID.SetMode(AUTOMATIC);
  //analogWrite(PWM_OUTPUT_PIN, 120);
  
}

void loop(){
  currentMillis = millis();
  
  if (currentMillis - prevMillis >= 100) {
    prevMillis = currentMillis;
    
    //Serial.println(' ');
    
    //Move values one step
    for (int i=3 ; i>0 ; i--) {
      pressureReadings[i] = pressureReadings[i-1];
      //Serial.println(pressureReadings[i]);
    }
    
    //Insert new reading at position 0
    pressureReadings[0] = analogRead(PRESSURE_INPUT_PIN)*0.03;

    //Serial.println(pressureReadings[0]);
    //Serial.println(' ');
    
   
    Input = 
    ((pressureReadings[0]+pressureReadings[1]+pressureReadings[2]+pressureReadings[3])/4);

    
    myPID.Compute();
   
    analogWrite(PWM_OUTPUT_PIN, Output);
 
    //Serial.print("Input/runningAvg ");
    Serial.print(Input);
    Serial.print(", ");
    Serial.println(pressureReadings[0]);  //  Serial.print(", ");
    //Serial.print("Output: ");
    //Serial.println(Output/20);
    
  }//end timer
  
}//end loop
