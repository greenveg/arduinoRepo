const int BUTTONS_LOCKED_SERVO_ANGLE = 60;
const int BUTTONS_UNLOCKED_SERVO_ANGLE = 90;

void lockButtons() {
  i2cServo(BUTTON_LOCK_I2C_ADDRESS, BUTTONS_LOCKED_SERVO_ANGLE);
  //lockButtonsPin.expander->digitalWrite(lockButtonsPin.pin, HIGH);
  buttonsAreLocked = true;
}

void unlockButtons() {
  i2cServo(BUTTON_LOCK_I2C_ADDRESS, BUTTONS_UNLOCKED_SERVO_ANGLE);
  //lockButtonsPin.expander->digitalWrite(lockButtonsPin.pin, LOW);
  buttonsArelocked = false;
}

int readButtons() {
  for (int i = 0; i < NUMBER_OF_BUTTONS; i++) {
    if (buttons[i].expander->digitalRead(buttons[i].pin) == LOW) {
      return i + 1;
    }
  }

  return 0;
}

