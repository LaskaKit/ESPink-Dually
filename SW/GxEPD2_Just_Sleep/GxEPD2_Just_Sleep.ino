/*
* Example code deep sleep current measuring,
* on V1.0 measured 15.5 µA
*
* Compile as ESP32S3 dev module
* Board:   LaskaKit ESPink-Dually:  https://www.laskakit.cz/laskakit-espink-dually-e-paper/
* Display: Good Display GDEY1248Z51 https://www.laskakit.cz/good-display-gdey1248z51-12-48--1304x984-epaper-cerveny-displej/
*
* Libraries:
* EPD library: https://github.com/ZinggJM/GxEPD2
*
*/

// Display Library example for SPI e-paper panels from Dalian Good Display and boards from Waveshare.
// Requires HW SPI and Adafruit_GFX. Caution: the e-paper panels require 3.3V supply AND data lines!
//
// Display Library based on Demo Example from Good Display: http://www.e-paper-display.com/download_list/downloadcategoryid=34&isMode=false.html
//
// Author: Jean-Marc Zingg
//
// Version: see library.properties
//
// Library: https://github.com/ZinggJM/GxEPD2

#define ENABLE_GxEPD2_GFX 0

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <GxEPD2_7C.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include "bitmaps/Bitmaps1304x984.h" // 12.48" b/w

GxEPD2_3C < GxEPD2_1248c, GxEPD2_1248c::HEIGHT / 4 >
display(GxEPD2_1248c(/*sck=*/ 35, /*miso=*/ -1, /*mosi=*/ 36, /*cs_m1=*/ 17, /*cs_s1=*/ 18, /*cs_m2=*/ 40, /*cs_s2=*/ 46,
                            /*dc1=*/ 20, /*dc2=*/ 39, /*rst1=*/ 19, /*rst2=*/ 38, /*busy_m1=*/ 8, /*busy_s1=*/ 3, /*busy_m2=*/ 37, /*busy_s2=*/ 41));

#define SLEEP_SEC 15         // Measurement interval (seconds)
const char HelloWorld[] = "Hello World!";

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");
  delay(100);

// turn on power to display
  pinMode(47, OUTPUT);
  digitalWrite(47, HIGH);   // turn the Display on (HIGH is the voltage level)
  Serial.println("Display power ON");
  delay(1000);   
  
  display.init(); // inicializace

  // first update should be full refresh
  helloWorld();
  delay(5000);
  Serial.println("setup done");

  goToSleep();
}

void goToSleep(){
  Serial.println("going to sleep " + String(SLEEP_SEC) + " sek");
  // ESP Deep Sleep 
    
  display.powerOff();
  digitalWrite(47, LOW);   // turn the display OFF (LOW is the voltage level)
  Serial.println("Display power OFF");
  Serial.println("ESP in sleep mode");
  Serial.flush(); 
  esp_sleep_enable_timer_wakeup(SLEEP_SEC * 1000000);
  esp_deep_sleep_start();
}

void loop()
{
}


void helloWorld()
{
  //Serial.println("helloWorld");
  display.setRotation(1);
  display.setFont(&FreeMonoBold9pt7b);
  if (display.epd2.WIDTH < 104) display.setFont(0);
  display.setTextColor(GxEPD_BLACK);
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(HelloWorld, 0, 0, &tbx, &tby, &tbw, &tbh);
  // center bounding box by transposition of origin:
  uint16_t x = ((display.width() - tbw) / 2) - tbx;
  uint16_t y = ((display.height() - tbh) / 2) - tby;
  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x, y);
    display.print(HelloWorld);
  }
  while (display.nextPage());
  Serial.println("helloWorld done");
}
