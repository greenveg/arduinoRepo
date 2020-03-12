/*
   http://arduino-info.wikispaces.com/Brick-Temperature-DS18B20
   Questions: terry@yourduino.com 
   V1.01  01/17/2013 ...based on examples from Rik Kretzinger
   
*/

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS_PIN 3
#define SSR_PIN 6

unsigned int state = 1;

unsigned long currentMillis = 0;
unsigned long tempLastMillis = 0;
unsigned int tempTimer = 1000;
double probe01Temp = 0;
double probe02Temp = 0;

unsigned long ssrLastMillis = 0;
unsigned int ssrFreq = 5;
unsigned int ssrDutyCycle = 50;
unsigned int count = 0;

unsigned long regLastMillis=0;
double R = 2;
double U = 0;
double U_old = 0;
double y = 0;
double regIntervall = 1000;
double Kp = 0.;



// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS_PIN);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
//Specific adresses
DeviceAddress Probe01 = { 0x28, 0x5D, 0xD8, 0x3D, 0x06, 0x00, 0x00, 0x1E }; 
DeviceAddress Probe02 = { 0x28, 0xFF, 0x1D, 0x62, 0x61, 0x15, 0x03, 0xB0 };




void setup() {
  pinMode(ONE_WIRE_BUS_PIN, INPUT);
  pinMode(SSR_PIN, OUTPUT);
  
  Serial.begin(9600);
  
  sensors.begin();
  // set the resolution to 10 bit (Can be 9 to 12 bits .. lower is faster)
  sensors.setResolution(Probe01, 10);
  sensors.setResolution(Probe02, 10);

  Serial.println();
  Serial.print("Number of Devices found on bus = ");  
  Serial.println(sensors.getDeviceCount());   
}

void loop() {
  currentMillis = millis();
  

  if (currentMillis - tempLastMillis >= tempTimer){
    tempLastMillis = currentMillis;
    
    sensors.requestTemperatures();  
    probe01Temp = getTemperature(Probe01);
    probe02Temp = getTemperature(Probe02);
     
    Serial.print(probe01Temp);
    Serial.print("\t");
    Serial.println(probe02Temp);

    if ((probe01Temp+probe02Temp)/2 < 35) {
      digitalWrite(SSR_PIN, HIGH);
    }
    else {
      digitalWrite(SSR_PIN, LOW); 
    }
    
  }//temp read timer


  if (currentMillis - ssrLastMillis >= 100){    
    ssrLastMillis = currentMillis;

    if (count > 100) {count = 0;
      digitalWrite(13, HIGH);
    }
    if (count == ssrDutyCycle) {
      digitalWrite(13, LOW);
    }
    count++;   
  }    
  
}


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


