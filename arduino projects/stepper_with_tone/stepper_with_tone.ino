/*
 * PREFACE
 * To be used with the CWD556 stepper driver or similar.
 * 
 * The arduino tone() function generates a square wave of the specified frequency.
 * Minimum frequency for the Uno is 31Hz = 31 pulses per second = 1 860 pulses per minute.
 * This means we have to put the CWD556 driver needs to have a pretty high pulse per rev setting. 
 * If we choose 25600 pulses per rev we get the following frequencies for 0.1rpm to 10rpm
 * 
 * 25600 steps/rev
 * 
 * 0.1 rpm = 2560 steps/min = 2560 pulses/min = 2560/60 = 42Hz
 * 10 rpm = 256000 steps/min = 256000 pulses/min = 25600/60 = 4266Hz = 4,267kHz 
 * 
 * 
 * Written by Johannes Holm 19/7/2016 for the Flappity Borderlands project
 */


//Set up some handy variables
int pulsePin = 3; //Sets which pin we use for sending out pulses to the driver.
int sensorPin= A0; //Sets which pin we use for reading the pot. Has to be an analog pin, ie A0 - A5
int directionPin = 4; //Sets which pin we use for direction
int enablePin = 5; //Sets which pin we use for enable ie on/off
int sensorValue = 0; //Sets up the sensor variable, will get a new value every loop.

//The tone() functions wants an unsigned int
unsigned int freq = 0; //Sets up the fequenzy variable we will later use.
unsigned int  lastFreq = 0; //So we can compare with last freq

//Code inside setup runs once when the arduino is started
void setup() {
  //Initialize serial communication 
  Serial.begin(9600);
  
  //Set pulsePin, directionPin and enablePin as OUTPUT-pins
  pinMode(pulsePin, OUTPUT);
  pinMode(directionPin, OUTPUT);
  pinMode(enablePin, OUTPUT);

  //Set direction and enable to HIGH = 5volt
  digitalWrite(directionPin, HIGH);
  digitalWrite(enablePin, HIGH);

  
  //Print the current setup over serial
  Serial.println("Current settings are:");
  Serial.println("Pulse output is on pin #" + pulsePin);
  if (enablePin == HIGH) {
    Serial.println("Direction is on pin #" + directionPin);
    Serial.print(" and is set to HIGH");
    }
  else{
  Serial.println("Direction is on pin #" + directionPin);
  Serial.print(" and is set to LOW");  
    }
  if (enablePin == HIGH) {
    Serial.println("Enable is on pin #" + enablePin);
    Serial.print(" and is set to HIGH");
    }
  else{
    Serial.println("Enable is on pin #" + enablePin);
    Serial.print(" and is set to LOW");    
    }


}


void loop() {
  sensorValue = analogRead(sensorPin); //Read the pot value
  
  freq = map(sensorValue, 0, 1023, 42, 4267); //Map the pot value (0-1023) to the desired freq. range (42-4267)
  
  delay(2); //Wait for a tiny bit so we get a proper read and map

  //Output the freq
  tone(pulsePin, freq);

  //Check if the freq has changed, if it has, write it out over serial
  if ( freq != lastFreq ) {
    Serial.println(freq + "Hz");
  }

  //Save current freq in another variable so we can compare them next loop
  lastFreq = freq;
}








