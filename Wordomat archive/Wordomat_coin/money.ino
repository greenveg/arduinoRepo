const int HATCH_OPEN_SERVO_ANGLE = 45;
const int HATCH_CLOSED_SERVO_ANGLE = 135;

void acceptCoins() {
  //latestReceivedCoin = 0;
}

void rejectAllCoins() {
  //latestReceivedCoin = 0;
}

void openReturnHatch() {
  paidAmount = 0;
  latestReceivedCoin = 0;
  // Öppna retur-luckan mha servo
  coinHatchServo.write(HATCH_OPEN_SERVO_ANGLE);
  //i2cServo(COIN_HATCH_I2C_ADDRESS, HATCH_OPEN_SERVO_ANGLE);

}

void closeReturnHatch() {
  // Stäng retur-luckan mha servo
  coinHatchServo.write(HATCH_CLOSED_SERVO_ANGLE);
  //i2cServo(COIN_HATCH_I2C_ADDRESS, HATCH_CLOSED_SERVO_ANGLE);
}

void collectPayment() {
  paidAmount = 0;
  latestReceivedCoin = 0;
  // Här behöver vi inte göra mer, motorn som matar ut varorna har redan öppnat myntluckan
}

bool coinReceived() {
  if (Serial.available() > 0) {
    latestReceivedCoin = Serial.parseInt();
    clearSerialBuffer();
  }

  if (Serial3.available() > 0) {
    latestReceivedCoin = Serial3.parseInt();
    clearSerial3Buffer();
  }

  /*
    Wire.requestFrom(COIN_SLOT_I2C_ADDRESS, 1);

    while (Wire.available()) {
    latestReceivedCoin = Wire.read();
    }
  */

  if (latestReceivedCoin = 255) {
    latestReceivedCoin = 0;
  }


  if (latestReceivedCoin > 0) {
    latestReceivedCoin -= 1;
    Serial.print("Received coin: ");
    Serial.println(latestReceivedCoin);
    return true;
  } else {
    return false;
  }
}

