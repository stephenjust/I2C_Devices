#include "../Sensors.h"

class HMC6352 : Sensors
{
public:
	// HMC6352();
	static void printData();
	static void readData();
	static float* getData();
	
	static void begin();
protected:
	static byte _buff[2];
private:
	static const int HMC6352SlaveAddress = 0x21;
	static const int HMC6352ReadAddress = 0x41; //"A" in hex
	static const uint8_t howManyBytesToRead = 2;
	static float headingInt;
};

byte HMC6352::_buff[2];
float HMC6352::headingInt;

float* HMC6352::getData()
{
	return &headingInt;
}

void HMC6352::readData()
{
	readFrom( HMC6352SlaveAddress, HMC6352ReadAddress, howManyBytesToRead, _buff); //read the acceleration data from the ADXL345
	
	float headingSum = (_buff[0] << 8) + _buff[1]; //(MSB / LSB sum)
	headingInt = headingSum / 10;
}

void HMC6352::printData()
{
	Serial.print(headingInt);
	Serial.print(" degrees");
}