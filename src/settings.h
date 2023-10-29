// Senor pins
#define TRIGGER_PIN     6
#define ECHO_PIN        5
#define MAX_DISTANCE    300 // Maximum distance we want to measure (in centimeters).

// Clock pins
#define CLOCK_CONNECTED true // set false if no clock

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
