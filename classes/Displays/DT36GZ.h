#ifndef _DT36GZ_H_
#define _DT36GZ_H_

class DT36GZ : T36GZ {
public:
	template <class T> static void display(T* display);
private:
	static float oldTemp;
	static float newTemp;
	static therm* data;
	static const coord_t cursorPos;
	static const int textSize = 4;
};

float DT36GZ::oldTemp = 0.0;
float DT36GZ::newTemp = 0.0;
therm* DT36GZ::data = getData();

const coord_t DT36GZ::cursorPos = {125, 105};

template <class T> void DT36GZ::display(T* display)
{
	newTemp = data->temp;
			
	if(sensorSecond)
	{
		sensorSecond = 0;
				
		if(oldTemp != newTemp)
		{
			display->setCursor(cursorPos.x, cursorPos.y);
			display->setTextSize(textSize);
			display->setTextColor(BACKGROUND_COLOR);
			Displays::print(display, oldTemp);
			Displays::print(display, " C");
			Displays::resetText(display);
				
			oldTemp = newTemp;
		}
			
		display->setCursor(cursorPos.x, cursorPos.y);
		display->setTextSize(textSize);
		Displays::print(display, data->temp);
		Displays::print(display, " C");
		Displays::resetText(display);
	}
}

#endif
