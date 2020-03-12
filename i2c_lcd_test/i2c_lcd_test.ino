#include <LiquidCrystal_I2C.h>


//Setup LCD screen on I2C
LiquidCrystal_I2C lcd(0x3F,20,4); // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup() {
  // put your setup code here, to run once:
  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("MODE: ");
  lcd.print("Standby");
  lcd.setCursor(0, 1);
  lcd.print("T1: ");
  lcd.print("20.5C");
  lcd.setCursor(11, 1);
  lcd.print("T2: ");
  lcd.print("21.5C");
  lcd.setCursor(0, 2);
  lcd.print("Setpoint: ");
  lcd.print("67.5C");
  lcd.setCursor(0, 3);
  lcd.print("Pump: ");
  lcd.print("50 %");  
  
}

void loop() {
  // put your main code here, to run repeatedly:

}

