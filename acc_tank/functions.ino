//////////////// FUNCTIONS ////////////////

void printDateAndTime() {
  int n;
  Serial.print("Date and time: ");
  n = Controllino_GetDay();
  Serial.print(n);
  Serial.print("/");
  n = Controllino_GetMonth();
  Serial.print(n);
  Serial.print("-");
  n = Controllino_GetYear();
  Serial.print(n);
  Serial.print("   ");
  n = Controllino_GetHour();
  Serial.print(n);
  Serial.print(":");
  n = Controllino_GetMinute();
  Serial.println(n);
}

void countDownMinutes(int m, String message) {
  if (!countDownMinutesHasRun) {
    userMinute = m;
    waitStartedMillis = currentMillis;
    previousMillis = waitStartedMillis;
    //Serial.print(userMinute);
    //ssstSerial.println(message);
    userMinute--;
    countDownMinutesHasRun = true;
  }
  
  if (currentMillis >= waitStartedMillis + m*minute) {
    countDownMinutesHasRun = false;
    //Serial.println("state++");
    state++;
  }
  else if (userMinute > 0 && currentMillis >= previousMillis + minute) {
    previousMillis = currentMillis;
    //Serial.print(userMinute);
    //stSerial.println(message);
    userMinute--;
  }
}//end countDownMinutes()

void waitForMs(int m) {
  if (!waitForMsHasRun) {
    waitStartedMillis = currentMillis;
    waitForMsHasRun = true;
  }
  if (currentMillis >= waitStartedMillis + m) {
    waitForMsHasRun = false;
    state++;
  }
}//end countDownMinutes()

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

}

void LcdPrint(int row, int col, String text) {
  lcd.setCursor(col, row);
  lcd.print(text);
}

void ClearLcdRow(int row) {
  lcd.setCursor(0, row);
  //Write 20 empty spaces as chars
  for (int i=0 ; i<20 ; i++) {
    lcd.print(' ');
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
    if (!probe01Temp > 99 || !probe01Temp < 5 || !probe02Temp > 99 || !probe02Temp < 5) {
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
