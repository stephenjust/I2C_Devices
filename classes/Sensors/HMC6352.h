#ifndef _HMC6352_H_
#define _HMC6352_H_

#include "../Sensors.h"

typedef struct angle
{
	float r;
	float d;
};

class HMC6352 : Sensors
{
public:
	// HMC6352();
	static void printData();
	static void readData();
	static angle* getData();
	
	static void begin();
protected:
	static byte _buff[2];
private:
	static const int HMC6352SlaveAddress = 0x21;
	static const int HMC6352ReadAddress = 0x41; //"A" in hex
	static const uint8_t howManyBytesToRead = 2;
	static angle heading;
};

byte HMC6352::_buff[2];
angle HMC6352::heading;

angle* HMC6352::getData()
{
	return &heading;
}

void HMC6352::readData()
{
	if(!ADXL345::check())
	{
		heading.d = NAN;
		heading.r = NAN;
		return;
	}
	
	readFrom( HMC6352SlaveAddress, HMC6352ReadAddress, howManyBytesToRead, _buff); //read the acceleration data from the ADXL345
	
	heading.d = (_buff[0] << 8) + _buff[1]; //(MSB / LSB sum)
	heading.d /= 10;
	heading.r = heading.d * 1000/57296;
}

void HMC6352::printData()
{
	#if SERIAL_PRINT_ENABLE
	Serial.print(heading);
	Serial.print(" degrees");
	#endif
}

#endif
