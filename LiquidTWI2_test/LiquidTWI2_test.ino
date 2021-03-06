/*
 Demonstration sketch for Adafruit i2c/SPI LCD backpack
 using MCP23008 I2C expander
 ( http://www.ladyada.net/products/i2cspilcdbackpack/index.html )

 This sketch prints "Hello World!" to the LCD
 and shows the time.
 
  The circuit for Duemilanove
 * 5V to Arduino 5V pin
 * GND to Arduino GND pin
 * CLK to Analog #5
 * DAT to Analog #4

  For other boards, use the following chart (CLK=SCL,DAT=SDA)
  Board      I2C / TWI pins
  ---------  --------------------------
  Uno, Yun   A4 (SDA), A5 (SCL)
  Mega2560   20 (SDA), 21 (SCL)
  Leonardo    2 (SDA),  3 (SCL)
  Due        20 (SDA), 21 (SCL)
*/

// include the library code:
#include <Wire.h>
#include <LiquidTWI2.h>

// Connect via i2c, address 0x20 (A0-A2 not jumpered)
LiquidTWI2 lcd(0x3F);

void setup() {
  // set the LCD type
  lcd.setMCPType(LTI_TYPE_MCP23008);  
  //lcd.setMCPType(LTI_TYPE_MCP23017); 
  // set up the LCD's number of rows and columns:
  lcd.begin(20, 4);
  // Print a message to the LCD.
  lcd.print("hello, world!");
}

void loop() {

}

