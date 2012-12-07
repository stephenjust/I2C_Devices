#ifndef _ADXL345_H_
#define _ADXL345_H_

#include "../Sensors.h"

typedef struct vector
{
	int x; // X pos
	int y; // Y pos
	int z; // Y pos
	int magnitude; // Mag of the vector
	float angle; // Angle in degrees
	float rangle; // Angle in radians
};

class ADXL345 : Sensors
{
public:
	static void printData(); // Designed to print the data onto serial
	static void readData(); // Executes the reading procedures from the sensor
	static vector* getData(); // Returns with a address to the data
	
	static void begin(); // Initilization routines for the sensor
	static bool check(); // A validity check for the sensor, if true, its reporting a valid response, if false, then response is invalid
protected:
	static byte _buff[6]; // Byte array storing the data buffer from the wire librairy accessed via the parent class
private:
	static const int ADXL345SlaveAddress = 0x1D; // The wire address to the sensor
	static const char POWER_CTL = 0x2D;	//Power Control Register
	static const int POWER_CTL_VALUE = 0x08; // Measurement mode for the device
	static const char DATA_FORMAT = 0x31; // Data format address
	static const int DATA_FORMAT_VALUE = 0x01; // Put the ADXL345 into +/- 4G range with 0x01 to the DATA_FORMAT register.
	static const char DATAX0 = 0x32; //X-Axis Data 0 address
	static const char DATAX1 = 0x33; //X-Axis Data 1 address
	static const char DATAY0 = 0x34; //Y-Axis Data 0 address
	static const char DATAY1 = 0x35; //Y-Axis Data 1 address
	static const char DATAZ0 = 0x36; //Z-Axis Data 0 address
	static const char DATAZ1 = 0x37; //Z-Axis Data 1 address
	static const uint8_t howManyBytesToRead = 6; // The amount of bytes to read from the serial interface before continuing
	static vector data; // Our struct to store our data inside.
	static const int maxAccel = 30; // A value of maxAccel would indicate a tilt too great.
	
	static const int centerZ = 137;
};

byte ADXL345::_buff[6];
vector ADXL345::data;

vector* ADXL345::getData()
{
	// Returns data address
	return &data;
}

bool ADXL345::check()
{
	// Checks to see if the x/y values ar beyond their maximum tolerances, if so return false, else, return true.
	if(data.x > maxAccel || data.x < -maxAccel || data.y > maxAccel || data.y < -maxAccel || data.z < 0)
	{
		return false;
	}
	
	return true;
}

void ADXL345::begin()
{
	writeTo(ADXL345SlaveAddress, DATA_FORMAT, DATA_FORMAT_VALUE); // Put the ADXL345 into +/- 4G range by writing the value 0x01 to the DATA_FORMAT register.
	writeTo(ADXL345SlaveAddress, POWER_CTL, POWER_CTL_VALUE); // Put the ADXL345 into Measurement Mode by writing 0x08 to the POWER_CTL register.
}

void ADXL345::readData()
{
    readFrom( ADXL345SlaveAddress, DATAX0, howManyBytesToRead, _buff); //read the acceleration data from the ADXL345
	
	// each axis reading comes in 10 bit resolution, ie 2 bytes.  Least Significat Byte first!! thus we are converting both bytes in to one int
    data.x = (((int)_buff[1]) << 8) | _buff[0];   
    data.y = (((int)_buff[3]) << 8) | _buff[2];
    data.z = (((int)_buff[5]) << 8) | _buff[4];
	
	// ~137 center
	data.magnitude = abs(data.z - centerZ);
		
	// Calculate the angle relative to the devices orientation
	data.rangle = atan2(data.x, data.y);
	data.angle = data.rangle * 180/3.14159; // Convert into degrees
	
}

void ADXL345::printData()
{
	// Print the recorded data over serial if enabled
	Serial.print(" x: ");
	Serial.print( data.x );
	Serial.print(" y: ");
	Serial.print( data.y );
	Serial.print(" z: ");
	Serial.println( data.z );
}

#endif
