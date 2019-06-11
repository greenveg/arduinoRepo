void goToState(STATE new_state) {
  oldState = currentState;
  currentState = new_state;
}

void setWaitMS(int wait_ms) {
  waitMS = wait_ms;
}

void StateUpdate() {
  if (currentState == IDLE) {
    return;
  } else if (currentState == RESET) {
    ResetAndGoToIdle();
  } else if (currentState == WAIT) {
    WaitLoop();
  } else if (currentState == COLDWATER) {
    ColdWaterSequence();
  } else if (currentState == BLEEDHOTWATER) {
    BleedHotWaterSequence();
  } else if (currentState == HOTWATER) {
    HotWaterSequence();
  } else if (currentState == BLEEDCOLDWATER) {
    BleedColdWaterSequence();
  }
}

void ResetAndGoToIdle() {
  stopPump();
  setValves(0, 0, 0, 0);
  cycleCount = 0;
  count_seconds = 0;
  count_minutes = 0;
  count_hours = 0;
  flagPrintCSVHeader = true;
  goToState(IDLE);
}

void WaitLoop() {
  if (oldState == COLDWATER && waitMS <= 0) {
    goToState(BLEEDHOTWATER);
  } else if (oldState == BLEEDHOTWATER && waitMS <= 0) {
    goToState(HOTWATER);
  } else if (oldState == HOTWATER && waitMS <= 0) {
    goToState(BLEEDCOLDWATER);
  } else if (oldState == BLEEDCOLDWATER && waitMS <= 0) {
    goToState(COLDWATER);
  }

  if (waitMS > 0) {
    waitMS -= 1;
  }
}

void ColdWaterSequence() {
  setValves(1, 0, 1, 0);
  startPump();
  setWaitMS(5000);
  goToState(WAIT);
}

void BleedHotWaterSequence() {
  setValves(0, 1, 1, 0);
  setWaitMS(1000);
  goToState(WAIT);
}

void HotWaterSequence() {
  setValves(0, 1, 0, 1);
  setWaitMS(5000);
  goToState(WAIT);
}

void BleedColdWaterSequence() {
  setValves(1, 0, 0, 1);
  startPump();
  setWaitMS(700);
  goToState(WAIT);
  cycleCount += 1;
}
