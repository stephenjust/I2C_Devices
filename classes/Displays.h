#include <Arduino.h>
#include <Adafruit_GFX.h>

typedef struct Rectangle {
	uint16_t x;
	uint16_t y;
	uint16_t w;
	uint16_t h;
	
	uint16_t padding_y;
};

class Displays
{
public:
	template <class T, class D> static void printError(T* display, D data)
	{
		display->setTextColor(WHITE,RED);
		print(display, data);
		resetText(display);
	}
	template <class T> static void print(T* d, vector* vect)
	{
		#if ALL_LABELS
		print(d, "Vector ");
		#endif
		
		print(d, "x: ");
		print(d, vect->x);
		print(d, " y: ");
		print(d, vect->y);
		print(d, " z: ");
		print(d, vect->z);
	}
	template <class T> static void print(T* display, therm* temp)
	{
		#if ALL_LABELS
		print(display, "Temp: ");
		#endif
		
		print(display, temp->temp);
		print(display, " C ");
		print(display, temp->voltage);
		print(display, " V");
	}
	template <class T> static void print(T* display, angle* an)
	{
		#if ALL_LABELS
		print(display, "Angle: ");
		#endif
		
		print(display, an->d);
		print(display, " Deg ");
		print(display, an->r);
		print(display, " Rad");
	}
	template <class T, class D> static void print(T* display, D data)
	{
		display->print(data);
	}
	template <class T, class D> static void println(T* display, D data)
	{
		print(display, data);
		display->println("");
	}
	template <class T> static void resetText(T* display)
	{
		display->setTextSize(1);
		display->setTextColor(WHITE);
	}
	// template <class T, class D> static void sprintln(T* displa)
};

// INTERFACE CLASSES
class Interface : Displays
{
public:
	Interface();
	template <class T> void addObject(T* object);
	void addObject(const char * title, uint16_t color, int index);
	void updateBounds();
	void setDisplay(Adafruit_GFX * d, uint16_t color)
	{
		display = d;
		
		pos.x = 0;
		pos.y = 0;
		pos.w = 100;
		pos.h = 50;
		
		pos.padding_y = 10;
		
		display->fillScreen(color);
	}
private:
	int index;
	Adafruit_GFX * display;
	Rectangle pos;
};

Interface::Interface()
{

}

template <class T> void Interface::addObject(T* object)
{
	display->setCursor(pos.x,pos.y);
	println(display, "Wrong type.");
}

void Interface::addObject(const char * title, uint16_t color, int index)
{
	#if SERIAL_PRINT_ENABLE
		#if DEBUG_INTERFACE
			Serial.print("Pane object printing... ");
			Serial.print(color, HEX);
			Serial.print(" @");
			Serial.print(pos.x);
			Serial.print(",");
			Serial.println(pos.y);
		#endif
	#endif
			
	display->setTextColor(WHITE);
	display->fillRect(pos.x, pos.h*index + ((index != 0) ? pos.padding_y*index : 0) + 5 , pos.w, pos.h, color);
	display->setCursor(pos.x + pos.padding_y,pos.h*index + (int)pos.h/2 + pos.padding_y*index);
	println(display, title);
}