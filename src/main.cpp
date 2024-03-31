#include <Arduino.h>
#include <NewPing.h>
#include <SPI.h>
#include <RTClib.h>
#include <ssd1306.h>
#include <nano_engine.h>
#include <EEPROM.h>
#include "pint.h"

#include "settings.h" // config
char daysOfTheWeek[7][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // Setup sensor
RTC_DS3231 rtc; // Setup clock

NanoEngine8 engine;
NanoSprite<NanoEngine8, engine> sprite( {120, 2}, {7, 7}, heartBMP );
NanoSprite<NanoEngine8, engine> sprite2( {8, 12}, {59, 96}, pintGlassBMP ); // x: 34 = center

// EEPROM layout
// 0 = maximum hours
// 1 = week-1 hours
// 2 = week-2 hours
// 3 = week-3 hours
// 4 = week-4 hours

int beerPerc = 0;
int weeklyPints = 0;
int weekHours = 0;
int maxHours = 0;
int avgHours = 0;

void clockDebug() {
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
  } else {
    Serial.println("Check CLOCK_CONNECTED setting.");
  }
}

void printWeekHours() {
  engine.refresh(76, 34, 128, 128);
  char buffer[10];
  if (weekHours == 1) {
    sprintf(buffer, "1 hour");
  } else {
    sprintf(buffer, "%d hours", weekHours);
  }
  engine.canvas.printFixed(76,  34, buffer, STYLE_NORMAL);
  engine.refresh(76, 34, 128, 128);
}

void printMax(int max) {
  engine.refresh(76, 62, 128, 128);
  char buffer[10];
  sprintf(buffer, "%d hours", max);
  engine.canvas.printFixed(76,  62, buffer, STYLE_NORMAL);
  engine.refresh(76, 62, 128, 128);
}

void printAvg(int avg) {
  engine.refresh(76, 90, 128, 128);
  char buffer[10];
  sprintf(buffer, "%d hours", avg);
  engine.canvas.printFixed(76,  90, buffer, STYLE_NORMAL);
  engine.refresh(76, 90, 128, 128);
}

void printTime() {
  if (CLOCK_CONNECTED) {
    DateTime now = rtc.now();

    char buffer[10];
    sprintf(buffer, "%s %02d:%02d", daysOfTheWeek[now.dayOfTheWeek()], now.hour(), now.minute());
    Serial.println(buffer);

    engine.refresh(4, 119, 128, 128);
    engine.canvas.printFixed(4,  119, buffer, STYLE_NORMAL);
    engine.refresh(4, 119, 128, 128); 
  }
}

void printPints() {
  engine.refresh(74, 119, 128, 128);
  char buffer[10];
  sprintf(buffer, "Pints: %d", weeklyPints);
  engine.canvas.printFixed(74,  119, buffer, STYLE_NORMAL);
  engine.refresh(74, 119, 128, 128);
}

void beerLevel(int perc) {
  engine.refresh(10, 98, 60, 30);
  engine.canvas.setColor(RGB_COLOR8(255,179,0));
  engine.canvas.drawLine(24, 98, 51, 98);
  engine.canvas.drawLine(20, 97, 55, 97);
  engine.canvas.drawLine(19, 96, 55, 96);
  engine.canvas.drawLine(19, 95, 55, 95);
  engine.canvas.drawLine(19, 94, 56, 94);

  engine.canvas.drawLine(19, 93, 56, 93);
  engine.canvas.drawLine(19, 92, 56, 92);
  engine.canvas.drawLine(19, 91, 56, 91);
  engine.canvas.drawLine(19, 90, 56, 90);

  engine.canvas.drawLine(18, 89, 56, 89);
  engine.canvas.drawLine(18, 88, 56, 88);
  engine.canvas.drawLine(18, 87, 56, 87);
  engine.canvas.drawLine(18, 86, 56, 86);

  engine.canvas.drawLine(18, 85, 57, 85);
  engine.canvas.drawLine(18, 84, 57, 84);
  engine.canvas.drawLine(18, 83, 57, 83);
  engine.canvas.drawLine(18, 82, 57, 82);
  engine.canvas.drawLine(18, 81, 57, 81);
  engine.canvas.drawLine(18, 80, 57, 80);
  
  engine.canvas.drawLine(17, 79, 57, 79);
  engine.canvas.drawLine(17, 78, 57, 78);
  engine.canvas.drawLine(17, 77, 57, 77);
  engine.canvas.drawLine(17, 76, 57, 76);

  engine.canvas.drawLine(17, 75, 58, 75);
  engine.canvas.drawLine(17, 74, 58, 74);
  engine.canvas.drawLine(17, 73, 58, 73);
  engine.canvas.drawLine(17, 72, 58, 72);
  engine.canvas.drawLine(17, 71, 58, 71);
  engine.canvas.drawLine(17, 70, 58, 70);

  engine.canvas.drawLine(16, 69, 58, 69);
  engine.canvas.drawLine(16, 68, 58, 68);
  engine.canvas.drawLine(16, 67, 58, 67);
  engine.canvas.drawLine(16, 66, 58, 66);
  engine.canvas.drawLine(16, 65, 58, 65);

  engine.canvas.drawLine(16, 64, 59, 64);
  engine.canvas.drawLine(16, 63, 59, 63);
  engine.canvas.drawLine(16, 62, 59, 62);
  engine.canvas.drawLine(16, 61, 59, 61);
  engine.canvas.drawLine(16, 60, 59, 60);

  engine.canvas.drawLine(15, 59, 59, 59);
  engine.canvas.drawLine(15, 58, 59, 58);
  engine.canvas.drawLine(15, 57, 59, 57);
  engine.canvas.drawLine(15, 56, 59, 56);
  engine.canvas.drawLine(15, 55, 59, 55);

  engine.canvas.drawLine(15, 54, 60, 54);
  engine.canvas.drawLine(15, 53, 60, 53);

  engine.canvas.drawLine(14, 52, 60, 52);
  engine.canvas.drawLine(14, 51, 61, 51);
  engine.canvas.drawLine(13, 50, 61, 50);
  engine.canvas.drawLine(13, 49, 62, 49);

  engine.canvas.drawLine(12, 48, 62, 48);
  engine.canvas.drawLine(12, 47, 63, 47);
  engine.canvas.drawLine(11, 46, 63, 46);
  engine.canvas.drawLine(11, 45, 63, 45);
  
  engine.canvas.drawLine(11, 44, 64, 44);
  engine.canvas.drawLine(11, 43, 64, 43);
  engine.canvas.drawLine(11, 42, 64, 42);
  engine.canvas.drawLine(11, 41, 64, 41);
  engine.canvas.drawLine(11, 40, 64, 40);
  engine.canvas.drawLine(11, 39, 64, 39);
  engine.canvas.drawLine(11, 38, 64, 38);
  engine.canvas.drawLine(11, 37, 64, 37);
  engine.canvas.drawLine(11, 36, 64, 36);
  engine.canvas.drawLine(11, 35, 63, 35);

  engine.canvas.drawLine(12, 34, 63, 34);
  engine.canvas.drawLine(12, 33, 63, 33);

  engine.canvas.drawLine(12, 32, 62, 32);
  engine.canvas.drawLine(12, 31, 62, 31);
  engine.canvas.drawLine(13, 30, 62, 30);

  engine.canvas.setColor(RGB_COLOR8(255,255,189));
  engine.canvas.drawLine(13, 29, 62, 29);
  engine.canvas.drawLine(13, 28, 62, 28);
  engine.canvas.drawLine(12, 27, 62, 27);
  engine.canvas.drawLine(12, 26, 62, 26);
  engine.canvas.drawLine(12, 25, 62, 25);
  engine.canvas.drawLine(12, 24, 62, 24);
  engine.canvas.drawLine(12, 23, 62, 23);
  engine.canvas.drawLine(12, 22, 63, 22);
  engine.canvas.drawLine(12, 21, 63, 21);
  engine.canvas.drawLine(12, 20, 63, 20);
  engine.canvas.drawLine(12, 19, 63, 19);
  engine.canvas.drawLine(12, 18, 63, 18);

  engine.canvas.drawLine(11, 17, 19, 17);
  engine.canvas.drawLine(56, 17, 63, 17);
  engine.canvas.drawLine(24, 16, 51, 16);
  engine.canvas.drawLine(15, 15, 59, 15);

  // add bubbles
  for (int i = 0; i < 50; i++) {
    int x = random(20, 55);
    int y = random(32, 96);
    engine.canvas.setColor(RGB_COLOR8(239,239,21));
    engine.canvas.drawLine(x, y, x, y);
    // engine.canvas.drawLine(x, y-1, x, y-1); //above
    // engine.canvas.drawLine(x, y+1, x, y+1); //below
    // engine.canvas.drawLine(x+1, y, x+1, y); //right
    // engine.canvas.drawLine(x-1, y, x-1, y); //left
  }

  engine.refresh(10, 98, 60, 30);
}


bool drawAll() {
    engine.canvas.clear(); // Clear canvas
    engine.canvas.setMode(CANVAS_MODE_TRANSPARENT);  // We want to draw non-transparent bitmap
    engine.canvas.setColor(RGB_COLOR8(255,255,255));  // draw with white color
    sprite2.draw(); // Draw pint glass

    engine.canvas.printFixed(76,  24, "THIS WK", STYLE_NORMAL);
    printWeekHours();

    engine.canvas.printFixed(76,  52, "MAX", STYLE_NORMAL);
    printMax(maxHours);

    engine.canvas.printFixed(76,  80, "AVG", STYLE_NORMAL);
    printAvg(avgHours);

    printTime();
    printPints();

    beerPerc = 10;
    beerLevel(beerPerc);

    return true;
}

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));

  if (CLOCK_CONNECTED) {
    if (!rtc.begin()) {
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

  // Set maxHours value
  maxHours = EEPROM.read(0);
  if (maxHours == 255) {
    Serial.println("No max hours set, setting to 0.");
    maxHours = 0;
  }
  // Set avgHours value
  int counter = 0;
  int totalTime = 0;
  for (int i = 1; i <= 4; i++) {
    int week = EEPROM.read(i);
    if (week != 255) {
      Serial.print("Adding: "); Serial.print(week); Serial.print(" hours to totalTime: "); Serial.println(totalTime);
      totalTime += week;
      counter++;
    }
  }
  avgHours = round(totalTime / counter);

  // Setup screen
  ssd1306_setFixedFont(ssd1306xled_font6x8);
  il9163_setOffset(2, 1);
  il9163_128x128_spi_init(TFT_RST, TFT_CS, TFT_DC);

  // Setup nanoengine
  engine.begin();
  engine.setFrameRate(30);
  engine.drawCallback(drawAll);  // Set callback to draw parts, when NanoEngine8 asks
  engine.refresh();                // Makes engine to refresh whole display content at start-up
}

void loop() {
  if (!engine.nextFrame()) return;
  engine.display();  
  //clockDebug();

  int distance = sonar.ping_cm();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println("cm");

  delay(1000);
}