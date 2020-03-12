// Arduino standard libraries

// Pin assignments
const int ENCODER_BUTTON_PIN = 18;
const int MOTOR_SENSOR_PIN = 19;
const int ENCODER_PIN_1 = 3;
const int ENCODER_PIN_2 = 2;

const int PUSHER_1 = 52;
const int PUSHER_2 = 53;
const int PUSHER_3 = 51;
const int PUSHER_4 = 49;
const int PUSHER_5 = 47;
const int PUSHER_6 = 45;
const int PUSHER_7 = 43;
const int PUSHER_8 = 41;
const int PUSHER_9 = 39;
const int PUSHER_10 = 37;
const int PUSHER_11 = 35;
const int PUSHER_12 = 33;
const int PUSHER_13 = 31;
const int PUSHER_14 = 29;
const int PUSHER_15 = 27;
const int PUSHER_16 = 25;

const int MOTOR_PIN = 23;


//Motor
int motorRunTimes = 0;


void setup() {
  pinMode(PUSHER_1, OUTPUT);
  digitalWrite(52, LOW);
  for (int i=53; i>=25; i=i-2) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }

  Serial.begin(9600);
}

void loop() {
  /*
  if (!motorIsInParkingPosition()) {
    runMotorOneRevolution();
  }
  */

/*  digitalWrite(52, HIGH);
  delay(300);
  digitalWrite(52, LOW);  
*/
  for (int i=53; i>=25; i=i-2) {
    digitalWrite(i, HIGH);
    delay(300);
    digitalWrite(i, LOW);
    delay(300);
  }

  delay(3000);
}


//MOTOR
/*
void runMotorOneRevolution() {
  Serial.println("runMotorOneRevolution()");
  startMotor();
  delay(200); // Wait until the sensor isn't triggering anymore
  while (!motorIsInParkingPosition());
  stopMotor();

}

void startMotor() {
  digitalWrite(MOTOR_PIN, HIGH);
}

void stopMotor() {
  digitalWrite(MOTOR_PIN, LOW);
}

bool motorIsInParkingPosition() {
  return digitalRead(MOTOR_SENSOR_PIN) == LOW;
}
*/
