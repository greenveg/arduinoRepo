  //////////////// FUNCTIONS ////////////////
  
  double getTemperature(DeviceAddress deviceAddress){
    double tempC = sensors.getTempC(deviceAddress);
    if (tempC == -127.00) {
      Serial.println("Error getting temperature");
    }
    else {
      return tempC;
    }
  
  }
  
  void updateEncoders() {
    //Encoder 1
    newPosition1 = encoder1.read();
    oldActPosition1 = actPosition1;
    if (newPosition1 >= oldPosition1 + 4) {
      actPosition1++;
      oldPosition1 = newPosition1;
    }
    else if (newPosition1 <= oldPosition1 - 4) {
      actPosition1--;
      oldPosition1 = newPosition1;
    }
  
    //Encoder 2
    newPosition2 = encoder2.read();
    oldActPosition2 = actPosition2;
    if (newPosition2 >= oldPosition2 + 4) {
      actPosition2++;
      oldPosition2 = newPosition2;
    }
    else if (newPosition2 <= oldPosition2 - 4) {
      oldActPosition2 = actPosition2;
      actPosition2--;
      oldPosition2 = newPosition2;
    }
  
    //Button 1
    if (debounced1.rose()) {
      button1RoseMillis = currentMillis;
    }
    if (debounced1.fell()) {
      button1FellMillis = currentMillis;
      button1Diff = button1FellMillis - button1RoseMillis;
    }
  
    if ( button1Diff != 0 && button1Diff < 500) {
      //Serial.println("Button 1 short press");
      button1ShortPressDetected = true;
      button1Diff = 0;
      button1RoseMillis = 0;
    }
    else if (button1RoseMillis + 1000 >= currentMillis-10 && button1RoseMillis + 1000 <= currentMillis+10 && digitalRead(ENCODER_1_BUTTON_PIN)) {
      //Serial.println("Button 1 long press");
      button1LongPressDetected = true;
      button1RoseMillis = 0;
    }
    //Button 2
    if (debounced2.rose()) {
      button2RoseMillis = currentMillis;
    }
    if (debounced2.fell()) {
      button2FellMillis = currentMillis;
      button2Diff = button2FellMillis - button2RoseMillis;
    }
  
    if ( button2Diff != 0 && button2Diff < 500) {
      //Serial.println("Button 2 short press");
      button2ShortPressDetected = true;
      button2Diff = 0;
      button2RoseMillis = 0;
    }
    else if (button2RoseMillis + 1000 >= currentMillis-10 && button2RoseMillis + 1000 <= currentMillis+10 && digitalRead(ENCODER_2_BUTTON_PIN)) {
      //Serial.println("Button 2 long press");
      button2LongPressDetected = true;
      button2RoseMillis = 0;
    }
  
  }
  
  void LcdPrint(int row, int col, String text) {
    lcd.setCursor(col, row);
    lcd.print(text);
  }
  
  void ClearLcdForNewState() {
    lcd.setCursor(6, 0);
    //Write 14 empty spaces as chars
    for (int i=0 ; i<14 ; i++) {
      lcd.print(' ');
    }
  }
  
  void ClearLcdRow(int row) {
    lcd.setCursor(0, row);
    //Write 20 empty spaces as chars
    for (int i=0 ; i<20 ; i++) {
      lcd.print(' ');
    }
  }
  
  void ProgressStateChecker() {
    if (button1ShortPressDetected) {
      button1ShortPressDetected = false;
      state++;
      power = 0;
      SetAllDutCycles(0);
      if (state > BOIL_STATE) {state=0;}
      lcdWasFlashed = false;
    }
  }
  
  void SetAllDutCycles(double duty){
    //Serial.println(duty);
    L1.setDutyCycle(duty);
    L2.setDutyCycle(duty);
    L3.setDutyCycle(duty);  
  }
  
  void updateAllSSR() {
    L1.updateSSR();
    L2.updateSSR();
    L3.updateSSR();
  }
  
  void runTempReg() {
    if (currentMillis - regLastMillis >= regTimer){
      //Sanity check for temp values
      if (!probe01Temp > 101 || !probe01Temp < 1 || !probe02Temp > 101 || !probe02Temp < 1) {
        y = probe01Temp;
        e = settemp - y;
    
        if (e>2) {u=1;}
        else if(e>0) {u=0.5;}
        else {u=0;}
  
        //MAGIC PID FUCKERY LIES HERE
        /*
        //TODO: INSERT ANTI-WINDUP
        P = KP*e;
        I = I + KI*regTimer*e;
    
        // Create unbounded control signal
        v = P + I;
    
        // Create truncated control signal
        if (v > 1.0) {
          u = 1.0;
        } else if (v < 0.0) {
          u = 0.0;
        } else {
          u = v;
        }
        */
        
        
        //Write to SSRs
        SetAllDutCycles(u);
        //Blink a 1 on screen to tell user reg is active
        a = !a;
        lcd.setCursor(19, 2);
        lcd.print(a); 
        regLastMillis = currentMillis;   
      }
      else {
        Serial.println("Error in temp probe values");
        Serial.println(probe01Temp);
        Serial.println(probe02Temp);
      }
  
    }
    else {
      lcd.setCursor(19, 2);
      lcd.print(' ');
    }
  }
  
  void enableTempUI() {
    if (actPosition1 != oldActPosition1) {
      if(actPosition1 - oldActPosition1 < 0 && settemp > 0) settemp-=1;
      else if(actPosition1 - oldActPosition1 > 0 && settemp < 100) settemp+=1;
      lcd.setCursor(9, 2);
      lcd.print(settemp, 1);
      lcd.print("C   ");
    }
  }
  
  void enablePumpUI() {
      //Update pumpPercentage to encoder2 position. Constained to between 0 & 100 and encoder fix.
      if (actPosition2 != oldActPosition2) {
        if(actPosition2 - oldActPosition2 < 0 && pumpPercentage > 0) pumpPercentage-=10;
        else if(actPosition2 - oldActPosition2 > 0 && pumpPercentage < 100) pumpPercentage+=10;
        lcd.setCursor(6, 3);
        lcd.print(pumpPercentage);
        lcd.print("%     ");
      }
      if (button2ShortPressDetected) {
        button2ShortPressDetected = false;
        pumpIsOn = !pumpIsOn;
      }
  
    if (pumpIsOn) {
      analogWrite(PUMP_PIN, map(pumpPercentage, 0, 100, 0, 255));
      LcdPrint(3, 17, " ON");
    }
      
    '*
    +9OL§ 
      A<
      analogWrite(PUMP_PIN, 0);
      LcdPrint(3, 17, "OFF");
    }
  }

  void SerialWriteCsvHeader() {
    Serial.print("state");   
    Serial.print(",\t");
    Serial.print("settemp");
    Serial.print(",\t");
    Serial.print("probe01Temp");
    Serial.print(",\t");
    Serial.print("probe02Temp");
    Serial.print(",\t");
    Serial.print("enclosureTemp");
    Serial.print(",\t");
    Serial.print("e");
    Serial.print(",\t");
    Serial.print("u");
    Serial.print(",\t");
    Serial.println("pumpPWM");
  }
  
  
