//All this does is read the heading from HMC6352 and spit it out via serial
#include <Arduino.h>

#include <Wire.h> // Interface library for use with sensors
#include <TimerThree.h> // Timer library for use with the GPS check

#include <TinyGPS.h> // TinyGPS library to parse the NMEA data

#include <Adafruit_GFX.h>// Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h> // TouchScreen library for interfacing with the TFTLCD

#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h> // SPI to use the specilized ports for the LCD display

#include <SD.h> // To read/write from/to the SD card

#include "classes/mem_syms.h"

#include "structs/structs.h" // Contains all structures used globally
#include "classes/Config.h" // Contains the configuration files and global variables
#include "classes/DataLog.h" // Contains the DataLog class

#include "classes/Sensors.h" // Contains the Sensor class and utility classes
#include "classes/Sensors/ADXL345.h" // Contains the ADXL345 class
#include "classes/Sensors/HMC6352.h" // Contains the HMC6352 class
#include "classes/Sensors/T36GZ.h" // Contains the T36GZ class
#include "classes/Sensors/GTPA010.h" // Contains the GTPA010 class

#include "classes/MapData.h" // Contains the mapping data class

#include "classes/Displays.h" // Contains the Displays class as well as the utility classes for displaying content on the screen
#include "classes/Displays/DADXL345.h" // Contains the DADXL345 class
#include "classes/Displays/DHMC6352.h" // Contains the DHMC6352 class
#include "classes/Displays/DT36GZ.h" // Contains the DT36GZ class
#include "classes/Displays/DGTPA010.h" // Contains the DGTPA010 class

Interface myInt = Interface::Interface();

/* Global initilization of core classes */
Interface interface = Interface::Interface(); // Interface definition
Sd2Card card; // SD card definition and initilization
DataLog * data; // DataLog initilization and definition

// Setup function, core to the Arduino setup
void setup(){
	
	Serial.begin(9600); // Start the hardware serial interface
	// Serial.println("INIT"); // Print INIT over the serial to ensure serial is working correctly
	
	Sensors::Sensors(); // Setup wire interface
	
	ADXL345::begin(); // Setup the ADXL345 device
	pinMode(GYRO_WARNING_LED, OUTPUT);
	
	GTPA010::begin(); // Setup the GTPA010 device
	
	/* ----------------------- Screen initilizations -----------------------  */
	
	// Initilize screens
	ST7735.initR(INITR_REDTAB);   // initialize a ST7735R chip, red tab
	TFTLCD.reset();
	
	uint16_t identifier = TFTLCD.readID(); // Read the identifier from the TFT screen
	
	#if SERIAL_PRINT_ENABLE
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
	#endif
	
	// Setup Displays
	TFTLCD.begin(identifier); // TFT Touchscreen
	TFTLCD.setTextColor(WHITE);
	TFTLCD.setRotation(3);
	
	ST7735.fillScreen(BLACK); // LCD Screen
	ST7735.setTextColor(WHITE);
	ST7735.setRotation(3);
	
	// Setup the SD card
	if (!SD.begin(SD_CS))
	{
		Serial.println("Failed to initilize SD card!");
		Displays::println(&ST7735, "Failed to initilize SD card!");
		return;
	}
	
	data = new DataLog("DATAFILE.CSV"); // Setup the datalogger + indicator light
	pinMode(DATAWRITE_LED, OUTPUT);
	
	// Setup display interface
	myInt.setDisplay(&TFTLCD, BACKGROUND_COLOR);
	
	myInt.addObject("Compass", BLUE, 0);
	myInt.addObject("GPS", BLUE, 1);
	myInt.addObject("Accelerometer", BLUE, 2);
	myInt.addObject("Temperature", BLUE, 3);
	
}

void loop()
{
	// printMem();
	
	static history touchInput = {-1,-1};
	
	// Rows displayed on the small LCD is limited, so we keep track of the active number of lines on the screen as to allow refreshed to occur at the maximum
	static int rows = 0;
	
	static history mode = {-1,0}; // Setup interfaces
	static history printedTime = {0,0};
	
	// Read the touch screen and populate the p Point
	Point p = ts.getPoint();
	
	//pinMode(XP, OUTPUT);
	pinMode(XM, OUTPUT);
	pinMode(YP, OUTPUT);
	//pinMode(YM, OUTPUT);
	
	if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
		// scale from 0->1023 to tft.width
		p.x = map(p.x, TS_MINX, TS_MAXX, TFTLCD.width(), 0);
		p.y = map(p.y, TS_MINY, TS_MAXY, TFTLCD.height(), 0);
		
		if(p.y > 150)
		{
			touchInput.o = touchInput.n;
			touchInput.n = (p.x + 20)/100;
			
			#if SERIAL_PRINT_ENABLE
				#if DEBUG_INTERFACE
				Displays::print(&ST7735,"OLD: ");
				Displays::print(&ST7735, touchInput.o);
				Displays::print(&ST7735," NEW: ");
				Displays::println(&ST7735,touchInput.n);
			
				Displays::print(&ST7735,"Pressure: ");
				Displays::print(&ST7735,p.x);
				Displays::print(&ST7735,",");
				Displays::print(&ST7735,p.y);
				Displays::print(&ST7735," - ");
				Displays::println(&ST7735,touchInput.n);
				#endif
			#endif
		}
		
		if(touchInput.o != touchInput.n)
		{
			switch(touchInput.n)
			{
				case 0 :
					// Displays::println(&ST7735,"Zero");
					myInt.addObject("Compass", SELECTED_COLOR, 0);
					break;
				case 1 :
					// Displays::println(&ST7735,"One");
					myInt.addObject("GPS", SELECTED_COLOR, 1);
					break;
				case 2 :
					// Displays::println(&ST7735,"Two");
					myInt.addObject("Accelerometer", SELECTED_COLOR, 2);
					break;
				case 3 :
					// Displays::println(&ST7735,"Three");
					myInt.addObject("Temperature", SELECTED_COLOR, 3);
					break;
			}
			
			mode.n = touchInput.n;
			
			switch(touchInput.o)
			{
				case 0 :
					myInt.addObject("Compass", BLUE, 0);
					DHMC6352::reset();
					break;
				case 1 :
					myInt.addObject("GPS", BLUE, 1);
					
					#if ANTLER_LAKE
					antlerLake->resetMap();
					#else
						#if UOFA_2
						uofa2->resetMap();
						#else
						uofa->resetMap();
						#endif
					#endif
					
					break;
				case 2 :
					myInt.addObject("Accelerometer", BLUE, 2);
					DADXL345::reset();
					break;
				case 3 :
					myInt.addObject("Temperature", BLUE, 3);
					break;
			}
		}
	}
	
	if(rows >= 15)
	{
		// The ST7735 screen has a maximum size of 15 lines of data, after which we need to clear the screen and reset the cursor
		ST7735.fillScreen(BLACK);
		ST7735.setCursor(0,0);
		
		// Reset the row count
		rows = 0;
	}
	
	HMC6352::readData();
	ADXL345::readData();
	T36GZ::readData();
	
	GTPA010::readData();
	
	#if FAKE_GPS_DATA
	GTPA010::fakeData();
	#endif
	
	#if SERIAL
	HMC6352::printData();
	ADXL345::printData(); // read the x/y/z tilt
	T36GZ::printData();
	GTPA010::printData();
	#endif
	
	// GTPA010::printData();
	
	// Read data from sensors into local varialbles
	vector *ADXL345data = ADXL345::getData();
	therm *T36GZdata = T36GZ::getData();
	angle *HMC6352data = HMC6352::getData();
	gpsData *GTPA010data = GTPA010::getData();
	
	
	if(mode.o != mode.n)
	{
		// If the mode has changed, we need to refresh the data pane for fresh content
		TFTLCD.fillRect(101, 0, TFTLCD.width(), TFTLCD.height(), BACKGROUND_COLOR);
	}
	
	switch(mode.n)
	{
		case 0 :
		{
			// Compass display
			Displays::println(&ST7735, HMC6352data);
			rows++;
			
			DHMC6352::display(&TFTLCD);
			
			break;
		}
		case 1 :
		{
			// GPS Data Display
			DGTPA010::display(&TFTLCD);
			
			break;
		}
		case 2 :
		{
			// Accelormeter Data Display
			Displays::println(&ST7735, ADXL345data);
			rows++;
			
			DADXL345::display(&TFTLCD);
			
			break;
		}
		case 3 :
		{
			// Temperature Display
			
			Displays::println(&ST7735, T36GZdata);
			rows++;
			
			DT36GZ::display(&TFTLCD);
			
			break;
		}
	}
	
	// Record old mode that was selected
	mode.o = mode.n;
	
	// Record data to SD card if the time is a modulus of 5 (Once every 5 seconds) only if we hae a GPS lock
	if(GTPA010::check() && (DataLog::printedTime.n = Sensors::getTime()) && DataLog::printedTime.n%5 == 0 && DataLog::printedTime.n != DataLog::printedTime.o)
	{
		DataLog::printedTime.o = DataLog::printedTime.n;
		DataLog::printedTime.n = Sensors::getTime();
		
		/* Add the data to the packet string */
		
		// GPS Data
		data->addData(GTPA010data->lat);
		data->addData(GTPA010data->lon);
		data->addData(GTPA010data->age);
		
		// Time Data from the GPS unit
		data->addData(GTPA010data->year);
		data->addData(GTPA010data->month);
		data->addData(GTPA010data->day);
		data->addData(GTPA010data->hour);
		data->addData(GTPA010data->minute);
		data->addData(GTPA010data->second);
		data->addData(GTPA010data->hundredths);
			
		// Accelerometer Data
		data->addData(ADXL345data->x);
		data->addData(ADXL345data->y);
		data->addData(ADXL345data->z);
			
		// Temperature Data
		data->addData(T36GZdata->temp);
		data->addData(T36GZdata->voltage);
			
		// Magnetometer Data
		data->addData(HMC6352data->d);
		
		
		// Save the data to the SD card and save the return status
		int saveStatus = data->saveData();
		
		#if DEBUG_SDCARD
		// Process return status of debug of the sdcard has been enabled
		if(saveStatus == 0)
		{
			Displays::println(&ST7735, "Data saved.");
			rows++;
		}
		else if(saveStatus == 1)
		{
			Displays::printError(&ST7735, "FAILED! Data not saved!");
			rows++;
		}
		else if(saveStatus == 3)
		{
			Displays::println(&ST7735, "WRITE DISABLED: CONFIG.H");
			rows++;
		}
		#endif
	}
	
	#if MEM_DEBUG
	printMem(); // Prints the available memory out to serial if MEM_DEBUG has been enabled
	#endif
}