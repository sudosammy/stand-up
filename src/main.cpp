#include <Arduino.h>
#include <NewPing.h>
#include <SPI.h>
#include <RTClib.h>
#include <ssd1306.h>
#include <nano_engine.h>
//#include "sova.h"

#include "settings.h" // config
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // Setup sensor
RTC_DS3231 rtc; // Setup clock

bool textDemo(int dist) {
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1306_clearScreen8();
    ssd1306_setColor(RGB_COLOR8(255,255,0));
    ssd1306_printFixed8(8,  16, "put dist in here", STYLE_NORMAL);

    return true;
}

void setup() {
  Serial.begin(9600);

  if (CLOCK_CONNECTED) {
    if (! rtc.begin()) {
      Serial.println("Couldn't find RTC");
      Serial.flush();
      while (1) delay(10);
    }

    if (rtc.lostPower()) {
      Serial.println("RTC lost power, let's set the time!");
      // When time needs to be set on a new device, or after a power loss, the
      // following line sets the RTC to the date & time this sketch was compiled
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
  }

  ssd1306_setFixedFont(ssd1306xled_font6x8);
  il9163_setOffset(2, 1);
  il9163_128x128_spi_init(TFT_RST, TFT_CS, TFT_DC);
  ssd1306_setMode(LCD_MODE_NORMAL);
  ssd1306_clearScreen8();
}

void loop() {
  delay(1000);

  int distance = sonar.ping_cm();

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println("cm");

  //il9163_setRotation(0); // this seems to shift the offset -1px on the x-axis. wtf?
  textDemo(distance);

  if (CLOCK_CONNECTED) {
    DateTime now = rtc.now();
      
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
  }
  
}