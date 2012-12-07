#ifndef _CONFIG_H_
#define _CONFIG_H_

// Features
#define SERIAL_PRINT_ENABLE 0
#define DEBUG_INTERFACE 0
#define DEBUG_SDCARD 0

#define ALL_LABELS 0

#define DISABLE_SDCARD_WRITING 0
#define DELETE_DATAFILE_ON_STARTUP 1

#define FAKE_GPS_DATA 0
#define ANTLER_LAKE 0
#define UOFA_2 0

#define MEM_DEBUG 0

// CONSTANTS

#define YP A14  // must be an analog pin, use "An" notation!
#define XM A15  // must be an analog pin, use "An" notation!
#define YM 30   // can be a digital pin
#define XP 31   // can be a digital pin

#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
// optional
#define LCD_RESET A4

// Touchscreen constants
#define MINPRESSURE 10
#define MAXPRESSURE 1000

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE	0x001F
#define	RED	 0xF800
#define	GREEN   0x07E0
#define CYAN	0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define BACKGROUND_COLOR 0x0010
#define SELECTED_COLOR 0x0410

// bmpDraw opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates. It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel). Increasing the buffer
// size takes more of the Arduino's precious RAM but
// makes loading a little faster. 20 pixels seems a
// good balance.

#define BUFFPIXEL 20

// standard U of A library settings, assuming Atmel Mega SPI pins
#define SD_CS 33  // Chip select line for SD card
#define TFT_CS 34   // Chip select line for TFT display
#define TFT_DC 7  // Data/command line for TFT
#define TFT_RST 32  // Reset line for TFT (or connect to +5V)

// Setup screens
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 348);
Adafruit_TFTLCD TFTLCD(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
Adafruit_ST7735 ST7735 = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// LED Pins
#define DATAWRITE_LED 11
#define GYRO_WARNING_LED 10

// GPS Setup
#define GPS_FIX_PIN 2
#define GPS_ENABLE_PIN 12
#define GPS_BAUD_RATE 4800
TinyGPS gps;

#endif
