void testAllSolenoids(MenuItem* item) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Testing solenoids...");
  Serial.println("testAllSolenoids()");
  unsigned long currentMillis = millis();
  //Serial.println(currentMillis);

  if ((millis() - previousMillis) > 1000) {
    lcd.setCursor(0, 1);
    //Serial.println("Timecheck OK");
    
      for (int i = 53; i >= 23; i = i - 2) {
      digitalWrite(i, LOW);
      lcd.setCursor(0, 1);
      lcd.print(i);
      Serial.println(i);
      delay(300);
      runMotorOneRevolution();
      digitalWrite(i, HIGH);
      delay(300);
      
      }
      delay(100);
    

    //53, 51, 49, 47, 45, 43, 41, 39, 37, 35, 33, 31, 29, 27, 25, 23
    //int k = 47;
    /*
    //for(int i = 0; i<10;i++){
      Serial.print("Pre run: ");
      Serial.println(i);
      digitalWrite(k, LOW);
      lcd.setCursor(0, 1);
      lcd.print(k);
      delay(300);
      //runMotorOneRevolution();
      digitalWrite(k, HIGH);
      //k = k - 2;
      delay(300);'
      }
      */
    
    
  previousMillis = millis();   
  }
  else {
    lcd.print("Timecheck not OK");
  }

//  Serial.println("Out of timechecker, inside testAllSolenoids()");
//  delay(1000);
//  Serial.println("still inside 1 sec later");

  previousMillis = currentMillis;

  //setAllPushersToLow();
}

void testMotorCallback(MenuItem* item) {
  Serial.println("testMotorCallback()");
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

