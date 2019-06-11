void setValves(bool coldIn, bool hotIn, bool coldOut, bool hotOut) {
  digitalWrite(COLD_IN_PIN, coldIn);
  digitalWrite(COLD_OUT_PIN, coldOut);
  digitalWrite(HOT_IN_PIN, hotIn);
  digitalWrite(HOT_OUT_PIN, hotOut);
}

void startPump() {
  analogWrite(PUMP_PWM_PIN, 100);
}

void stopPump() {
  analogWrite(PUMP_PWM_PIN, 0);
}
