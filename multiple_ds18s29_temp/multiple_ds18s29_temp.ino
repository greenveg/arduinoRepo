/*
MultiTempSensor code by WT040
The sensor code is based on the examples from Rik Kretzinger and some other snippets of code

History:
v0.1.1  27/11/2016
v0.1.5  04/12/2016 pre-release version
v1.0.0  04/12/2017 release version
*/

//Include's
#include <OneWire.h>
#include <DallasTemperature.h>

//Constants
#define SENSOR_PIN 3
#define READ_TIMER 2adsasd000  //10 seconds
//

OneWire oneWire(SENSOR_PIN);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

//Sensor Adresses 
DeviceAddress Sensor1 = { 0x28, 0x5D, 0xD8, 0x3D, 0x06, 0x00, 0x00, 0x1E };
DeviceAddress Sensor2 = { 0x28, 0xFF, 0x1D, 0x62, 0x61, 0x15, 0x03, 0xB0 };


int amountOfSensors = 2;


//variable to check amount of sensors on bus once in a while
int counterCheckBus = 0;

void setup()
{
  // start serial port to show results
  Serial.begin(9600);
  //display_Running_Sketch();
  printProgramName();
  Serial.print("Initializing Temperature Control Library Version ");
  Serial.println(DALLASTEMPLIBVERSION);

  Serial.print("Searching for ");
  Serial.print(amountOfSensors);
  Serial.println(" sensors...");

  // Initialize the Temperature measurement library
  sensors.begin();

  // set the resolution to 10 bit (Can be 9 to 12 bits .. lower is faster)
  sensors.setResolution(Sensor1, 10);
  sensors.setResolution(Sensor2, 10);

  getSensorsOnBus();
  Serial.println("System initialized");
}

void loop()
{
  counterCheckBus++;
  delay(READ_TIMER);
  Serial.println();

  if (counterCheckBus == 360)
  {
    getSensorsOnBus();
    counterCheckBus = 0;
  }


  sensors.requestTemperatures();

  Serial.print("Sensor 1:   ");
  printTemperature(Sensor1);
  Serial.println();

  Serial.print("Sensor 2:   ");
  printTemperature(Sensor2);
  Serial.println();


}

void printTemperature(DeviceAddress deviceAddress)
{

  float tempC = sensors.getTempC(deviceAddress);

  if (tempC == -127.00)
  {
    Serial.print("Sensor error!");
  }
  else
  {
    Serial.print("C: ");
    Serial.print(tempC);
  }
}

void printProgramName() {
  String path = __FILE__;
  int slash = path.lastIndexOf('\\');
  String programName = path.substring(slash + 1);
  int dot = programName.lastIndexOf('.');
  programName = programName.substring(0, dot);

  Serial.print("\nProgram version: ");
  Serial.println(programName);
}

void getSensorsOnBus() {
  Serial.print("Number of sensors found on bus: ");
  Serial.println(sensors.getDeviceCount());
}
