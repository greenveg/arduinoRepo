void runMotorOneRevolution() {
  //noInterrupts();
  unsigned long currentMillis = millis();

  Serial.println(previousMillis);
  Serial.println(currentMillis);
  if(currentMillis - previousMillis > 200) {
    Serial.println("runMotorOneRevolution()");
    startMotor();
    delay(200);
    int motorDrive = true;
    while(motorDrive){ 
      while (digitalRead(MOTOR_ENCODER_PIN) == HIGH) {
      }
      stopMotor();
      delay(10);
      if(digitalRead(MOTOR_ENCODER_PIN) == LOW){
        motorDrive=false;
      }else{
        startMotor();
        motorDrive=true;
      }
      //startMotor();
    }
    //Serial.println("STAHP");  
    stopMotor();
    delay(10);
    encoderPushed=false;
  }
  previousMillis = currentMillis;
  motorEncoderHasTriggered = false;
}

void startMotor() {
  digitalWrite(MOTOR_PIN, HIGH);
}

void stopMotor() {
  digitalWrite(MOTOR_PIN, LOW);
  motorEncoderHasTriggered = false;
}

bool motorIsInParkingPosition() {
  return digitalRead(MOTOR_ENCODER_PIN) == LOW;
}

void assureMotorIsInParkingPosition() {
  if (motorIsInParkingPosition()) {
    return;
  }

  runMotorOneRevolution();
}
