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
  
  
  /*
  startMotor();
  
  delay(200); // Wait until the sensor isn't triggering anymore
  //interrupts();
  
  while (!motorIsInParkingPosition()) {
    updateAllPushers();
    delay(10);
  }
  
  stopMotor();*/
  //motorEncoderHasTriggered = false;
 
}

void startMotor() {
  digitalWrite(MOTOR_PIN, HIGH);
  //motorPin.expander->digitalWrite(motorPin.pin, HIGH);
}

void stopMotor() {
  digitalWrite(MOTOR_PIN, LOW);
  motorEncoderHasTriggered = false;
  //motorPin.expander->digitalWrite(motorPin.pin, LOW);
}

bool motorIsInParkingPosition() {
  return digitalRead(MOTOR_ENCODER_PIN) == LOW;
  //return motorEncoderHasTriggered || digitalRead(MOTOR_ENCODER_PIN) == LOW;
  //motorSensorPin.expander->digitalRead(motorSensorPin.pin) == LOW;
}

void assureMotorIsInParkingPosition() {
  if (motorIsInParkingPosition()) {
    return;
  }

  runMotorOneRevolution();
}

void runMotor200() {
  Serial.println("runMotor200()");
  digitalWrite(MOTOR_PIN, HIGH);
  delay(200);
  digitalWrite(MOTOR_PIN, LOW);
  }
