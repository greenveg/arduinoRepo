/*************************************************************
Motor Shield 1-Channel DC Motor Demo
by Randy Sarafan

For more information see:
http://www.instructables.com/id/Arduino-Motor-Shield-Tutorial/

*************************************************************/

void setup() {
  
  //Setup Channel B
  pinMode(13, OUTPUT); //Initiates Motor Channel A pin
  pinMode(8, OUTPUT); //Initiates Brake Channel A pin

  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("dc test!");
  
}

void loop(){


  Serial.println("Test starts in 5");
  delay(1000);
  Serial.println("...4");
  delay(1000);
  Serial.println("...3");
  delay(1000);
  Serial.println("...2");
  delay(1000);
  Serial.println("...1");
  delay(1000);
  Serial.println("NOW");
  
  
  digitalWrite(13, HIGH); //Establishes forward direction of Channel B
  digitalWrite(8, LOW);   //Disengage the Brake for Channel B
  analogWrite(11, 100);   //Spins the motor on Channel B

  int readCurrent = analogRead(A1);

  Serial.println(readCurrent);

  /*
  if (readCurrent > 3) {
    analogWrite(11, 0);
    Serial.println("First loopstopper");   
  }
  */
 
  delay(5000);
  
  readCurrent = analogRead(A1);
  
  Serial.println(readCurrent);

  /*
  if (readCurrent > 3.3) {
    analogWrite(11, 0);
    Serial.println("Sec loopstopper");   
  }
  */
  
  delay(100);
  analogWrite(11, 0);
  delay(5000);
}


