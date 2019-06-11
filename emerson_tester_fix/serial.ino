void CheckForSerialCommands() {
  if (Serial.available() > 0) {
    String recivedLine = Serial.readString();

    if (recivedLine == "start\n") {
      goToState(COLDWATER);
    } else if (recivedLine == "stop\n") {
      goToState(RESET);
    }
  }
}
