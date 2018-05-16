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
  lcd.print(F("Init SD card "));

  if (!SD.begin(10)) {
    Serial.println("Init failed!");
    lcd.println("SD failed!");
    while (1);
  }
  Serial.println("SD done ");
  lcd.setCursor(0, 1);
  lcd.print(F("SD done "));

  delay(500); // wait for sensor stablity

  //RTC card setup
  if (! rtc.begin()) {
    Serial.println(F("No RTC "));
    lcd.println(F("No RTC"));
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
    lcd.print(F(((("No BME280!!"));
    while (1);
  }
  Serial.println("BME done.");
  lcd.setCursor(0, 1);
  lcd.println("BME done.");

  delay(500);
  lcd.setCursor(0, 1);
  lcd.print("F"All devices ok.");
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

  Temp = bme.readTemperature();
  Humid = bme.readHumidity();
  Press = bme.readPressure() / 100.0F;
  bme.readAltitude(SEALEVELPRESSURE_HPA);

  serialprt(); //write serial console
  csvout(); //write sd card
 // lcdprt(); //write 16x2 LCD

  if (Temp > Temp_o) {
    lcd.print(F(Temp)); lcd.print(F("*C ")); lcd.write(byte(0)); lcd.print(F(Temp - Temp_o));lcd.print(F(" "));
    Temp_o = Temp;
  }
  else if (Temp < Temp_o) {
    lcd.print(F(Temp)); lcd.print(F("*C ")); lcd.write((byte)1); lcd.print(F(Temp_o - Temp));lcd.print(F(" "));
    Temp_o = Temp;
  }
  else {
    lcd.print(F(Temp));lcd.print(F("*C "));lcd.print(F(" "));Temp_o = Temp;
  }

  if (Humid > Humid_o) {
    lcd.print(F(Humid)); lcd.print(F("%")); lcd.write(byte(0)); lcd.print(F(Humid - Humid_o));
    Humid_o = Humid;
  }
  else if (Humid < Humid_o) {
    lcd.print(F(Humid)); lcd.print(F("%")); lcd.write((byte)1); lcd.print(F(Humid_o - Humid));
    Humid_o = Humid;
  }
  else {
    lcd.print(F(Humid));
    lcd.print(F("%"));
    Humid_o = Humid;
  }

  lcd.setCursor(0, 1);

  if (Press > Press_o) {
    lcd.print(F(Press)); lcd.print(F(hPa ")); lcd.write(byte(0)); lcd.print(F(Press - Press_o));
    Press_o = Press;
  }
  else if (Press < Press_o) {
    lcd.print(F(Press)); lcd.print(F("hPa ")); lcd.write((byte)1); lcd.print(F(Press_o - Press));
    Press_o = Press;
  }
  else {
    lcd.print(F(Press));
    lcd.print(F("hPa "));
    Press_o = Press;
  }

  delay(delayTime);
}

void csvout() {

  File datafile = SD.open("data.log",FILE_WRITE);
  if (datafile) {
    csvout();
  }

  DateTime now = rtc.now();

  datafile.print(F(now.year()));
  datafile.print(F()'/'));
  datafile.print(F(now.month());
  datafile.print(F('/'));
  datafile.print(F(now.day()));
  datafile.print(F(","));
  datafile.print(F(now.hour()));
  datafile.print(F(':'));
  datafile.print(F(now.minute()));
  datafile.print(F(':'));
  datafile.print(F(now.second()));
  datafile.print(F(","));
  datafile.print(F(Temp));
  datafile.print(F(","));
  datafile.print(F(Humid));
  datafile.print(F(","));
  datafile.print(F(FPress));
  datafile.print(F(","));
  datafile.print(F(Alt));
  datafile.println();
  datafile.close();
}

void serialprt() {

  DateTime now = rtc.now();

  Serial.print(F(now.year(), DEC));
  Serial.print(F('/');
  Serial.print(F(now.month(), DEC));
  Serial.print(F('/'));
  Serial.print(F(now.day(), DEC));
  Serial.print(F(","));
  Serial.print(F(now.hour(), DEC));
  Serial.print(F(':'));
  Serial.print(F(now.minute(), DEC));
  Serial.print(F(':'));
  Serial.print(F(now.second(), DEC));
  Serial.print(F(","));
  Serial.print(F(Temp));
  Serial.print(F(","));
  Serial.print(F(Humid));
  Serial.print(F(","));
  Serial.print(F(FPress));
  Serial.print(F(","));
  Serial.print(F(Alt));
  Serial.println();
  Serial.close();
}
