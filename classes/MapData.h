#ifndef _MAPDATA_H_
#define _MAPDATA_H_

// Structure used to store a set of ordered coordanites
typedef struct coord_t {
	int x;
	int y;
};

// Structure used to store map related coordinate information
typedef struct map_coord
{
	long int north;
	long int south;
	
	long int east;
	long int west;
};

// Structure used to store information related to a BMP image
typedef struct lcd_image_t {
	char file_name[25];
	uint16_t ncols;
	uint16_t nrows;
};

class MapData
{
public:
	MapData(long int north, long int south, long int west, long int east, char * filename, int width, int height); // Adds a map object
	void drawMap(); // Draws a map image as defined by its properties
	void drawMap(coord_t * tile); // An overloaded function for drawing a specific tile of the map
	bool drawCursor(Adafruit_TFTLCD *display, gpsData* data); // Draws a cursor/map image onto the screen passed using provided gps info
	void resetMap(); // Resets the map drawing parameter
protected:
	void mapCursor(long int* lat, long int* lon); // Maps a given cursor with gps info provided onto a pixel map of the image
	void remapCursor(); // Remaps the cursor from its pixel map to a location on the moving local map tile
	void centerCursor(); // Centers the display on the display pane
	void bmpDraw(int x, int y, coord_t * source); // Draws a bmp image as stored in the objects variables
	uint32_t read32(File f); // Reads 32 bytes of data
	uint16_t read16(File f); // Reads 16 bytes of data
private:
	map_coord tile; // Defines a map tile for display
	lcd_image_t map_image; // Defines the storage element for the max image
	bool draw; // Stores whether the image should be printed or not
	coord_t cursor; // Holds the current cursors position
	coord_t oldCursor; // Holds the old cursors position
	coord_t map_tile; // Holds the local map tile for display
	
	Adafruit_TFTLCD * display; // Holds the image that we are displaying locally
	
	// To print the maps on a offset from the left (to leave room for interfaces), add an offset in px
	static const int screenoffset = 105;
};

MapData::MapData(long int north, long int south, long int west, long int east, char * filename, int width, int height)
{
	// Translates the input into the storage structure
	tile.north = north;
	tile.south = south;
	tile.west = west;
	tile.east = east;
	
	// Save the map information for mapping
	map_image.ncols = width;
	map_image.nrows = height;
	
	// Store the filename into the local structure
	for(int i = 0; i <= strlen(filename)*sizeof(char); i++)
		map_image.file_name[i] = filename[i];
	
	// Indicates that the drawing should be conducted (upon ini, the map has not been printed, by definition, we need to tell it that it still needs to be printed)
	draw = true;
	
	// Generate some old variables to ensure that the cursor is printed
	oldCursor.x = -1;
	oldCursor.y = -1;
}

void MapData::drawMap(coord_t * tile)
{
	bmpDraw(screenoffset, 0, tile); // Draws the selected bmp image
}

void MapData::resetMap()
{
	draw = true; // Resets the printing mode
}

void MapData::mapCursor(long int* lat, long int* lon)
{
	if(*lon < tile.west || *lon > tile.east) // If out of bounds
	{
		cursor.x = -1;
		return;
	}
	
	cursor.x = map(*lon, tile.west, tile.east, 0, map_image.ncols); // Map the cursor
	
	if(*lat > tile.north || *lat < tile.south) // If out of bounds
	{
		cursor.y = -1;
		return;
	}
	
	cursor.y = map(*lat, tile.north, tile.south, 0, map_image.nrows); // Map the cursor
}

void MapData::remapCursor()
{
	// Remaps the cursor for moving printing on the local map tile
	cursor.x = map(cursor.x, map_tile.x, map_tile.x + display->width() - screenoffset, screenoffset, display->width());
	cursor.y = map(cursor.y, map_tile.y, map_tile.y + display->height(), 0, display->height());
}

void MapData::centerCursor()
{
	// Center the cursor
	cursor.x = (display->width() - screenoffset)/2 + screenoffset;
	cursor.y = display->height()/2;
}

bool MapData::drawCursor(Adafruit_TFTLCD* d, gpsData* data)
{
	display = d; // Reserves the display object
	
	mapCursor(&data->lat, &data->lon); // Map the cursor for the map tile to print
	
	if(cursor.x < (display->width() - screenoffset)/2) // If out of bounds...
	{
		#if SERIAL_PRINT_ENABLE
		Serial.print("Cursor out of bounds: ");
		Serial.print(data->lat);
		Serial.print(" => ");
		Serial.println(cursor.x);
		#endif
		
		return false;
	}
	
	if(cursor.y < display->height()/2) // If out of bounds...
	{
		#if SERIAL_PRINT_ENABLE
		Serial.print("Cursor out of bounds: ");
		Serial.print(data->lon);
		Serial.print(" => ");
		Serial.println(cursor.y);
		#endif
		
		return false;
	}
	
	if(draw) // If we havent drawn the map yet or need to redraw....
	{
		map_tile.x = cursor.x - (display->width() - screenoffset)/2;
		map_tile.y = cursor.y - display->height()/2;
	
		centerCursor();
	
		drawMap(&map_tile);
	
		display->fillCircle(cursor.x, cursor.y, 3, CYAN);
		oldCursor = cursor;
		draw = false;
	}
	else // No? Then just draw a cursor
	{
		remapCursor();
		
		if(oldCursor.x != cursor.x || oldCursor.y != cursor.y) // Only if its changed...
		{
			if(oldCursor.x > 0 && oldCursor.y > 0) // And its actually a valid location to print...
				display->fillCircle(oldCursor.x, oldCursor.y, 3, BLUE); // Fill the old one with blue...
			
			display->fillCircle(cursor.x, cursor.y, 3, CYAN); // The new one with CYAN!
			oldCursor = cursor; // And don't forget our location, we'll need to change its color
			
			if(cursor.x > display->width() - 10 || cursor.x < 105 + 10 || cursor.y > display->height() - 10 || cursor.y < 0 + 10) // But if its too closeto the edge...
			{
				draw = true; // Then move the map to compensate
			}
			
		}
	}
	
	return true; // And let the caller know that it all worked out
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.
uint16_t MapData::read16(File f)
{
	uint16_t result;
	((uint8_t *)&result)[0] = f.read(); // LSB
	((uint8_t *)&result)[1] = f.read(); // MSB
	return result;
}

uint32_t MapData::read32(File f)
{
	uint32_t result;
	((uint8_t *)&result)[0] = f.read(); // LSB
	((uint8_t *)&result)[1] = f.read();
	((uint8_t *)&result)[2] = f.read();
	((uint8_t *)&result)[3] = f.read(); // MSB
	return result;
}

void MapData::bmpDraw(int x, int y, coord_t * source) {

	File bmpFile;
	int bmpWidth, bmpHeight; // W+H in pixels
	uint8_t	bmpDepth; // Bit depth (currently must be 24)
	uint32_t bmpImageoffset; // Start of image data in file
	uint32_t rowSize; // Not always = bmpWidth; may have padding
	uint8_t	sdbuffer[3*BUFFPIXEL]; // pixel in buffer (R+G+B per pixel)
	uint16_t lcdbuffer[BUFFPIXEL]; // pixel out buffer (16-bit per pixel)
	uint8_t	buffidx = sizeof(sdbuffer); // Current position in sdbuffer
	boolean	goodBmp = false; // Set to true on valid header parse
	boolean	flip = true; // BMP is stored bottom-to-top
	int w, h, row, col;
	uint8_t	r, g, b;
	uint32_t pos = 0;
		
	#if SERIAL_PRINT_ENABLE
	uint32_t startTime = millis();
	#endif
	
	uint8_t	lcdidx = 0;
	boolean	first = true;

	if((x >= display->width()) || (y >= display->height())) return;
	#if SERIAL_PRINT_ENABLE
	Serial.println();
	Serial.print("Loading image '");
	Serial.print(map_image.file_name);
	Serial.println('\'');
	#endif
	// Open requested file on SD card
	if ((bmpFile = SD.open(map_image.file_name)) == NULL) {
	#if SERIAL_PRINT_ENABLE
		Serial.print("File not found");
	#endif
		return;
	}

	// Parse BMP header
	if(read16(bmpFile) == 0x4D42) { // BMP signature
		// Serial.print("File size: "); Serial.println(read32(bmpFile));
	(void)read32(bmpFile);
		(void)read32(bmpFile); // Read & ignore creator bytes
		bmpImageoffset = read32(bmpFile); // Start of image data
		// Serial.print("Image Offset: "); Serial.println(bmpImageoffset, DEC);
		// Read DIB header
		// Serial.print("Header size: "); Serial.println(read32(bmpFile));
	(void)read32(bmpFile);
		bmpWidth	= read32(bmpFile);
		bmpHeight = read32(bmpFile);
		if(read16(bmpFile) == 1) { // # planes -- must be '1'
			bmpDepth = read16(bmpFile); // bits per pixel
			// Serial.print("Bit Depth: "); Serial.println(bmpDepth);
			if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

				goodBmp = true; // Supported BMP format -- proceed!
				// Serial.print("Image size: ");
				// Serial.print(bmpWidth);
				// Serial.print('x');
				// Serial.println(bmpHeight);

				// BMP rows are padded (if needed) to 4-byte boundary
				rowSize = (bmpWidth * 3 + 3) & ~3;

				// If bmpHeight is negative, image is in top-down order.
				// This is not canon but has been observed in the wild.
				if(bmpHeight < 0) {
					bmpHeight = -bmpHeight;
					flip			= false;
				}

				// Crop area to be loaded
				w = bmpWidth;
				h = bmpHeight;
				if((x+w-1) >= display->width())	w = display->width()	- x;
				if((y+h-1) >= display->height()) h = display->height() - y;
		
		

				// Set TFT address window to clipped image bounds
				display->setAddrWindow(x, y, x+w-1, y+h-1);

				for (row=0; row<h; row++) { // For each scanline...
					// Seek to start of scan line.	It might seem labor-
					// intensive to be doing this on every line, but this
					// method covers a lot of gritty details like cropping
					// and scanline padding.	Also, the seek only takes
					// place if the file position actually needs to change
					// (avoids a lot of cluster math in SD library).
					// Seek to start of pixels to read from, need 32 bit arith for big images
					pos = bmpImageoffset + (( bmpHeight - 1 - (uint32_t) source->y - (uint32_t) row) * (uint32_t) rowSize ) + (uint32_t) source->x * 3;
					if(bmpFile.position() != pos) { // Need seek?
						bmpFile.seek(pos);
						buffidx = sizeof(sdbuffer); // Force buffer reload
					}

					for (col=0; col<w; col++) { // For each column...
						// Time to read more pixel data?
						if (buffidx >= sizeof(sdbuffer)) { // Indeed
							// Push LCD buffer to the display first
							if(lcdidx > 0) {
								display->pushColors(lcdbuffer, lcdidx, first);
								lcdidx = 0;
								first	= false;
							}
							bmpFile.read(sdbuffer, sizeof(sdbuffer));
							buffidx = 0; // Set index to beginning
						}

						// Convert pixel from BMP to TFT format
						b = sdbuffer[buffidx++];
						g = sdbuffer[buffidx++];
						r = sdbuffer[buffidx++];
						lcdbuffer[lcdidx++] = display->color565(r,g,b);
					} // end pixel
				} // end scanline
				// Write any remaining data to LCD
				if(lcdidx > 0) {
					display->pushColors(lcdbuffer, lcdidx, first);
				} 
				// Serial.print("Loaded in ");
				// Serial.print(millis() - startTime);
				// Serial.println(" ms");
			}
		}
	}

	bmpFile.close();
	// if(!goodBmp) Serial.println("BMP format not recognized.");
}

// Declare the loaded maps
MapData * antlerLake = new MapData(5350648, 5347778, -11300692, -11296224, "ant2.bmp", 1920, 2073);
MapData * uofa = new MapData(5353106, 5352296, -11353317, -11351506, "uofa.bmp", 2286, 1720);

#endif
