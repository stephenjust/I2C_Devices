#ifndef _MAPDATA_H_
#define _MAPDATA_H_

#include "bmpDraw.h"

class MapData
{
public:
	MapData(long int north, long int south, long int west, long int east, char * filename, int width, int height);
	void drawMap();
	void drawMap(coord_t * tile);
	template <class T> void drawCursor(T* display, gpsData* data);
	void resetMap();
protected:
	void mapCursor(long int* lat, long int* lon);
	void remapCursor();
	void centerCursor();
private:
	map_coord tile;
	lcd_image_t map_image;
	static coord_t Dmap_position;
	bool draw;
	coord_t cursor;
	coord_t oldCursor;
	coord_t map_tile;
	
	Adafruit_TFTLCD * display;
};

coord_t MapData::Dmap_position = {1346,728};

MapData::MapData(long int north, long int south, long int west, long int east, char * filename, int width, int height)
{
	tile.north = north;
	tile.south = south;
	tile.west = west;
	tile.east = east;
	
	map_image.ncols = width;
	map_image.nrows = height;
	
	int i;
	
	for(i = 0; i <= strlen(filename)*sizeof(char); i++)
		map_image.file_name[i] = filename[i];
	
	draw = true;
	
	oldCursor.x = 0;
	oldCursor.y = 0;
}

void MapData::drawMap()
{
	if(draw)
	{
		drawMap(&Dmap_position);
		draw = false;
	}
}

void MapData::drawMap(coord_t * tile)
{
	bmpDraw(display, map_image.file_name, 105, 0, tile);
}

void MapData::resetMap()
{
	draw = true;
}

void MapData::mapCursor(long int* lat, long int* lon)
{
	cursor.x = map(*lon, tile.west, tile.east, 0, map_image.ncols);
	cursor.y = map(*lat, tile.north, tile.south, 0, map_image.nrows);
}

void MapData::remapCursor()
{
	cursor.x = map(cursor.x, map_tile.x, map_tile.x + display->width() - 105, 105, display->width());
	cursor.y = map(cursor.y, map_tile.y, map_tile.y + display->height(), 0, display->height());
}

void MapData::centerCursor()
{
	cursor.x = (display->width() - 105)/2 + 105;
	cursor.y = display->height()/2;
}

template <class T> void MapData::drawCursor(T* d, gpsData* data)
{
	display = d;
	
	mapCursor(&data->lat, &data->lon);
	
	if(cursor.x < (display->width() - 105)/2)
	{
		// #if SERIAL_PRINT_ENABLE
		Serial.print("Cursor out of bounds: ");
		Serial.print(data->lat);
		Serial.print(" => ");
		Serial.println(cursor.x);
		// #endif
		
		return;
	}
	
	if(cursor.y < display->height()/2)
	{
		// #if SERIAL_PRINT_ENABLE
		Serial.print("Cursor out of bounds: ");
		Serial.print(data->lon);
		Serial.print(" => ");
		Serial.println(cursor.y);
		// #endif
		
		return;
	}
	
	if(draw)
	{
		map_tile.x = cursor.x - (display->width() - 105)/2;
		map_tile.y = cursor.y - display->height()/2;
	
		centerCursor();
	
		drawMap(&map_tile);
	
		display->fillCircle(cursor.x, cursor.y, 3, CYAN);
		oldCursor = cursor;
		draw = false;
	}
	else
	{
		remapCursor();
		
		if(oldCursor.x != cursor.x || oldCursor.y != cursor.y)
		{
			display->fillCircle(oldCursor.x, oldCursor.y, 3, BLUE);
			display->fillCircle(cursor.x, cursor.y, 3, CYAN);
			oldCursor = cursor;
			
			if(cursor.x > display->width() - 10 || cursor.x < 105 - 10 || cursor.y > display->height() - 10 || cursor.y < 0 + 10)
			{
				draw = true;
			}
			
		}
	}
}

// MapData * antlerLake = new MapData(53.521, 53.435, -113.085, -112.938, "ANTLERLAKE.lcd", 2015, 1981);
MapData * antlerLake = new MapData(5350648, 5347778, -11300692, -11296224, "ant2.bmp", 1920, 2073);

#endif
