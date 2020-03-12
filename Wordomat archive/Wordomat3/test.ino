void testAllSolenoids(MenuItem* item) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Testing solenoids...");

  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > 5000) {

    //52, 53, 51, 49, 47, 45, 43, 41, 39, 37, 35, 33, 31, 29, 27, 25
    int k = 47;
    digitalWrite(k, LOW);
    lcd.setCursor(0, 1);
    lcd.print(k);
    delay(1000);
    digitalWrite(k,HIGH); 
 
  }

  Serial.println("inside testAllSolenoids()");
  delay(1000);
  Serial.println("still inside 1 sec later");
  
  previousMillis = currentMillis;
    
  //setAllPushersToLow();
}

void testMotorCallback(MenuItem* item) {
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

  while (buttonNumber == 0) {
    buttonNumber = readButtons();
    Serial.println("buttonNumber: ");
    Serial.println(buttonNumber);
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

