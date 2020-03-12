#include <Wire.h> //include i2c library
/*Sketch to drive Brushed DC-Motor in two directions*/

#define  IS_1  0
#define  IS_2  1
#define  IN_1  3
#define  IN_2  11
#define  INH_1 12
#define  INH_2 13

#define TCONST 100      //Delay Time between Steps

int Motor_DC     = 0;    // actual DC
int Motor_DC_MAX = 60;   // 50% Dutycycle

void setup() {

  // put your setup code here, to run once:
  pinMode(IN_1,OUTPUT);
  pinMode(IN_2,OUTPUT);
  pinMode(INH_1,OUTPUT);
  pinMode(INH_2,OUTPUT);
  
  reset_ports();
  
  digitalWrite(INH_1,1);
  digitalWrite(INH_2,1);
}

void fade_Motor(int port)
{
   for(int i = 0; i< Motor_DC_MAX; i++)
  {
  Motor_DC = map( i , 0 , 100 , 0 , 255 );
  analogWrite( port , Motor_DC );

  delay(TCONST);
  }
  
  //Slow down Motor
  for(int i = Motor_DC_MAX; i>= 0; i--)
  {
  Motor_DC = map( i , 0 , 100 , 0 , 255 );
  analogWrite( port , Motor_DC );

  delay(TCONST);
  }
}

//Stop
void reset_ports()
{
  digitalWrite(IN_1,0);
  digitalWrite(IN_2,0);
}

void loop() {

 
  //Fade Motor in forward direction
  fade_Motor(IN_2);
  
  //Wait and Stop
  reset_ports();
  delay(1000);  //Wait for 1s
  
  //Fade Motor in backward direction
  fade_Motor(IN_1);
  
  //Wait and Stop
  reset_ports();
  delay(1000);  //Wait for 1s
  
}
