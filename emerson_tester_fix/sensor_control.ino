float ReadSensor1() {
  return float(analogRead(CONTROLLINO_A0));
}

float ReadSensor2() {
  return float(analogRead(CONTROLLINO_A1));
}

float ReadSensor3() {
  return float(analogRead(CONTROLLINO_A2));
}

float ReadRefSensor() {
  return float(analogRead(CONTROLLINO_A3));
}

void updateCountTime() {
  if (count_seconds <= 59) {
    count_seconds += 1;
  } else {
    count_seconds = 0;
    count_minutes += 1;
  }

  if (count_minutes >= 60) {
    count_minutes = 0;
    count_hours += 1;
  }
}

String updateRunTime() {
  String time_out;
  time_out = zeroPad(count_hours);
  time_out += ":";
  time_out += zeroPad(count_minutes);
  time_out += ":";
  time_out += zeroPad(count_seconds);
  return time_out;
}

String zeroPad(int _count) {
  if (_count >= 10) {
    return String(_count);
  } else {
    return "0" + String(_count);
  }
}

void printCSVHeader() {
  Serial.print("runTime");
  Serial.print(",");
  Serial.print("cycleCount");
  Serial.print(",");
  Serial.print("sensor1(RawADC)");
  Serial.print(",");
  Serial.print("sensor2(RawADC)");
  Serial.print(",");
  Serial.print("sensor3(RawADC)");
  Serial.print(",");
  Serial.print("refSensor(RawADC)");
  Serial.print("\n");
}

void printCSVLine() {
  Serial.print(updateRunTime());
  Serial.print(",");
  Serial.print(cycleCount);
  Serial.print(",");
  Serial.print(ReadSensor1());
  Serial.print(",");
  Serial.print(ReadSensor2());
  Serial.print(",");
  Serial.print(ReadSensor3());
  Serial.print(",");
  Serial.print(ReadRefSensor());
  Serial.print("\n");
}
