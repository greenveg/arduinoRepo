/********************************************************
 * PID RelayOutput Example
 * Same as basic example, except that this time, the output
 * is going to a digital pin which (we presume) is controlling
 * a relay.  the pid is designed to Output an analog value,
 * but the relay can only be On/Off.
 *
 *   to connect them together we use "time proportioning
 * control"  it's essentially a really slow version of PWM.
 * first we decide on a window size (5000mS say.) we then
 * set the pid to adjust its output between 0 and that window
 * size.  lastly, we add some logic that translates the PID
 * output into "Relay On Time" with the remainder of the
 * window being "Relay Off Time"
 ********************************************************/

 /*
   http://arduino-info.wikispaces.com/Brick-Temperature-DS18B20
   Questions: terry@yourduino.com 
   V1.01  01/17/2013 ...based on examples from Rik Kretzinger
   
*/


#include <PID_v1.h>

#include <OneWire.h>
#include <DallasTemperature.h>


#define ONE_WIRE_BUS_PIN 3
#define SSR_PIN 13

//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Specify the links and initial tuning parameters
double Kp=480, Ki=0, Kd=0;

PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

int WindowSize = 5000;
unsigned long windowStartTime;

unsigned long currentMillis = 0;
unsigned long tempLastMillis = 0;
double probe01Temp = 0;
double probe02Temp = 0;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS_PIN);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
//Specific adresses
DeviceAddress Probe01 = { 0x28, 0x5D, 0xD8, 0x3D, 0x06, 0x00, 0x00, 0x1E }; 
DeviceAddress Probe02 = { 0x28, 0xFF, 0x1D, 0x62, 0x61, 0x15, 0x03, 0xB0 };



void setup(){
  Serial.begin(9600);

  windowStartTime = millis();
 

  sensors.begin();
  // set the resolution to 10 bit (Can be 9 to 12 bits .. lower is faster)
  sensors.setResolution(Probe01, 9);
  sensors.setResolution(Probe02, 9);
  
  //initialize the variables we're linked to
  Setpoint = 65.6;

  //tell the PID to range between 0 and the full window size
  myPID.SetOutputLimits(0, WindowSize);

  //turn the PID on
  myPID.SetMode(AUTOMATIC);
}

void loop(){
  currentMillis = millis();
  
  if (currentMillis - tempLastMillis >= 500){
    tempLastMillis = currentMillis;
    
    sensors.requestTemperatures();  
    probe01Temp = getTemperature(Probe01);
    probe02Temp = getTemperature(Probe02);
     
    //Serial.print(probe01Temp);
    //Serial.print("\t");
    //Serial.print(probe02Temp);

  
    Input = (probe01Temp+probe02Temp)/2;
    Serial.print(Input);
    Serial.print("\t");
    Serial.println(Setpoint);
    //Serial.print("Input: ");
    //Serial.println(Input);
    myPID.Compute();
    //Serial.print("\t");
    //Serial.println(Output);
    //Serial.print("Output: ");
    //Serial.println(Output);
  } 
  
  

   //turn the output pin on/off based on pid output

  if (millis() - windowStartTime > WindowSize)
  { //time to shift the Relay Window
    windowStartTime += WindowSize;
  }
  if (Output < millis() - windowStartTime) digitalWrite(SSR_PIN, LOW);
  else digitalWrite(SSR_PIN, HIGH);

}// end loop



double getTemperature(DeviceAddress deviceAddress)
{
  double tempC = sensors.getTempC(deviceAddress);
  if (tempC == -127.00) {
    Serial.print("Error getting temperature  ");
  } 
  else {
    return tempC;
  }
}



