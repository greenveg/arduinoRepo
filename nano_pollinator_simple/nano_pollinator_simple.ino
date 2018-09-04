#include <EEPROM.h>         //eeprom lib

#define PUMP_1_PIN 13
#define PUMP_2_PIN 13

unsigned long minute = 60000;
uint8_t count = 1;
uint8_t maxCount = 200;
int cycleNumber = 0;



void setup() {
  count = EEPROM.read(4);
  pinMode(PUMP_1_PIN, OUTPUT);
  pinMode(PUMP_2_PIN, OUTPUT);
  //pinMode(PUMP_3_PIN, OUTPUT);

  Serial.begin(115200);

  Serial.println("booting");
  Serial.print("EEPROM count = ");
  Serial.println(count);
  
  
  delay(500);
  Serial.write("SHOWER ON\n");
  delay(500);
  Serial.write("SHOWER ON\n");
  delay(500);
  Serial.write("SHOWER ON\n");
  delay(500);
  
}
 
void loop() {
  if (count <= maxCount) {

    Serial.write("SHOWER ON\n");

    //Serial.println("3 minutes til schampoo");
    delay(minute);
    //Serial.println("2 minutes til schampoo");
    delay(minute);
    //Serial.println("1 minute til schampoo");
    delay(minute);
  
    digitalWrite(PUMP_1_PIN, 1);
    //Serial.println("Pump 1 (schampoo) was started (3800ms run)");
    delay(2000);
    digitalWrite(PUMP_1_PIN, 0);
    //Serial.println("Pump 1 was stopped");
    
     
    //Serial.println("2 minutes til conditioner");
    delay(minute);  
    //Serial.println("1 minute til conditioner");
    delay(minute);
    
    digitalWrite(PUMP_2_PIN, 1);
    //Serial.println("Pump 2 (condiotioner) was started (3800ms run)");
    delay(2000);
    digitalWrite(PUMP_2_PIN, 0);
    //Serial.println("Pump 2 was stopped");
    
  
    //Serial.println("5 minutes til shower off");
    delay(minute);
    //Serial.println("4 minute til shower off"); 
    delay(minute);
    //Serial.println("3 minutes til shower off");
    delay(minute);
    //Serial.println("2 minutes til shower off");
    delay(minute);  
    //Serial.println("1 minute til shower off");
  
    Serial.write("SHOWER OFF\n");
   
  
    delay(minute*11);
  /*
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
    */
    count++;
    EEPROM.write(4, count);
  }
}


