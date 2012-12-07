#ifndef _T36GZ_H_
#define _T36GZ_H_

#include "../Sensors.h"

typedef struct therm
{
	float voltage; // Stores the voltage recorded from the thermistor
	double temp; // Stores the calculated temperature in C
};

class T36GZ : Sensors
{
public:
	static void printData(); // Prints the data out to the Serial interface
	static void readData(); // Read the data from the sensor
	static therm* getData(); // Gives the pointer to an object outside the class
private:
	static therm data; // Stores our data object
	static const int T36GZPin = A7; // A7 is our data pin for the thermistor
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
	
	// Calculate the voltage from the analogRead, and calculate the corrasponding temperature using Adafruits provided documenation on the T36GZ
	data.voltage = (5/1024.0) * analogRead(T36GZPin);
	data.temp = (double)((data.voltage - 0.5) * 100);
}

void T36GZ::printData()
{
	#if SERIAL_PRINT_ENABLE
	// Display the data on over serial
	Serial.print(data.temp);
	Serial.print(" C ");
	Serial.print(data.voltage);
	Serial.print(" mV");
	#endif
}

#endif
