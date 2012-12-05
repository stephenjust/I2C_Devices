#ifndef _DGTPA010_H_
#define _DGTPA010_H_

class DGTPA010 : GTPA010 {
public:
	template <class T> static void display(T* display);
protected:
	static gpsData *data;
};

gpsData * DGTPA010::data = getData();

template <class T> void DGTPA010::display(T* display)
{
	data = getData();
	
	if(check())
		antlerLake->drawCursor(display, data);
	else
	{
		display->setCursor(135,90);
		display->setTextSize(2);
		display->setTextColor(RED);
		display->print("Acquiring GPS");
		display->setCursor(190,130);
		display->print("Fix");
		Displays::resetText(display);
	}
}

#endif
