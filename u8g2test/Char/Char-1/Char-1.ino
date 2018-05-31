#include <Arduino.h>
#include <Wire.h>
#include <U8x8lib.h>
//#include <U8g2lib.h>


/* Constructor */
//U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);  // Adafruit ESP8266/32u4/ARM Boards + FeatherWing OLED
U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);   // Adafruit ESP8266/32u4/ARM Boards + FeatherWing OLED

/* u8x8.begin() is required and will sent the setup/init sequence to the display */
void setup(void)
{
  u8x8.begin();
}

void loop(void)
{
  u8x8.setFont(u8x8_font_artossans8_r);
  u8x8.drawString(0,0,"Hello World!");
  delay(1000);
}
