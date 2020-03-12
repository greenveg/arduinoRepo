int read0 = 0;
int read1 = 0;

void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600); 


}

void loop() {
  read0 = analogRead(A0);
  //read1 = analogRead(A3);

  Serial.print("Pot 1 on A0: ");
  Serial.println(read0);
  //Serial.print("Pot 2 on A1: ");
  //Serial.println(read1);
  Serial.println("--------------");
  
  delay(1000);
}
