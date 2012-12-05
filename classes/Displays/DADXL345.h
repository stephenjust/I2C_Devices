#ifndef _DADXL345_H_
#define _DADXL345_H_

class DADXL345 : ADXL345 {
public:
	template <class T> static void display(T* display);
	static void reset();
private:
	static vector * data;
	static const coord_t position;
	static vector printed;
	static const int sensistivity = 30;
	static circle largeCircle;
	static circle dataIndicator;
	static circle oldCompass;
};

const coord_t DADXL345::position = {125,30};

vector * DADXL345::data = getData();

vector DADXL345::printed = {0,0};

circle DADXL345::largeCircle;
circle DADXL345::dataIndicator;
circle DADXL345::oldCompass;

template <class T> void DADXL345::display(T* display)
{
	data = getData();
	
	if(sensorSecond)
	{
		sensorSecond = 0;
		
		largeCircle.r = 63;
		dataIndicator.r = 5;
		largeCircle.x = 210;
		largeCircle.y = 125;
			
		display->drawCircle(largeCircle.x, largeCircle.y, largeCircle.r, WHITE);
			
		int mapped_magnitude = map(data->magnitude, 0, sensistivity, 0, largeCircle.r);
		
		dataIndicator.x = cos(-data->rangle) * (mapped_magnitude + dataIndicator.r + 2);
		dataIndicator.y = sin(data->rangle) * (mapped_magnitude + dataIndicator.r + 2);
				
		if(oldCompass.x != dataIndicator.x)
		{
			display->fillCircle(oldCompass.x + largeCircle.x, oldCompass.y + largeCircle.y, oldCompass.r,BACKGROUND_COLOR);
			display->fillCircle(dataIndicator.x + largeCircle.x, dataIndicator.y + largeCircle.y, dataIndicator.r, YELLOW);
					
			oldCompass = dataIndicator;
		}
	}
}

void DADXL345::reset()
{
	printed.x = 0;
	printed.y = 0;
}

#endif
