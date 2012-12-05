#ifndef _STRUCTS_H_
#define _STRUCTS_H_

typedef struct vector
{
	int x;
	int y;
	int z;
	int magnitude;
	float angle;
	float rangle;
};

typedef struct gpsData
{
	long int lat;
	long int lon;
	
	// Checksum information
	unsigned long int age;
	
	// Date/Time information
	int year;
	byte month;
	byte day;
	byte hour;
	byte minute;
	byte second;
	byte hundredths;
	
};

typedef struct therm
{
	float voltage;
	double temp;
};

typedef struct Rectangle {
	uint16_t x;
	uint16_t y;
	uint16_t w;
	uint16_t h;
	
	uint16_t padding_y;
};

typedef struct history {
	int n;
	int o;
};

typedef struct coord_t {
	int x;
	int y;
};

typedef struct circle {
	int x;
	int y;
	int r;
};


// Mapping strutures
typedef struct map_coord
{
	long int north;
	long int south;
	
	long int east;
	long int west;
};

typedef struct {
  char file_name[25];
  uint16_t ncols;
  uint16_t nrows;
} lcd_image_t;

#endif
