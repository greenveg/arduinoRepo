/*
  Code for the Uno wich is in slave mode (I2C).
  Written by Johannes Holm @ Malmö university in May 2016
 */


//include i2c library
#include <Wire.h> 

void setup() {
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output

  // Set dc-shield pins as output
  pinMode(3,OUTPUT);    //IN_1
  pinMode(11,OUTPUT);   //IN_2
  pinMode(12,OUTPUT);   //INH_1
  pinMode(13,OUTPUT);   //INH_2


  //Please dont go to sleep DC-shield
  digitalWrite(12, HIGH);
  digitalWrite(13, HIGH);

}


void loop() {
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {

  //read the message
  int x = Wire.read();      // receive byte as an integer
  Serial.println(x);        // print the integer for easy troubleshooting

  //set the top PWM that we accelarate too
  int motorPWMtop = 105;

 //Main switch, does the work 
 switch (x) {
    case 1:
      Serial.println("Now inside case 1");    //For troubleshooting
      Serial.println("Forward");

      //Accelerate the motor
      for (int i = 0 ; i <= motorPWMtop ; i++) {
        analogWrite(3, i);
        delay(500);
      }
      for (int i = motorPWMtop ; i >=0 ; i--) {
        analogWrite(3, i);
        delay(500);
      }
    break;
 
    case 2:
      Serial.println("Now inside case 2");    //For troubleshooting
      Serial.println("Backward");

      //Accelerate the motor backwards and compensate for "help" we get from the spring in the grip
      for (int i = 0 ; i <= (motorPWMtop-17) ; i++) {
        analogWrite(11, i);
        delay(500);
      }
      for (int i = (motorPWMtop-17) ; i >=0 ; i--) {
        analogWrite(11, i);
        delay(500);
      }
     
    break;
    
    }
}
