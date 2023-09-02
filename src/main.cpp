#include <Arduino.h>
#include <NewPing.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h>
#include "settings.h" // config

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // Setup sensor
TFT_ILI9163C tft = TFT_ILI9163C(TFT_CS, TFT_DC, TFT_RST); // Setup TFT

unsigned long testText(int dist) {
  tft.fillScreen();

  tft.setCursor(10, 40);
  tft.setTextColor(WHITE);  
  tft.setTextSize(3);
  tft.println(String(dist) + " cm");
  return true;
}

void setup() {
  Serial.begin(9600);

  tft.begin();
  tft.setRotation(2); // if screen if upside down
}

void loop() {
  delay(1000);

  int distance = sonar.ping_cm();

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println("cm");

  testText(distance);
}
