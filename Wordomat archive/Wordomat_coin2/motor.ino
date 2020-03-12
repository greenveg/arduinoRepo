int runMotorOneRevolution() {
  //noInterrupts();
  unsigned long motorStartMillis = millis();
  Serial.println("runMotorOneRevolution()");
  startMotor();
  delay(200);

  int runMotor = true;
  while(runMotor){
    while ( digitalRead(MOTOR_ENCODER_PIN) == HIGH) {
     if((millis()-motorStartMillis) > 5000){
      stopMotor();
      Serial.println("ERROR: Motor interupt never came");
      return -1;
     }
    }
    stopMotor();
    Serial.println("Stopped");
    delay(100);
    if(digitalRead(MOTOR_ENCODER_PIN) == HIGH){
      startMotor();
      Serial.println("To early, starting");
      runMotor=true;
    }else{
      runMotor=false;
      Serial.println("Stopped correctly");
    }
    
  }
  encoderPushed=false;
  motorEncoderHasTriggered = false;
  return 1;
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
