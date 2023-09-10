#include <Arduino.h>
#include <NewPing.h>
#include <SPI.h>
#include <RTClib.h>
#include <ssd1306.h>
#include <nano_engine.h>
#include "pint.h"

#include "settings.h" // config
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // Setup sensor
RTC_DS3231 rtc; // Setup clock

NanoEngine8 engine;
NanoSprite<NanoEngine8, engine> sprite( {120, 2}, {7, 7}, heartBMP );
NanoSprite<NanoEngine8, engine> sprite2( {8, 12}, {59, 96}, pintGlassBMP ); // x: 34 = center

bool drawAll() {
    engine.canvas.clear();
    engine.canvas.setMode(0);  // We want to draw non-transparent bitmap
    engine.canvas.setColor(RGB_COLOR8(255,0,0));  // draw with red color
    sprite.draw();
    engine.canvas.setColor(RGB_COLOR8(255,255,255));  // draw with white color
    engine.canvas.printFixed(58,  2, "@sudosammy", STYLE_NORMAL);

    engine.canvas.setColor(RGB_COLOR8(255,255,255));  // draw with white color
    sprite2.draw();

    engine.canvas.setColor(RGB_COLOR8(255,255,0));  // draw with yellow color
    engine.canvas.printFixed(76,  24, "THIS WK", STYLE_NORMAL);
    engine.canvas.printFixed(76,  34, "1 hour", STYLE_NORMAL);

    engine.canvas.printFixed(76,  52, "MAX", STYLE_NORMAL);
    engine.canvas.printFixed(76,  62, "25 hours", STYLE_NORMAL);

    engine.canvas.printFixed(76,  80, "AVG", STYLE_NORMAL);
    engine.canvas.printFixed(76,  90, "6 hours", STYLE_NORMAL);

    engine.canvas.printFixed(4,  119, "TUE 17:05", STYLE_NORMAL);
    engine.canvas.printFixed(74,  119, "Pints: 3", STYLE_NORMAL);

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

  // Setup screen
  ssd1306_setFixedFont(ssd1306xled_font6x8);
  il9163_setOffset(2, 1);
  il9163_128x128_spi_init(TFT_RST, TFT_CS, TFT_DC);

  // Setup nanoengine
  engine.begin();
  engine.setFrameRate(45);
  engine.drawCallback( drawAll );  // Set callback to draw parts, when NanoEngine8 asks
  engine.refresh();                // Makes engine to refresh whole display content at start-up
}

void loop() {
  delay(1000);

  int distance = sonar.ping_cm();

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println("cm");

  //il9163_setRotation(0); // this seems to shift the offset -1px on the x-axis. wtf?

  if (!engine.nextFrame()) return;
  // You will see horizontal flying heart
  //sprite.moveBy( { 1, 0 } );
  engine.display();                // refresh display content

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