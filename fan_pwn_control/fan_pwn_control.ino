/*
 * Timer black magic from
 * https://forum.arduino.cc/index.php?topic=415167.0
 * 
 * Succesfully controls a noctua 12V 4-pin fan with good results.
 */

#define POT_PIN A1
#define PWM_OUT_PIN 10

uint16_t potVal;
uint32_t currentMillis;
uint32_t lastMillis;

word pwmB = 288; // 90% duty (0-320 = 0-100% duty cycle)


void setup() {
  Serial.begin(9600);
  
  
  pinMode(POT_PIN, INPUT);
  pinMode(PWM_OUT_PIN, OUTPUT);


  //Changin timer divisor = BLACK MAGIC
  
  TCCR1A = 0;            //clear timer registers
  TCCR1B = 0;
  TCNT1 = 0;

  TCCR1B |= _BV(CS10);   //no prescaler
  ICR1 = 320;            //PWM mode counts up 320 then down 320 counts (25kHz)
  
  OCR1B = pwmB;          //0-320 = 0-100% duty cycle
  TCCR1A |= _BV(COM1B1); //output B clear rising/set falling

  TCCR1B |= _BV(WGM13);  //PWM mode with ICR1 Mode 10
  TCCR1A |= _BV(WGM11);  //WGM13:WGM10 set 1010
}


void loop() {
  currentMillis = millis();

  //Simple timer to read every n ms
  if (currentMillis - lastMillis >= 200) {
    lastMillis = currentMillis;
    
    
    potVal = analogRead(POT_PIN);
    if(potVal <= 10) {potVal= 0;}
    delay(1);
    
    //Map potVal to weird 0-320 value. 
    pwmB = map(potVal, 0, 1023, 0, 320);

    /*
    Serial.print("Analog read: ");
    Serial.println(potVal);
    Serial.print("pwmB set to [%]: ");
    Serial.println(map(potVal, 0, 1023, 0, 100) );
    Serial.println(' ');
    */
  }
  
  analogWrite(PWM_OUT_PIN, pwmB );
  delay(1);//delay for stability, maybe not needed, maybe bad for time divisory BFM.
  
}
