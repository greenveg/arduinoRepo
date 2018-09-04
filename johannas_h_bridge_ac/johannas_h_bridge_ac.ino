
#define INA_PIN 7
#define INB_PIN 8
#define PWM_PIN 9

const int Hz = 100;
//const int pwm = 128;

void setup() {
  pinMode(INA_PIN, OUTPUT);
  pinMode(INB_PIN, OUTPUT);
  pinMode(PWM_PIN, OUTPUT);

  digitalWrite(PWM_PIN, HIGH);
}

void loop() {

  digitalWrite(INA_PIN, HIGH);
  digitalWrite(INB_PIN, LOW);

  delay(4);

  digitalWrite(INA_PIN, LOW);
  digitalWrite(INB_PIN, LOW);

  delay(1);

  digitalWrite(INA_PIN, LOW);
  digitalWrite(INB_PIN, HIGH);

  delay(4);

  digitalWrite(INA_PIN, LOW);
  digitalWrite(INB_PIN, LOW);

  delay(1);

  
}


