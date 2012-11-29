#include "../Sensors.h"

typedef struct vector
{
	int x;
	int y;
	int z;
};

class ADXL345 : Sensors
{
public:
	// ADXL345();
	static void printData();
	static void readData();
	static vector* getData();
	
	static void begin();
protected:
	static byte _buff[6];
private:
	static const int ADXL345SlaveAddress = 0x1D;
	static const char POWER_CTL = 0x2D;	//Power Control Register
	static const char DATA_FORMAT = 0x31;
	static const char DATAX0 = 0x32;	//X-Axis Data 0
	static const char DATAX1 = 0x33;	//X-Axis Data 1
	static const char DATAY0 = 0x34;	//Y-Axis Data 0
	static const char DATAY1 = 0x35;	//Y-Axis Data 1
	static const char DATAZ0 = 0x36;	//Z-Axis Data 0
	static const char DATAZ1 = 0x37;	//Z-Axis Data 1
	static const uint8_t howManyBytesToRead = 6;
	static vector data;
};

byte ADXL345::_buff[6];
vector ADXL345::data;

vector* ADXL345::getData()
{
	return &data;
}

void ADXL345::begin()
{
	//Put the ADXL345 into +/- 4G range by writing the value 0x01 to the DATA_FORMAT register.
	writeTo(ADXL345SlaveAddress, DATA_FORMAT, 0x01);
	//Put the ADXL345 into Measurement Mode by writing 0x08 to the POWER_CTL register.
	writeTo(ADXL345SlaveAddress, POWER_CTL, 0x08);
}

void ADXL345::readData()
{
    readFrom( ADXL345SlaveAddress, DATAX0, howManyBytesToRead, _buff); //read the acceleration data from the ADXL345

    // each axis reading comes in 10 bit resolution, ie 2 bytes.  Least Significat Byte first!!
    // thus we are converting both bytes in to one int
    data.x = (((int)_buff[1]) << 8) | _buff[0];   
    data.y = (((int)_buff[3]) << 8) | _buff[2];
    data.z = (((int)_buff[5]) << 8) | _buff[4];
}



void ADXL345::printData()
{
  
  Serial.print(" x: ");
  Serial.print( data.x );
  Serial.print(" y: ");
  Serial.print( data.y );
  Serial.print(" z: ");
  Serial.println( data.z );
}