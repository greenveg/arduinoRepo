volatile long latestEncoderPush = 0;

void onEncoderPush() {
  if (millis() - latestEncoderPush < 50) {
    return;
  }

  encoderPushed = true;
  latestEncoderPush = millis();
}

void setupEncoderButton(int pin) {
  pinMode(pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(pin), onEncoderPush, LOW);
}

void encoderHandler(Encoder _encoder) {
  long encoderPosition = _encoder.read() / 4;


  if (encoderPosition != 0) {
    _encoder.write(0);
  }

  if (encoderPosition > 0) {
    for (int ie = 0; ie < encoderPosition; ie++) {
      ms.next();
    }
  } else if (encoderPosition < 0) {
    for (int ie = 0; ie > encoderPosition; ie--) {
      ms.prev();
    }
  }

  if (encoderPosition != 0) {
    ms.display();
  }

  if (encoderPushed) {
    encoderPushed = false;
    ms.select();
    ms.display();
  }
}
