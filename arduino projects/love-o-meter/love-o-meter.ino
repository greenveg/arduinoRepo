const int sensorPin = A0;
const float baselineTemp = 20.0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  for (int thisPin = 8; thisPin < 11; thisPin++) {
    pinMode(thisPin, OUTPUT);
    digitalWrite(thisPin, LOW);  
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  //for (int i=1; i<100; i++){
  while (true) {
    int sensorVal = analogRead(sensorPin);
    Serial.print("Sensor Value: ");
    Serial.print(sensorVal);
  
    Serial.print(", Volts: ");
    float voltage = (sensorVal/1024.0) * 5.0;
    Serial.print(voltage);
  
    Serial.print(", degrees C: ");
    float temperature = ((voltage - 0.5) * 100);
    Serial.println(temperature);
  
    delay(100);
  }
  delay(5000);
}
