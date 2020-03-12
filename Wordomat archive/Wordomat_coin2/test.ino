void testAllSolenoids(MenuItem* item) {
  lcd.clear();
  lcd.setCursor(0, 0);
  //lcd.print("Testing solenoids...");
  Serial.println("testAllSolenoids()");
  unsigned long currentMillis = millis();
  //Serial.println(currentMillis);
    lcd.setCursor(0, 1);
    //Serial.println("Timecheck OK");
    if(soloniodTest>=NUMBER_OF_PUSHERS){
      soloniodTest=0;
    }
    digitalWrite(PUSHER_PINS[soloniodTest], LOW);
    lcd.setCursor(0, 1);
    lcd.print(soloniodTest);
    Serial.print(soloniodTest);
    Serial.print(": is pin: ");
    Serial.println(PUSHER_PINS[soloniodTest]);
    delay(300);
    if(soloniodTest>0) digitalWrite(PUSHER_PINS[soloniodTest-1], HIGH);
   delay(300);
    soloniodTest++;
    
//      for (int i = 0; i < NUMBER_OF_PUSHERS; i++) {
//      digitalWrite(PUSHER_PINS[i], LOW);
//      lcd.setCursor(0, 1);
//      lcd.print(i);
//      Serial.print(i);
//      Serial.print(": is pin: ");
//      Serial.println(PUSHER_PINS[i]);
//      delay(300);
//      digitalWrite(PUSHER_PINS[i], HIGH);
//      delay(300);
//      }
    

    //53, 51, 49, 47, 45, 43, 41, 39, 37, 35, 33, 31, 29, 27, 25, 23
    //int k = 47;
    /*
    for(int i = 0; i<10;i++){
      Serial.print("Pre run: ");
      Serial.println(i);
      digitalWrite(k, LOW);
      lcd.setCursor(0, 1);
      lcd.print(k);
      delay(300);
      runMotorOneRevolution();
      digitalWrite(k, HIGH);
      //k = k - 2;
      delay(300);
    }
    */

//  Serial.println("Out of timechecker, inside testAllSolenoids()");
//  delay(1000);
//  Serial.println("still inside 1 sec later");


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

