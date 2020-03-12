void testAllSolenoids(MenuItem* item) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Testing solenoids...");

  Serial.println("testAllSolenoids()");
  unsigned long currentMillis = millis();

  Serial.println(previousMillis);
  Serial.println(currentMillis);
  if(currentMillis - previousMillis > 5000) {
    for (int i = 0; i < NUMBER_OF_PUSHERS; i++) {
      lcd.setCursor(0, 1);
      lcd.print(i + 1);
      setPusher(i, HIGH);
      delay(300);
      setPusher(i, LOW);
      delay(300);
    }
  }
  previousMillis = currentMillis;

  setAllPushersToLow();
}

void testMotorCallback(MenuItem* item) {
    Serial.println("testMotorCallback");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Testing motor...");
    runMotorOneRevolution();

}

void testButtonsCallback(MenuItem* item) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Push button..."); 

  int buttonNumber = 0;

  while (buttonNumber != 0) {
    buttonNumber = readButtons();
  }

  lockButtons();
  
  lcd.setCursor(0, 1);
  lcd.print(buttonNumber);
  delay(1000);
  
  unlockButtons();
}

void openHatchCallback(MenuItem* item) {
  openReturnHatch();
}


void closeHatchCallback(MenuItem* item) {
  closeReturnHatch();
}

