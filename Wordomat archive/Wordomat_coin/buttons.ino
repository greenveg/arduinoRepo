const int BUTTONS_LOCKED_SERVO_ANGLE = 60;
const int BUTTONS_UNLOCKED_SERVO_ANGLE = 90;

bool buttonsAreLocked = false;

void lockButtons() {
  if (!buttonsAreLocked) {
    buttonLockServo.write(BUTTONS_LOCKED_SERVO_ANGLE);
    //i2cServo(BUTTON_LOCK_I2C_ADDRESS, BUTTONS_LOCKED_SERVO_ANGLE);
  }
  
  buttonsAreLocked = true;
}

void unlockButtons() {
  if (!buttonsAreLocked) {
    buttonLockServo.write(BUTTONS_UNLOCKED_SERVO_ANGLE);
    //i2cServo(BUTTON_LOCK_I2C_ADDRESS, BUTTONS_UNLOCKED_SERVO_ANGLE);
  }
  
  buttonsAreLocked = true;
}

int readButtons() {
  for (int i = 0; i < 15; i++) {
    if (digitalRead(BUTTON_PINS[i]) == LOW) {
      //Serial.println("readButtons() ends and will return: ");
      //Serial.println(i+1);
      delay(2);
      return i + 1;
    }
  }
  //Serial.println("readButtons() ends and will return 0");
  return 0;
}

