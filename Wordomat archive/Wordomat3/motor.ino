void runMotorOneRevolution() {
  //noInterrupts();
  unsigned long currentMillis = millis();

  Serial.println(previousMillis);
  Serial.println(currentMillis);
  if(currentMillis - previousMillis > 5000) {
    Serial.println("runMotorOneRevolution()");
    startMotor();
    delay(200);
      
    while ( digitalRead(MOTOR_ENCODER_PIN) == HIGH) {
      startMotor();
      Serial.println("loopsie");
      delay(2);
      }
    Serial.println("STAHP");  
    stopMotor();
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
