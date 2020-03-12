#define OUTPUT_PIN 2

unsigned int windowLength = 2000;
unsigned long windowStartTime = 0;
double dutyCycle = 0.5;
unsigned long currentMillis = 0;

void setup() {
  pinMode(OUTPUT_PIN, OUTPUT);  

}

void loop() {
  currentMillis = millis();

  //Check if it's time to turn on
  if(currentMillis - windowStartTime <= windowLength * dutyCycle) {
    digitalWrite(OUTPUT_PIN, HIGH);  
  }
  else {
    digitalWrite(OUTPUT_PIN, LOW);
  }

  //Check if windowsLength has passed, if so, set new windowStartTime
  if (currentMillis - windowStartTime > windowLength) {
    windowStartTime += windowLength;
  }
   

  
}

