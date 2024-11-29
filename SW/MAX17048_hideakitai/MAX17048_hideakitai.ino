/* Battery percentage test for LaskaKit ESPink-4.26"
 * 
 * Board:   LaskaKit ESPink-4.26   
 *
 * Libraries:
 * EPD library: https://github.com/ZinggJM/GxEPD2
 * MAX17048>  https://github.com/hideakitai/MAX17048
 * Email:podpora@laskakit.cz
 * Web:laskakit.cz
 */

#include <Wire.h>
#include <GxEPD2_3C.h>
#include "MAX17048.h"
#include <Fonts/FreeMonoBold12pt7b.h>
#include <driver/rtc_io.h>

#define POWER 47
#define SDA   42
#define SCL   2
#define ENABLE_GxEPD2_GFX 0
#define TIME_TO_SLEEP 5           // Time ESP32 will go to sleep (in seconds)
#define uS_TO_S_FACTOR 1000000ULL // Conversion factor for micro seconds to seconds
RTC_DATA_ATTR int bootCount = 0;  // Variable for keeping number of wakeups
#define TEXT_PADDING 30

GxEPD2_3C<GxEPD2_1248c, GxEPD2_1248c::HEIGHT / 4> display(GxEPD2_1248c(/*sck=*/ 12, /*miso=*/ -1, /*mosi=*/ 11, /*cs_m1=*/ 10, /*cs_s1=*/ 18, /*cs_m2=*/ 48, /*cs_s2=*/ 41,
                            /*dc1=*/ 46, /*dc2=*/ 45, /*rst1=*/ 3, /*rst2=*/ 39, /*busy_m1=*/ 8, /*busy_s1=*/ 17, /*busy_m2=*/ 40, /*busy_s2=*/ 16));
MAX17048 pwr_mgmt;

void start_sleep()
{
  gpio_hold_en((gpio_num_t)POWER); // Hold Epaper on during sleep
  gpio_deep_sleep_hold_en();                   // Hold Epaper on during sleep
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}

void print_middle_line(char *text, uint16_t padding)
{
  int16_t tbx, tby;
  uint16_t tbw, tbh;
  display.setFont(&FreeMonoBold12pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh);
  display.setCursor((display.width() - tbw) / 2, padding);
  display.println(text);
}

void display_text()
{
  char text[40];
  display.fillScreen(GxEPD_WHITE);
  sprintf(text, "Battery voltage is %0.2f V", pwr_mgmt.voltage());
  //
  print_middle_line(text, TEXT_PADDING);
  sprintf(text, "Battery Percentage is %d %%", pwr_mgmt.percent());

  print_middle_line(text, TEXT_PADDING * 2);
  sprintf(text, "Battery Accurate Percentage is %0.2f %%", pwr_mgmt.accuratePercent());

  print_middle_line(text, TEXT_PADDING * 3);
  sprintf(text, "Went to sleep %d times", bootCount);

  print_middle_line(text, TEXT_PADDING * 4);
  sprintf(text, "Next wake up in %d seconds", TIME_TO_SLEEP);

  print_middle_line(text, TEXT_PADDING * 5);
  sprintf(text, "Going to sleep");
  
  print_middle_line(text, TEXT_PADDING * 6);
  display.display(true);
}

void setup()
{
  Serial.begin(115200);
  Wire.begin (SDA, SCL);
  pwr_mgmt.attatch(Wire); //připojí čip MAX17048 k desce

  pinMode(POWER, OUTPUT);    // Set epaper transistor as output
  digitalWrite(POWER, HIGH); // Turn ON display
  delay(100);                            // Delay so it has time to turn on

  if (bootCount)                         // If first boot, redraw
  {
    display.init(0, false);
  }
  else
  {
    display.init(0, true);
  }
  display.setRotation(0);
  display.setPartialWindow(0, 0, display.width(), display.height()); // Set display window for fast update

  display_text();
  bootCount++; // Increment boot number every reboot
  start_sleep();
}

void loop()
{
}
