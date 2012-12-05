#ifndef _GTPA010_H_
#define _GTPA010_H_

#include "../Sensors.h"

class GTPA010 : Sensors
{
public:
	static void printData();
	static void readData();
	static gpsData* getData();
	
	static void fakeData();
	
	static void begin();
	static bool check();
	
	static void gpsCheck();
private:
	static gpsData data;
	static bool newData;
	static const long int timer_ticks = 1000000;

	static volatile bool gpsValue;
	static volatile bool gpsLock;
};

gpsData GTPA010::data;
bool GTPA010::newData = 0;

volatile bool GTPA010::gpsValue = -1;
volatile bool GTPA010::gpsLock = 0;

gpsData* GTPA010::getData()
{
	return &data;
}

bool GTPA010::check()
{
	return newData;
}

void GTPA010::begin()
{
	// Start GPS - Defines in classes/Config.h
	Serial2.begin(GPS_BAUD_RATE);
	pinMode(GPS_FIX_PIN,INPUT);
	pinMode(GPS_ENABLE_PIN,OUTPUT);
	
	// Setup GPS Interrupt check - Defines in classes/Config.h
	Timer3.initialize();
	digitalWrite(GPS_ENABLE_PIN,HIGH);
	Timer3.attachInterrupt(&gpsCheck, timer_ticks);
}

void GTPA010::gpsCheck()
{
	bool oldGpsValue = gpsValue;
	
	gpsValue = digitalRead(GPS_FIX_PIN);
	
	if(!(oldGpsValue ^ gpsValue))
	{
		gpsLock = 1;
	}
	else
	{
		gpsLock = 0;
	}
	
	sensorSecond = !sensorSecond;
}

void GTPA010::readData()
{
	if(gpsLock)
	{
		// For one second we parse GPS data and report some key values
		// for (unsigned long start = millis(); millis() - start < 1000;)
		// {
			while (Serial2.available())
			{
				char c = Serial2.read();
				
				if (gps.encode(c)) // Did a new valid sentence come in?
					newData = true;
			}
		// }
		
		gps.get_position(&data.lat, &data.lon, &data.age);
		gps.crack_datetime(&data.year, &data.month, &data.day, &data.hour, &data.minute, &data.second, &data.hundredths, &data.age);
		
		if(data.lat == TinyGPS::GPS_INVALID_ANGLE || data.lon == TinyGPS::GPS_INVALID_ANGLE || gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES || gps.hdop() == TinyGPS::GPS_INVALID_HDOP)
		{
			newData = false;
			
			#if SERIAL_PRINT_ENABLE
			Serial.println("GPS DATA INVALID");
			#endif
		}
			
	}
}

void GTPA010::fakeData()
{
	data.lat = 5348435 + Sensors::getTime();
	data.lon = -11298005 + Sensors::getTime();
	newData = true;
}

void GTPA010::printData()
{
	Serial.print("LAT=");
		Serial.print(data.lat == TinyGPS::GPS_INVALID_ANGLE ? NAN : data.lat);
	Serial.print(" LON=");
		Serial.print(data.lon == TinyGPS::GPS_INVALID_ANGLE ? NAN : data.lon);
	Serial.print(" SAT=");
		Serial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? NAN : gps.satellites());
	Serial.print(" PREC=");
		Serial.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? NAN : gps.hdop());
	  
	Serial.print(" DATE=");
	  
	if (data.age == TinyGPS::GPS_INVALID_AGE)
		Serial.println("*******    *******    ");
	else
	{
		char sz[32];
		sprintf(sz, "%02d,%02d,%02d,%02d:%02d:%02d", data.month, data.day, data.year, data.hour, data.minute, data.second);
		Serial.println(sz);
	}
}

#endif