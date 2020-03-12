//#include <LiquidCrystal_I2C.h>
#include <Bounce2.h>

const int POT1_PIN = A0;
const int BUTTON_PIN = 12;
const int LED_PINS[] = {
  4, 5, 6, 7
};

unsigned int state = 0;
double setpoint = 0;
int power = 0;

//Setup LCD screen on I2C
//LiquidCrystal_I2C lcd(0x27,20,4); // set the LCD address to 0x27 for a 16 chars and 2 line display

// Instantiate a Bounce object
Bounce debouncer = Bounce(); 


void setup() {
  /*
  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(8, 1);
  lcd.print("state:");
  */
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(POT1_PIN, INPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  
  Serial.begin(9600);

  debouncer.attach(BUTTON_PIN);
  debouncer.interval(5);
  
  delay(1000);

}//end setup

void loop() {

  switch (state) {

    case 0: //start state
      Serial.println("state 0");
      setStateLeds(state);
      //lcd.setCursor(0, 1);
      //lcd.print("        ");
      
      if( debouncer.fell() ) {state++; }
      break;  
      
    case 1: //reg state
      Serial.println("state 1");
      setStateLeds(state);
      setpoint = map(analogRead(POT1_PIN), 0, 1023, 95, 0);
      /*
      lcd.setCursor(0, 1);
      lcd.print("SP:");
      lcd.setCursor(4, 1);
      lcd.print(setpoint, 1);
      */     
  
      //Button press = next state
       if( debouncer.fell() ) {state++; }
      break;
      
    case 2: //power state
      Serial.println("state 2");
      setStateLeds(state);
      power = map(analogRead(POT1_PIN), 0, 1023, 100, 0);
      /*
      lcd.setCursor(0, 1);
      lcd.print("PW:");
      lcd.setCursor(4, 1);
      lcd.print(power);
      */
      //Button press = next state
       if( debouncer.fell() ) {state = 0; }
      break;
  }
  //updateLCD();
  debouncer.update();
}//end main loop


void updateLCD() {
  //lcd.setCursor(15, 1);
  //lcd.print(state); 
}

void setStateLeds(int state) {
  for(int i = 0 ; i < (sizeof(LED_PINS)-1) ; i++) {
    digitalWrite(LED_PINS[i], LOW);
  }
  digitalWrite(LED_PINS[state], HIGH);
  
  
}


