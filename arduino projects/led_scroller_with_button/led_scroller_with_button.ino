/* LED SCROLLER */
//timer describes the ON time of the LED.
int timer = 300;
int switchState = 0;

// the setup function runs once when you press reset or power the board
void setup() {

  
  // use a for loop to initialize each pin as an output:
  for (int thisPin = 7; thisPin < 11; thisPin++) {
    pinMode(thisPin, OUTPUT);
  }
  pinMode(2, INPUT);
}

// the loop function runs over and over again forever
void loop() {
 
   switchState = digitalRead(2);
   
   if ( switchState == LOW ) {
     for (int thisPin = 7; thisPin <10; thisPin++) {
      digitalWrite(thisPin, HIGH);
      delay(timer);
      digitalWrite(thisPin, LOW);
     }
  
    for (int thisPin = 10; thisPin > 7; thisPin--) {
      digitalWrite(thisPin, HIGH);
      delay(timer);
      digitalWrite(thisPin, LOW);
    } 
   }
  
 


}
