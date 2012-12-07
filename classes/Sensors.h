#ifndef _SENSORS_H_
#define _SENSORS_H_

#include <Arduino.h>

class Sensors {
public:
	Sensors();
	static void writeTo(int destination, byte address, byte val);
	static void readFrom(int destination, byte address, int num, byte _buff[]);
	static int getTime();
	static unsigned long int getLongTime();
	static volatile bool sensorSecond;
private:
	static unsigned long int time;
};

unsigned long int Sensors::time;

volatile bool Sensors::sensorSecond = 0;

Sensors::Sensors()
{
	Wire.begin();
	time = millis();
}

int Sensors::getTime()
{
	return (int)((float)(0.001 * (millis() - time)));
}

unsigned long int Sensors::getLongTime()
{
	return millis() - time;
}

void Sensors::writeTo(int destination, byte address, byte val) {
	Wire.beginTransmission(destination); // start transmission to device 
	Wire.write(address);             // send register address
	Wire.write(val);                 // send value to write
	Wire.endTransmission();         // end transmission
}

// Reads num bytes starting from address register on device in to _buff array
void Sensors::readFrom(int destination, byte address, int num, byte _buff[]) {
	Wire.beginTransmission(destination); // start transmission to device 
	Wire.write(address);             // sends address to read from
	Wire.endTransmission();         // end transmission
	
	delay(6);
	
	Wire.beginTransmission(destination); // start transmission to device
	Wire.requestFrom(destination, num);    // request 6 bytes from device
	
	int i = 0;
	while(Wire.available())         // device may send less than requested (abnormal)
	{ 
		_buff[i] = Wire.read();    // receive a byte
		i++;
	}
	
	Wire.endTransmission();         // end transmission
}

#endif