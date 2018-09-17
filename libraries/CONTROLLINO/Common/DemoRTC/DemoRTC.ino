#include <SPI.h>
#include <Controllino.h>  /* Usage of CONTROLLINO library allows you to use CONTROLLINO_xx aliases in your sketch. */

/*
  CONTROLLINO - Demonstration of Real Time Clock usage, Version 01.00

  Periodically reads out the date and time information from the RTC chip.
  As it is done each 5 seconds, the number of seconds read out from the chip should be always for 5 seconds higher.
  Compatible with CONTROLLINO MINI, MAXI and MEGA.

  IMPORTANT INFORMATION!
  Please, select proper target board in Tools->Board->Controllino MINI/MAXI/MEGA before Upload to your CONTROLLINO.

  Do not forget to properly setup the mechanical switch at your CONTROLLINO MINI!
  
  (Refer to https://github.com/CONTROLLINO-PLC/CONTROLLINO_Library if you do not see the CONTROLLINOs in the Arduino IDE menu Tools->Board.)

  You need only your PC, CONTROLLINO and USB cable. 
  
  Created 12 Jan 2017
  by Lukas

  https://controllino.biz/
  
  (Check https://github.com/CONTROLLINO-PLC/CONTROLLINO_Library for the latest CONTROLLINO related software stuff.)
*/


// The setup function runs once when you press reset (CONTROLLINO RST button) or connect power supply (USB or external 12V/24V) to the CONTROLLINO.
void setup() {
  // initialize serial communication at 9600 bits per second
  Serial.begin(9600);
  
  Controllino_RTC_init(0);

  Controllino_SetTimeDate(12,4,1,17,15,41,23); // set initial values to the RTC chip
}

// the loop function runs over and over again forever
void loop() {
  int n;  
  Serial.print("Day: ");n = Controllino_GetDay(); Serial.println(n);
  
  Serial.print("WeekDay: ");n = Controllino_GetWeekDay(); Serial.println(n);
  
  Serial.print("Month: ");n = Controllino_GetMonth(); Serial.println(n);

  Serial.print("Year: ");n = Controllino_GetYear(); Serial.println(n);

  Serial.print("Hour: ");n = Controllino_GetHour(); Serial.println(n);

  Serial.print("Minute: "); n = Controllino_GetMinute(); Serial.println(n);

  Serial.print("Second: ");n = Controllino_GetSecond(); Serial.println(n);
 
  delay(5000);        
}

/* End of the example. Visit us at https://controllino.biz/ or https://github.com/CONTROLLINO-PLC/CONTROLLINO_Library or contact us at info@controllino.biz if you have any questions or troubles. */

/* 2017-01-12: The sketch was successfully tested with Arduino 1.6.13, Controllino Library 1.0.0 and CONTROLLINO MINI, MAXI and MEGA. */

