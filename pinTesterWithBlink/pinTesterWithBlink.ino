

const int pause = 2000;

void blinkPinWithDelay(int pin) {
  flashPin13();
  digitalWrite(pin, HIGH);
  delay(pause);             
  digitalWrite(pin, LOW);    
  delay(pause); 
}

void flashPin13() {
  digitalWrite(13, HIGH);
  delay(200);             
  digitalWrite(13, LOW);    
  delay(200); 
}

void setup() {
  // initialize digital pin 13 as an output.
  pinMode(13, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {

  flashPin13();
  flashPin13();
  flashPin13();
 
  // BOTTOM TERMINALS LEFT TO RIGHT
  blinkPinWithDelay(4);
  blinkPinWithDelay(5);
  blinkPinWithDelay(8);
  blinkPinWithDelay(9);
  blinkPinWithDelay(10);
  blinkPinWithDelay(11);

  flashPin13();
  flashPin13();
  flashPin13();

  // TOP TERMINALS LEFT TO RIGHT
  blinkPinWithDelay(A3);
  blinkPinWithDelay(A4);
  blinkPinWithDelay(A5);
  blinkPinWithDelay(3);

  
}
