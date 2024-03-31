#include <Arduino.h>
#include <NewPing.h>
#include <SPI.h>
#include <RTClib.h>
#include <ssd1306.h>
#include <nano_engine.h>
#include <EEPROM.h>
#include "pint.h"
#include "settings.h" // config

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // Setup sensor
RTC_DS3231 rtc; // Setup clock

NanoEngine8 engine;
NanoSprite<NanoEngine8, engine> pintglass({8, 12}, {59, 96}, pintGlassBMP); // x: 34 = center
NanoSprite<NanoEngine8, engine> heart({118, 5}, {7, 7}, heartBMP);

// EEPROM layout
// 0 = maximum hours
// 1 = week-4 hours
// 2 = week-3 hours
// 3 = week-2 hours
// 4 = last weeks hours
// 5 = current week hours

// TODO create setters/getters for these
int weeklyPints = 0;              // 1 pint = 5 hours / 300 minutes
int lastPintAt = 0;               // Number of hours when last pint was added
int weekHours = 0;                // Number of hours standing this week
int maxHours = 0;                 // Maximum number of hours standing in a week
int avgHours = 0;                 // Average number of hours standing in a week (last 4 weeks)
int resetDay = 0;                 // 0 = Sunday
float beerPerc = 0;               // Percentage of beer to fill pint glass
int currentSession = 0;           // Number of minutes standing in current session
bool showSprite = false;          // Show heart sprite when standing
int lastSensorReading = 0;        // Last distance sensor reading
unsigned long sensorReadTime = 0; // For counting seconds (lastSensorReading)
unsigned long timeRun = 0L;       // For counting minutes (currentSession)
#if !DEBUG
unsigned long minuteCounter = (60*1000L);
#else
unsigned long minuteCounter = (1*1000L); // Make time speedy
#endif
char daysOfTheWeek[7][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

void clockDebug() {
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

void calcAvgHours() {
  int counter = 0;
  int totalTime = 0;
  for (int i = 1; i <= 4; i++) {
    int week = EEPROM.read(i);
    if (week != 255) {
      totalTime += week;
      counter++;
      Serial.print(week); Serial.print(" hours added to totalTime: "); Serial.println(totalTime);
    }
  }
  avgHours = round(totalTime / counter);
}

void resetMax() {
  EEPROM.write(0, 255);
  Serial.println("Max hours reset.");
}

void resetAvg() {
  EEPROM.write(1, 255);
  EEPROM.write(2, 255);
  EEPROM.write(3, 255);
  EEPROM.write(4, 255);
  Serial.println("Avg hours reset.");
}

void resetWeek() {
  EEPROM.write(5, 255);
  Serial.println("Week hours reset.");
}

void printWeekHours() {
  engine.refresh(76, 34, 128, 44);
  char buffer[10];
  if (weekHours == 1) {
    sprintf(buffer, "1 hour");
  } else {
    sprintf(buffer, "%d hours", weekHours);
  }
  engine.canvas.printFixed(76, 34, buffer, STYLE_NORMAL);
  engine.refresh(76, 34, 128, 44);
}

void printMax() {
  engine.refresh(76, 62, 128, 72);
  char buffer[10];
  sprintf(buffer, "%d hours", maxHours);
  engine.canvas.printFixed(76, 62, buffer, STYLE_NORMAL);
  engine.refresh(76, 62, 128, 72);
}

void printAvg() {
  engine.refresh(76, 90, 128, 100);
  char buffer[10];
  sprintf(buffer, "%d hours", avgHours);
  engine.canvas.printFixed(76, 90, buffer, STYLE_NORMAL);
  engine.refresh(76, 90, 128, 100);
}

void printTime() {
  DateTime now = rtc.now();

  char buffer[10];
  sprintf(buffer, "%s %02d:%02d", daysOfTheWeek[now.dayOfTheWeek()], now.hour(), now.minute());
  engine.refresh(4, 119, 74, 128);
  engine.canvas.printFixed(4, 119, buffer, STYLE_NORMAL);
  engine.refresh(4, 119, 74, 128); 
}

void printPints() {
  engine.refresh(74, 119, 128, 128);
  char buffer[10];
  sprintf(buffer, "Pints: %d", weeklyPints);
  engine.canvas.printFixed(74, 119, buffer, STYLE_NORMAL);
  engine.refresh(74, 119, 128, 128);
}

void beerLevel() {
  int level = map(beerPerc, 0, 100, 0, 84);

  // fill the pint glass
  engine.refresh(10, 15, 64, 98); // refresh whole pint glass
  for (int i = 0; i < level; i++) {
    if (i >= 70) {
      engine.canvas.setColor(RGB_COLOR8(255,255,189)); // head
    } else {
      engine.canvas.setColor(RGB_COLOR8(255,179,0)); // beer
    }

    int x1 = pgm_read_word(&pintArray[i][0]);
    int y1 = pgm_read_word(&pintArray[i][1]);
    int x2 = pgm_read_word(&pintArray[i][2]);
    int y2 = pgm_read_word(&pintArray[i][3]);
    engine.canvas.drawLine(x1, y1, x2, y2);

    if (i == 81) { // special case for 81 where we need to draw two lines
      engine.canvas.drawLine(56, 17, 63, 17);
    }

    // bubbles?
    if (i > 1 && i < 70) {
      int bubbleX = random(22, 50);
      int bubbleY = random(constrain(pgm_read_word(&pintArray[level][1])+2, 28, 98), 98);
      engine.canvas.setColor(RGB_COLOR8(239,239,21));
      engine.canvas.drawLine(bubbleX, bubbleY, bubbleX, bubbleY);
    }
  }
  engine.refresh(10, 15, 64, 98);
}

bool isStanding() {
  int distance = lastSensorReading;
  // Read updated distance every second
  if (lastSensorReading == 0 || (millis() - sensorReadTime > 1000)) {
    distance = sonar.ping_cm();
    if (DEBUG) {
      Serial.print("Distance: "); Serial.print(distance); Serial.println("cm");
    }
    lastSensorReading = distance;
    sensorReadTime = millis();
  }

  if (distance <= 125) { // distance to roof when sitting: ~150cm
    return true;
  }
  return false;
}

bool isWorking() {
  DateTime now = rtc.now();
  // Mon - Fri
  if (now.dayOfTheWeek() >= 1 && now.dayOfTheWeek() <= 5 && now.hour() >= WORK_START && now.hour() <= WORK_END) {
    if (DEBUG) {
      Serial.println("Within working hours.");
    }
      return true;
  }
  if (DEBUG && ANYTIME) {
    Serial.println("Anytime mode.");
    return true;
  }
  return false;
}

void trackTime()  {
  if (isWorking() && isStanding()) {
    showSprite = true;
    if (millis() - timeRun >= minuteCounter) {
      timeRun = millis();
      currentSession++; // increment currentSession by 1 minute
      if (DEBUG) {
        Serial.print("Standing, current session: "); Serial.println(currentSession);
      }
    }

    // update pint glass
    beerPerc = ((float)(((weekHours - weeklyPints * 5) * 60) + currentSession) / 300) * 100;
    if (DEBUG) {
      Serial.print("Beer percentage: "); Serial.println(beerPerc);
    }

    if (currentSession >= 60) {
      weekHours++;
      EEPROM.write(5, weekHours); // update weekly hours in EEPROM

      // update max hours if necessary
      if (weekHours > maxHours) {
        maxHours = weekHours;
        EEPROM.write(0, maxHours);
      }
      currentSession = 0; // one hour has passed, reset timer
    }
  } else {
    showSprite = false;
  }

  // if 5 hours have passed, add a pint
  if (weekHours % 5 == 0 && weekHours != lastPintAt) {
    weeklyPints++;
    lastPintAt = weekHours;
  }

  // if end of week, save weekHours to EEPROM and reset  
  DateTime now = rtc.now();
  if (DEBUG && TEST_NEW_WEEK) {
    resetDay = now.dayOfTheWeek();
  }
  if (weekHours != 0 && now.dayOfTheWeek() == resetDay) { // run only once on Sunday
    Serial.print("Saving this weeks hours: "); Serial.println(weekHours);
    bool weekAvail = false;
    for (int i = 4; i >= 1; i--) {
      int week = EEPROM.read(i);
      if (week == 255) {
        EEPROM.write(i, weekHours);
        weekAvail = true;
        break;
      }
    }
    if (!weekAvail) {
      EEPROM.update(1, EEPROM.read(2));
      EEPROM.update(2, EEPROM.read(3));
      EEPROM.update(3, EEPROM.read(4));
    }
    // reset weekHours & weeklyPints values
    Serial.println("Resetting week.");
    weekHours = 0;
    EEPROM.write(5, weekHours);
    weeklyPints = 0;
    lastPintAt = 0;
    // recalculate avgHours
    calcAvgHours();
  }
}

bool drawAll() {
    engine.canvas.clear(); // Clear canvas
    engine.canvas.setMode(CANVAS_MODE_TRANSPARENT);  // We want to draw non-transparent bitmap
    engine.canvas.setColor(RGB_COLOR8(255,255,255));  // draw with white color
    pintglass.draw();

    engine.canvas.printFixed(76,  24, "THIS WK", STYLE_NORMAL);
    printWeekHours();

    engine.canvas.printFixed(76,  52, "MAX", STYLE_NORMAL);
    printMax();

    engine.canvas.printFixed(76,  80, "AVG", STYLE_NORMAL);
    printAvg();

    printTime();
    printPints();

    beerLevel();
    
    if (showSprite) {
      // add heart sprite
      engine.refresh(118, 5, 128, 12);
      engine.canvas.setColor(RGB_COLOR8(255,255,255));
      heart.draw();
      engine.refresh(118, 5, 128, 12);
    } else {
      // remove heart sprite
      engine.refresh(118, 5, 128, 12);
      engine.canvas.setColor(RGB_COLOR8(0,0,0));
      heart.draw();
      engine.refresh(118, 5, 128, 12);
    }

    return true;
}

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));
  
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

  if (DEBUG && RESET) {
    resetMax();
    resetAvg();
    resetWeek();
  }

  // Ensure reset day isn't today
  if (DEBUG) {
    DateTime now = rtc.now();
    if (now.dayOfTheWeek() == 6) {
      resetDay = now.dayOfTheWeek()-1;
    } else {
      resetDay = now.dayOfTheWeek()+1;
    }
  }

  // Set weekHours value
  weekHours = EEPROM.read(5);
  if (weekHours == 255) {
    Serial.println("No week hours set, setting to 0.");
    weekHours = 0;
  }
  
  // Set maxHours value
  maxHours = EEPROM.read(0);
  if (maxHours == 255) {
    Serial.println("No max hours set, setting to 0.");
    maxHours = 0;
  }

  // Set avgHours value
  calcAvgHours();

  // Set pint count
  weeklyPints = weekHours / 5;
  lastPintAt = weekHours;

  // Set beer percentage
  beerPerc = ((float)(((weekHours - weeklyPints * 5) * 60) + currentSession) / 300) * 100;

  // Setup screen
  ssd1306_setFixedFont(ssd1306xled_font6x8);
  il9163_setOffset(2, 1);
  il9163_128x128_spi_init(TFT_RST, TFT_CS, TFT_DC);

  // Setup nanoengine
  engine.begin();
  engine.setFrameRate(1);
  engine.drawCallback(drawAll);  // Set callback to draw parts, when NanoEngine8 asks
  engine.refresh();              // Makes engine to refresh whole display content at start-up
}

void loop() {
  if (!engine.nextFrame()) return;
  engine.display();
  //clockDebug();
  trackTime();
}