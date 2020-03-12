const int redPin = 9;
const int bluePin = 10;
const int greenPin = 11;

void setup() {
    Serial.begin(9600);
    
    pinMode(redPin, OUTPUT);
    pinMode(bluePin, OUTPUT);
    pinMode(greenPin, OUTPUT);

    
    
}

void loop() {

  int redSensorVal = analogRead(A0);
  delay(5);
  int blueSensorVal = analogRead(A1);
  delay(5);
  int greenSensorVal = analogRead(A2);
  delay(5);  

  Serial.print("Red Sensor Value: ");
  Serial.print(redSensorVal);

  Serial.print(" Blue Sensor Value: ");
  Serial.print(blueSensorVal);

  Serial.print(" Green Sensor Value: ");
  Serial.println(greenSensorVal);
  

  analogWrite(redPin, redSensorVal/4);
  analogWrite(bluePin, blueSensorVal/4);
  analogWrite(greenPin, greenSensorVal/4);

  
}
