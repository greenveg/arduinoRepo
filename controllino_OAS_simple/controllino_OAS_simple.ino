#include <Controllino.h>
           //eeprom lib

#define PUMP_1_PIN CONTROLLINO_D1
#define PUMP_2_PIN CONTROLLINO_D3
#define VALVE_1_PIN CONTROLLINO_D5
#define VALVE_2_PIN CONTROLLINO_D7

unsigned long minute = 60000;
uint8_t count = 1;
uint8_t maxCount = 200;
int cycleNumber = 0;



void setup() {
  //count = EEPROM.read(4);
  pinMode(PUMP_1_PIN, OUTPUT);
  pinMode(PUMP_2_PIN, OUTPUT);
  pinMode(VALVE_1_PIN, OUTPUT);
  pinMode(VALVE_2_PIN, OUTPUT);

  Serial.begin(9600);
  Serial2.begin(115200);

  Serial.println("booting");
  //Serial.print("EEPROM count = ");
  //Serial.println(count);
 
} 

void loop() {
  /*
  if (count <= maxCount) {
   
    Serial.print("Session count: ");
    Serial.print(count);
    Serial.print("/");
    Serial.println(maxCount); 
    
    Serial2.write("SHOWER ON\n");
    Serial.println("SHOWER ON command was sent");
    //delay(2000);
    //Serial2.write("RECYCLING_TOP-UP OK\n");
    //Serial.println("RECYCLING command was sent");
    
    Serial.println("3 minutes til schampoo");
    delay(minute);
    Serial.println("2 minutes til schampoo");
    delay(minute);
    Serial.println("1 minute til schampoo");
    delay(minute);
   */
    analogWrite(PUMP_1_PIN, 50);
    digitalWrite(VALVE_1_PIN, HIGH);
    Serial.println("Pump 1 (schampoo) was started (3800ms run)");
    delay(2000);
    digitalWrite(VALVE_1_PIN, LOW);
    analogWrite(PUMP_1_PIN, 0);
    Serial.println("Pump 1 was stopped");

    
    Serial.println("2 minutes til conditioner");
    delay(minute);  
    Serial.println("1 minute til conditioner");
    delay(minute);
    
    
    analogWrite(PUMP_2_PIN, 50);
    digitalWrite(VALVE_2_PIN, HIGH);
    Serial.println("Pump 2 (condiotioner) was started (3800ms run)");
    delay(2000);
    digitalWrite(VALVE_2_PIN, LOW);
    analogWrite(PUMP_2_PIN, 0);
    Serial.println("Pump 2 was stopped");

    delay(minute*18);

    /*
    Serial.println("5 minutes til shower off");
    delay(minute);
    Serial.println("4 minute til shower off"); 
    delay(minute);
    Serial.println("3 minutes til shower off");
    delay(minute);
    Serial.println("2 minutes til shower off");
    delay(minute);  
    Serial.println("1 minute til shower off");
  
    Serial2.write("SHOWER OFF\n");
    Serial.println("SHOWER OFF command was sent");
  
  
  
    Serial.println("11 minute til restart");
    delay(minute);
    Serial.println("10 minutes til restart");
    delay(minute);
    Serial.println("9 minutes til restart"); 
    delay(minute);
    Serial.println("8 minutes til restart");
    delay(minute);
    Serial.println("7 minutes til restart");
    delay(minute);
    Serial.println("6 minute til restart");
    delay(minute);  
    Serial.println("5 minutes til restart");
    delay(minute);
    Serial.println("4 minute til restart"); 
    delay(minute);
    Serial.println("3 minutes til restart");
    delay(minute);
    Serial.println("2 minutes til restart");
    delay(minute);
    Serial.println("1 minute til restart");
    delay(minute);
    count++;
    EEPROM.write(4, count);
    
  }
  */
}


