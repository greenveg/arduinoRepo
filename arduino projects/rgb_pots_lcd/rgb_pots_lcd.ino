// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//Set pins for rgb LED
const int redPin = 6;
const int bluePin = 9;
const int greenPin = 10;

long timer = 0;


void setup() {
  //Set up serial communication
  Serial.begin(9600);

  //Set up pins
  pinMode(redPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(greenPin, OUTPUT);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Red  Green  Blue");


  timer = millis();

}

void loop() {
  //Screen fix?
  //lcd.clear()

  if (millis() - timer > 100) {

    //Check pot values
    int redPotVal = analogRead(A0);
    int greenPotVal = analogRead(A1);
    int bluePotVal = analogRead(A2);


    //Print values serially to confuser
    Serial.print("Red Pot Value: ");
    Serial.print(redPotVal);

    Serial.print(" Green Pot Value: ");
    Serial.println(greenPotVal);

    Serial.print(" Blue Pot Value: ");
    Serial.print(bluePotVal);
    
    //Write to screen
    lcd.setCursor(0, 1);
    lcd.print(redPotVal); lcd.print("    ");
    lcd.setCursor(5, 1);
    lcd.print(greenPotVal); lcd.print("    ");
    lcd.setCursor(12, 1);
    lcd.print(bluePotVal); lcd.print("    ");

    
    //Write to the LED
    analogWrite(redPin, redPotVal / 4);
    analogWrite(greenPin, greenPotVal / 4);
    analogWrite(bluePin, bluePotVal / 4);


    timer = millis();
  }

}
