#include <Controllino.h>  /* Usage of CONTROLLINO library allows you to use CONTROLLINO_xx aliases in your sketch. */


// the setup function runs once when you press reset (CONTROLLINO RST button) or connect the CONTROLLINO to the PC
void setup() {
  Serial.begin(9600);
  Serial2.begin(115200);

  Serial.println("Hello world");
  Serial2.write("SHOWER ON\n");
  
  // initialize all used digital output pins as outputs
  pinMode(CONTROLLINO_D0, OUTPUT);
  pinMode(CONTROLLINO_D1, OUTPUT);  // note that we are using CONTROLLINO aliases for the digital outputs
  pinMode(CONTROLLINO_D2, OUTPUT);
  pinMode(CONTROLLINO_D3, OUTPUT);  // the alias is always like CONTROLLINO_
  pinMode(CONTROLLINO_D4, OUTPUT);  // and the digital output label as you can see at the CONTROLLINO device
  pinMode(CONTROLLINO_D5, OUTPUT);  // next to the digital output screw terminal
  pinMode(CONTROLLINO_D6, OUTPUT);
  pinMode(CONTROLLINO_D7, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(CONTROLLINO_D0, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);                          // wait for 100 milliseconds which is 1/10 of a second 
  digitalWrite(CONTROLLINO_D0, LOW);    // turn the LED off by making the voltage LOW
  delay(100);                          // wait for 100 milliseconds which is 1/10 of a second 
  digitalWrite(CONTROLLINO_D1, HIGH);  
  delay(100);                       
  digitalWrite(CONTROLLINO_D1, LOW);    
  delay(100);                       
  digitalWrite(CONTROLLINO_D2, HIGH);   // please, visit https://controllino.biz/downloads/   
  delay(100);                           // if you want to know more about the mapping of the CONTROLLINO
  digitalWrite(CONTROLLINO_D2, LOW);    // digital outputs to the Arduino pins
  delay(100);                      
  digitalWrite(CONTROLLINO_D3, HIGH);   
  delay(100);                      
  digitalWrite(CONTROLLINO_D3, LOW);    // by using CONTROLLINO aliases instead of Arduino pin numbers    
  delay(100);                           // you ensure sketch portability between all CONTROLLINO variants
  digitalWrite(CONTROLLINO_D4, HIGH);  
  delay(100);                       
  digitalWrite(CONTROLLINO_D4, LOW);    
  delay(100);                     
  digitalWrite(CONTROLLINO_D5, HIGH);  
  delay(100);                      
  digitalWrite(CONTROLLINO_D5, LOW);    
  delay(100);                      
  digitalWrite(CONTROLLINO_D6, HIGH);  
  delay(100);                      
  digitalWrite(CONTROLLINO_D6, LOW);   
  delay(100);                       
  digitalWrite(CONTROLLINO_D7, HIGH);  
  delay(100);                       
  digitalWrite(CONTROLLINO_D7, LOW);   
  delay(100);                      
}

