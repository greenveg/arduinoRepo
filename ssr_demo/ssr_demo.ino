
const int ledPins[3] = {9, 10, 11};
const int ssrPwmPeriod = 2000;

unsigned long currentMillis = 0;


class SSR_Control {
  private:  
  unsigned int _pin;
  unsigned int _windowLength;
  unsigned long _windowStartTime;
  double _dutyCycle;

  public:
  SSR_Control(int pin, int windowLength) {
    _pin = pin;
    _windowLength = windowLength;
    
    pinMode(_pin, OUTPUT);
  }    
  void updateSSR(){
    //Check if it's time to turn on
    if(currentMillis - _windowStartTime <= _windowLength * _dutyCycle) {
      digitalWrite(_pin, HIGH);  
    }
    else {
      digitalWrite(_pin, LOW);
    }
  
    //Check if windowsLength has passed, if so, set new windowStartTime
    if (currentMillis - _windowStartTime > _windowLength) {
      _windowStartTime += _windowLength;
    }
  } 
  void setDutyCycle(double newDutyCycle) {
    _dutyCycle = newDutyCycle;
  }
  double getDutyCycle(){
    return _dutyCycle;
  }
 
};//end SSR class

SSR_Control L1(ledPins[0], ssrPwmPeriod);
SSR_Control L2(ledPins[1], ssrPwmPeriod);
SSR_Control L3(ledPins[2], ssrPwmPeriod);



void setup() {
  Serial.begin(9600);

  for (int i=0 ; i<3 ; i++) {
    pinMode(ledPins[i], OUTPUT);
  }

  L1.setDutyCycle(0.50);
  L2.setDutyCycle(0.75);
  L3.setDutyCycle(1);
  
  Serial.println("booting");
  delay(1000);
}

void loop() {
  currentMillis = millis();
 
  L1.updateSSR();
  L2.updateSSR();
  L3.updateSSR();

}
