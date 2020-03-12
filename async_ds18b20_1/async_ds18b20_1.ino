
//
// Sample of using Async reading of Dallas Temperature Sensors
// 
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS A3

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

DeviceAddress probe1Addr = { 0x28, 0xFF, 0x94, 0x19, 0xA2, 0x17, 0x04, 0x11 }; 
DeviceAddress probe2Addr = { 0x28, 0xFF, 0x8A, 0x61, 0xA0, 0x17, 0x05, 0x6A };

uint8_t resolution = 10;
unsigned long lastTempRequest = 0;
int  delayInMillis = 0;
float temperature = 0.0;
int  idle = 0;
//
// SETUP
//
void setup(void)
{
 Serial.begin(115200);
 Serial.println("Dallas Temperature Control Library - Async Demo");
 Serial.print("Library Version: ");
 Serial.println(DALLASTEMPLIBVERSION);
 Serial.println("\n");

 sensors.begin();
 sensors.getAddress(probe1Addr, 0);
 sensors.setResolution(resolution); //Sets global resolution
 
 sensors.setWaitForConversion(false);
 sensors.requestTemperatures();
 delayInMillis = 750 / (1 << (12 - resolution)); //ca 187 for 10 bit res
 lastTempRequest = millis();
 
}

void loop(void)
{ 
 
 if (millis() - lastTempRequest >= delayInMillis) // waited long enough??
 {
   Serial.print(" Temperature: ");
   temperature = sensors.getTempCByIndex(0);
   Serial.println(temperature, resolution - 8); 
   Serial.print("  Resolution: ");
   Serial.println(resolution); 
   Serial.print("Idle counter: ");
   Serial.println(idle);     
   Serial.println(); 
   
   idle = 0; 
       
   
   sensors.requestTemperatures(); 
   lastTempRequest = millis(); 
 }
 
 // we can do usefull things here 
 // for the demo we just count the idle time in millis
 delay(1);
 idle++;
}

