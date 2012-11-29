//All this does is read the heading from HMC6352 and spit it out via serial
#include <Arduino.h>
#include <Wire.h>
// #include <TimerThree.h>

#include <Adafruit_GFX.h>	// Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>

#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>

#include <SD.h>

#include "classes/Sensors.h"
#include "classes/Sensors/ADXL345.h"
#include "classes/Sensors/HMC6352.h"
#include "classes/Sensors/T36GZ.h"

#include "classes/Displays.h"

#define YP A14  // must be an analog pin, use "An" notation!
#define XM A15  // must be an analog pin, use "An" notation!
#define YM 30   // can be a digital pin
#define XP 31   // can be a digital pin

#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 348);

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
// optional
#define LCD_RESET A4

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE	0x001F
#define	RED	 0xF800
#define	GREEN   0x07E0
#define CYAN	0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Adafruit_TFTLCD TFTLCD(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

// standard U of A library settings, assuming Atmel Mega SPI pins
#define SD_CS 33  // Chip select line for SD card
#define TFT_CS 34   // Chip select line for TFT display
#define TFT_DC 7  // Data/command line for TFT
#define TFT_RST 32  // Reset line for TFT (or connect to +5V)

Adafruit_ST7735 ST7735 = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void setup(){
	Serial.begin(9600);

	Serial.println("INIT");

	Sensors::Sensors();

	ADXL345::begin();
	
	TFTLCD.reset();
	
	ST7735.initR(INITR_REDTAB);   // initialize a ST7735R chip, red tab
  
	uint16_t identifier = TFTLCD.readID();

	if(identifier == 0x9325) {
		Serial.println("Found ILI9325 LCD driver");
	} else if(identifier == 0x9328) {
		Serial.println("Found ILI9328 LCD driver");
	} else if(identifier == 0x7575) {
		Serial.println("Found HX8347G LCD driver");
	} else {
		Serial.print("Unknown LCD driver chip: ");
		Serial.println(identifier, HEX);
		return;
	}
	
	TFTLCD.begin(identifier);
	TFTLCD.fillScreen(BLACK);
	TFTLCD.setCursor(0,0);
	TFTLCD.setTextColor(WHITE);
	TFTLCD.setRotation(3);
	
	ST7735.fillScreen(BLACK);
	ST7735.setCursor(0,0);
	ST7735.setTextColor(WHITE);
	ST7735.setRotation(3);
	
}

void loop()
{
	static int rows = 0;
	
	if(rows == 25)
	{
		ST7735.fillScreen(BLACK);
		ST7735.setCursor(0,0);
		
		TFTLCD.fillScreen(BLACK);
		TFTLCD.setCursor(0,0);
		
		rows = 0;
	}
	
	HMC6352::readData();
	ADXL345::readData();
	T36GZ::readData();
	
	#if SERIAL
	HMC6352::printData();
	ADXL345::printData(); // read the x/y/z tilt
	T36GZ::printData();
	#endif
	
	vector *ADXL345data = ADXL345::getData();
	
	therm *T36GZdata = T36GZ::getData();
	
	// ST7735.print("T:");
	// ST7735.print(Sensors::getTime());
	// 
	// ST7735.print(" T:");
	// ST7735.print(T36GZdata->temp);
	// 
	// ST7735.print(" C V:");
	// ST7735.println(T36GZdata->voltage);
	
	// TFTLCD.print("T:");
	// TFTLCD.print(Sensors::getTime());
	// 
	// TFTLCD.print(" G:");
	// TFTLCD.print(ADXL345data->x);
	// 
	// TFTLCD.print(",");
	// TFTLCD.print(ADXL345data->y);
	// 
	// TFTLCD.print(",");
	// TFTLCD.print(ADXL345data->z);
	// 
	// TFTLCD.print(" T:");
	// TFTLCD.print(T36GZdata->temp);
	// 
	// TFTLCD.print(" C V:");
	// TFTLCD.println(T36GZdata->voltage);
	
	Displays::print(&TFTLCD, T36GZdata);
	Displays::print(&TFTLCD, " ");
	Displays::println(&TFTLCD, ADXL345data);
	
	rows++;
	
	// delay(100);
}