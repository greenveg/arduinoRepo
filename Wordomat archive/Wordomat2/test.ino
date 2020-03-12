void testAllSolenoids(MenuItem* item) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Testing solenoids...");

  digitalWrite(52, HIGH);
  delay(300);
  digitalWrite(52, LOW);  

  for (int i=53; i>=25; i=i-2;) {
    lcd.setCursor(0, 1);
    lcd.print(i + 1);
    digitalWrite(i, HIGH);
    delay(300);
    digitalWrite(i, LOW);
    delay(300);
  }
}

void testMotorCallback(MenuItem* item) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Testing motor...");

  runMotorOneRevolution();
  Serial.println("Test: runMotorOneRevolution()");
}

void testButtonsCallback(MenuItem* item) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Push button...");
  while (!readButtons());
  lockButtons();
  lcd.setCursor(0, 1);
  lcd.print(readButtons());
  delay(500);
  unlockButtons();
}

void openHatchCallback(MenuItem* item) {
  openReturnHatch();
}


void closeHatchCallback(MenuItem* item) {
  closeReturnHatch();
}

