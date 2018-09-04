
#include <PID_v1.h>
#include <FreqCount.h>
#include <Controllino.h>

double count = 0;

//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Specify the links and initial tuning parameters
double Kp=2, Ki=5, Kd=1;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

void setup()
{
  Serial.begin(57600);
  FreqCount.begin(1000);
  
  //initialize the variables we're linked to


  Input = FreqCount.read(); 
  Setpoint = 100000;

  //turn the PID on
  myPID.SetMode(AUTOMATIC);

  digitalWrite(CONTROLLINO_D19, HIGH);
}


void loop()
{
  if (FreqCount.available()) {
    Input = FreqCount.read();
    Serial.print("Freq: ");
    Serial.println(Input);
    myPID.Compute();
    Serial.print("Output: ");
    Serial.println(Output);
  }

}


