// Debug options
#define DEBUG           false       // Set true to make 1 minute = 1 sec
#define RESET_ALL       false        // Set true to reset EEPROM on boot
// DEBUG must be true for the following to work
#define ANYTIME         false       // Set true to ignore work days/times
#define TEST_NEW_WEEK   false       // Set true to set today as the data save day

// Workday
#define WORK_START      8       // Start time in 24h format
#define WORK_END        18      // End time in 24h format

// Senor pins
#define TRIGGER_PIN     6
#define ECHO_PIN        5
#define MAX_DISTANCE    300     // Maximum distance we want to measure (in centimeters)

// TFT details
// LED = 5v (contains 100 ohm resistor)
// SCK = SCLK 13
// SDA = MOSI 11
// A0  = DC (any free pin, 9)
// RESET = (any free pin, 8)
// CS = SS 10
// GND = GND
// VCC = 3.3v
#define TFT_CS          10
#define TFT_DC          9
#define TFT_RST         8
