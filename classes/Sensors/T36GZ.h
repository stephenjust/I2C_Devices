#ifndef _T36GZ_H_
#define _T36GZ_H_

#include "../Sensors.h"

typedef struct therm
{
	float voltage;
	double temp;
};

class T36GZ : Sensors
{
public:
	static void printData();
	static void readData();
	static therm* getData();
	
	static void begin();
private:
	static therm data;
	static const int T36GZPin = A7; // A7
	
};

therm T36GZ::data = {0.0,0.0};

therm* T36GZ::getData()
{
	return &data;
}

void T36GZ::readData()
{
	// Prevents excess measurements, only capture every 5 seconds
	if(data.voltage != 0.0 && Sensors::getTime()%5 != 0)
		return;
	
	int reading = analogRead(T36GZPin);
	data.voltage = (5/1024.0) * reading;
	data.temp = (double)((data.voltage - 0.5) * 100);
}

void T36GZ::printData()
{
	#if SERIAL_PRINT_ENABLE
	Serial.print(data.temp);
	Serial.print(" C ");
	Serial.print(data.voltage);
	Serial.print(" mV");
	#endif
}

#endif
