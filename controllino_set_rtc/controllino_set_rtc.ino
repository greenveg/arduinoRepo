#include "Arduino.h"
#include <SPI.h>
#include <Controllino.h>    //Controllino lib

int day;
int weekDay = 2; //MUST BE MANUALLY SET
int month;
int year;
int hour;
int minute;
int second;

String timeStr = __TIME__ ;      //Format: hh:mm:ss
String dateStr = __DATE__ ;       //Format: Sep 18 2018   

int getYear() {
  return dateStr.substring(9, 11).toInt(); 
}

int getMonth() {

  if (dateStr.substring(0, 3) == "Jan") {
    return 1;
  }
  else if (dateStr.substring(0, 3) == "Feb") {
    return 2;
  }
  else if (dateStr.substring(0, 3) == "Mar") {
    return 3;
  }
  else if (dateStr.substring(0, 3) == "Apr") {
    return 4;
  }
  else if (dateStr.substring(0, 3) == "May") {
    return 5;
  }
  else if (dateStr.substring(0, 3) == "Jun") {
    return 6;
  }
  else if (dateStr.substring(0, 3) == "Jul") {
    return 7;
  }
  else if (dateStr.substring(0, 3) == "Aug") {
    return 8;
  }
  else if (dateStr.substring(0, 3) == "Sep") {
    return 9;
  }
  else if (dateStr.substring(0, 3) == "Oct") {
    return 10;
  }
  else if (dateStr.substring(0, 3) == "Nov") {
    return 11;
  }
  else if (dateStr.substring(0, 3) == "Dec") {
    return 12;
  }
}

int getDay() {
  return dateStr.substring(4, 6).toInt();
}

int getHour() {
  return timeStr.substring(0, 2).toInt();
}

int getMinute() {
  return timeStr.substring(3, 5).toInt();
}

int getSecond() {
  return timeStr.substring(6, 8).toInt();  
}

void printDateAndTime() {
  int n = 0;
  Serial.print("Date and time: ");
  n = Controllino_GetDay();
  Serial.print(n);
  Serial.print("/");
  n = Controllino_GetMonth();
  Serial.print(n);
  Serial.print("-");
  n = Controllino_GetYear();
  Serial.print(n);
  Serial.print("   ");
  n = Controllino_GetHour();
  Serial.print(n);
  Serial.print(":");
  n = Controllino_GetMinute();
  Serial.print(n);
}

void setup() {
  Serial.begin(9600);
  Serial.println(' ');
  Serial.println("booting");
  Serial.println(' ');
  
  year = getYear();
  month = getMonth();
  day = getDay();
  hour = getHour();
  minute = getMinute();
  second = getSecond();
  
  Serial.print("year = "); Serial.println(year);
  Serial.print("month = "); Serial.println(month);
  Serial.print("day = "); Serial.println(day);
  Serial.print("hour = "); Serial.println(hour);
  Serial.print("minute = "); Serial.println(minute);
  Serial.print("second = "); Serial.println(second);

  Controllino_RTC_init(0);
  Controllino_SetTimeDate(day, weekDay, month, year, hour, minute, second);
  delay(1000);
  printDateAndTime();
}

void loop() {
  // put your main code here, to run repeatedly:

}
