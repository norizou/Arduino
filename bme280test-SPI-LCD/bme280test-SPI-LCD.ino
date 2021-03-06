/***************************************************************************
  This is a library for the BME280 humidity, temperature & pressure sensor

  Designed specifically to work with the Adafruit BME280 Breakout
  ----> http://www.adafruit.com/products/2650

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface. The device's I2C address is either 0x76 or 0x77.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

//#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <LiquidCrystal.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


//Adafruit_BME280 bme; // I2C
Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

unsigned long delayTime;

void setup() {

    bool status;
        
    delayTime = 3000;

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Weather Monitor");
  lcd.setCursor(0, 1);
  lcd.print("BME280 test");

    // default settings
    // (you can also pass in a Wire library object like &Wire2)
    status = bme.begin();  
    if (!status) {
        lcd.print("Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }

    delay(delayTime);

}


void loop() { 
    lcd.clear();
    printValues();
    delay(delayTime);
}

void printValues() {
    lcd.setCursor(0, 0);
    lcd.print("Temp. = ");
    lcd.print(bme.readTemperature());
//    lcd.println(" *C");

    lcd.setCursor(0, 1);
    lcd.print("Humid. = ");
    lcd.print(bme.readHumidity());
//    lcd.println(" %");

    delay(delayTime);

    lcd.setCursor(0, 0);
    lcd.print("Press. = ");
    lcd.print(bme.readPressure() / 100.0F);
//    lcd.println(" hPa");

    lcd.setCursor(0, 1);
    lcd.print("Alt. = ");
    lcd.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
//    lcd.println(" m");
}
