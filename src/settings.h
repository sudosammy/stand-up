// Senor pins
#define TRIGGER_PIN  6
#define ECHO_PIN     5
#define MAX_DISTANCE 300 // Maximum distance we want to measure (in centimeters).

// Specific to my TFT
#define _TFTWIDTH  		130
#define _TFTHEIGHT 		130
#define _GRAMWIDTH      130
#define _GRAMHEIGH      130
#define __OFFSET		1

// TFT details
// LED = 5v (contains 100 ohm resistor)
// SCK = SCLK 13
// SDA = MOSI 11
// A0  = DC (any free pin, 9)
// RESET = (any free pin, 8)
// CS = SS 10
// GND = GND
// VCC = 3.3v
#define TFT_CS   10
#define TFT_DC   9
#define TFT_RST  8

// Colour definitions
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0  
#define WHITE   0xFFFF