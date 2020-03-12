int sensorPin = A0;
int sensorValue = 0;

void setup() {                
  pinMode(8, OUTPUT);     //direction pin
  pinMode(9, OUTPUT); //step pin
  digitalWrite(8, HIGH);
  digitalWrite(9, LOW);
}

void loop() {
  sensorValue = analogRead(sensorPin);
  sensorValue = map(sensorValue,0,1023,3600,1);
  digitalWrite(9, HIGH);
  delayMicroseconds(sensorValue);          
  digitalWrite(9, LOW); 
  delayMicroseconds(sensorValue);
 }
