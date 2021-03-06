// Environment Data Logger
// Purpose:: learn electronic circuit and component, Arduino software
// every 1 min, capture data and store SD card.

// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
//  Hardware component
//  RTC DS3221
//  BME280
//  Micro SD card IF
//  16x2 LCD temporary, will be taken place by OELD 128x64

#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
//#include "Adafruit_MAX31855.h"
#include "RTClib.h"
#include "Time.h"
#include <LiquidCrystal_I2C.h>

/*
    I2C Bus device addresses A4:SDA, A5:SCL VDD=3.3V
    RTC     0x68, EEPROM 0x57
    BME280  0x76
    I2C LCD 0x3F
*/

/*
   SPI CS D10, CLK D13, MOSI D11, MISO D12
   SD card 5V
*/

LiquidCrystal_I2C lcd(0x3F, 16, 2);
RTC_DS3231 rtc;

Adafruit_BME280 bme;
#define SEALEVELPRESSURE_HPA (1013.25)

char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

unsigned long delayTime = 60000;
unsigned long Temp, Humid, Press, Alt;
unsigned long Temp_o, Humid_o, Press_o, Alt_o;
char filename;

byte UpArrow[8] = {
  0b00000,
  0b00000,
  0b00100,
  0b01110,
  0b11111,
  0b00000,
  0b00000,
  0b00000
};

byte DownArrow[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b11111,
  0b01110,
  0b00100,
  0b00000,
  0b00000
};

void setup () {

  Temp_o = 20; Humid_o = 50; Press_o = 1000, Alt_o = 80;

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  // I2C setup
  Wire.begin();
  // I2C LCD setup
  Serial.println("Devices init ");
  Serial.println("Init LCD ");

  lcd.begin();
  lcd.clear();
  lcd.home();
  lcd.println("Init. devices. ");

  lcd.createChar(0, UpArrow);
  lcd.createChar(1, DownArrow);

  delay(500);

  //SD card setup
  Serial.println("Init SD card ");
  lcd.print("Init SD card ");

  if (!SD.begin(10)) {
    Serial.println("Init failed!");
    lcd.println("SD failed!");
    while (1);
  }
  Serial.println("SD done ");
  lcd.setCursor(0, 1);
  lcd.print("SD done ");

  delay(500); // wait for sensor stablity

  //RTC card setup
  if (! rtc.begin()) {
    Serial.println("No RTC ");
    lcd.println("No RTC");
    while (1);
  }
  Serial.println("RTC done.");
  lcd.setCursor(0, 1);
  lcd.println("RTC done ");

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  delay(500); // wait for sensor stablity

  //BME setup
  if (!bme.begin(0x76)) {
    Serial.println("No BME280!!");
    lcd.setCursor(0, 1);
    lcd.print("No BME280!!");
    while (1);
  }
  Serial.println("BME done.");
  lcd.setCursor(0, 1);
  lcd.println("BME done.");

  delay(500);
  lcd.setCursor(0, 1);
  lcd.print("All devices ok.");
  delay(3000);

}

void loop () {
  lcd.clear();
  DateTime now = rtc.now();
/*
  lcd.print(now.hour(), DEC);
  lcd.print(':');
  lcd.print(Fnow.minute(), DEC);
*/

  // Read values from BME280 sensor
  Temp = bme.readTemperature();
  Humid = bme.readHumidity();
  Press = bme.readPressure() / 100.0F;
  bme.readAltitude(SEALEVELPRESSURE_HPA);

  // lcdprt(); //write 16x2 LCD
  serialprt(); //write serial console
  csvout(); //write sd card

  if (Temp > Temp_o) {
    lcd.print(Temp); lcd.print(F("*C ")); lcd.write(byte(0)); lcd.print(Temp - Temp_o);lcd.print(F(" "));
  }
  else if (Temp < Temp_o) {
    lcd.print(Temp); lcd.print(F("*C ")); lcd.write((byte)1); lcd.print(Temp_o - Temp);lcd.print(F(" "));
  }
  else {
    lcd.print(Temp);lcd.print(F("*C "));lcd.print(F(" "));
  }

  lcd.setCursor(11,0);
  if (Humid > Humid_o) {
    lcd.print(Humid); lcd.print(F("%")); lcd.write(byte(0)); lcd.print(Humid - Humid_o);
  }
  else if (Humid < Humid_o) {
    lcd.print(Humid); lcd.print(F("%")); lcd.write((byte)1); lcd.print(Humid_o - Humid);
  }
  else {
    lcd.print(Humid);lcd.print(F("%"));
  }

  lcd.setCursor(0, 1);
  if (Press > Press_o) {
    lcd.print(Press); lcd.print("hPa "); lcd.write(byte(0)); lcd.print(Press - Press_o);
  }
  else if (Press < Press_o) {
    lcd.print(Press); lcd.print("hPa "); lcd.write((byte)1); lcd.print(Press_o - Press);
  }
  else {
    lcd.print(Press);lcd.print("hPa ");
  }
  lcd.setCursor(11,1);
  lcd.print( now.hour());
  lcd.print( ':');
  lcd.print( now.minute());
  lcd.home();

//every 10 mins 
  delay(600000);

  Temp_o = Temp;
  Humid_o = Humid;
  Press_o = Press;
}

void csvout() {

  File datafile = SD.open("data.log",FILE_WRITE);

  DateTime now = rtc.now();

  datafile.print( now.year());
  datafile.print( '/');
  datafile.print( now.month());
  datafile.print( '/');
  datafile.print( now.day());
  datafile.print( ",");
  datafile.print( now.hour());
  datafile.print( ':');
  datafile.print( now.minute());
  datafile.print( ':');
  datafile.print( now.second());
  datafile.print( ",");
  datafile.print( Temp);
  datafile.print( ",");
  datafile.print( Humid);
  datafile.print( ",");
  datafile.print( Press);
  datafile.print( ",");
  datafile.print( Alt);
  datafile.println();
  datafile.close();
}

void serialprt() {

  DateTime now = rtc.now();

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print( now.month(), DEC);
  Serial.print( '/');
  Serial.print( now.day(), DEC);
  Serial.print( ",");
  Serial.print( now.hour(), DEC);
  Serial.print( ':');
  Serial.print( now.minute(), DEC);
  Serial.print( ':');
  Serial.print( now.second(), DEC);
  Serial.print( ",");
  Serial.print( Temp);
  Serial.print( ",");
  Serial.print( Humid);
  Serial.print( ",");
  Serial.print( Press);
  Serial.print( ",");
  Serial.print( Alt);
  Serial.println();

}
