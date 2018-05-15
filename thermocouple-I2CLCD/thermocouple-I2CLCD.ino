/*************************************************** 
  This program is thermocouple and I2C'd LCD

Adafruit: MAX 31855
16x2 LCD

  
  BSD license, all text above must be included in any redistribution
 ****************************************************/
#include <SPI.h>
#include <Wire.h>
#include "Adafruit_MAX31855.h"
#include <LiquidCrystal_I2C.h>

//Digital I/O pins for MAX31855
#define MAXDO   11
#define MAXCS   10
#define MAXCLK  12

// Initialize the Thermocouple for SPI
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);

// initialize I2C'd LCD 16x2, Default device ID is 0x3F for 

LiquidCrystal_I2C lcd(0x3F, 16, 2);

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
   #define Serial SerialUSB
#endif
  
void setup() {
  #ifndef ESP8266
    while (!Serial);     // will pause Zero, Leonardo, etc until serial console opens
  #endif

  /*
   * Serial.begin(9600); 
   */

  // set up LCD 
  lcd.begin();
  lcd.clear();
  lcd.print("MAX31855 test");
  // wait for MAX chip to stabilize
  delay(500);
}

void loop() {
  // basic readout test, just print the current temp
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("Int. Temp = ");
   lcd.println(thermocouple.readInternal());
//   Serial.print("Int. Temp = ");
//   Serial.println(thermocouple.readInternal());
     
   double c = thermocouple.readCelsius();
   lcd.setCursor(0, 1);
   if (isnan(c)) 
   {
     lcd.print("T/C Problem");
   } 
   else 
   {
     lcd.print("C = "); 
     lcd.print(c);
     lcd.print("  "); 
//     Serial.print("Thermocouple Temp = *");
//     Serial.println(c);
   }
 
   delay(5000);
}
