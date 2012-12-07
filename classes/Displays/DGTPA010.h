#ifndef _DGTPA010_H_
#define _DGTPA010_H_

class DGTPA010 : GTPA010 {
public:
	template <class T> static void display(T* display); // Display the interface
protected:
	static gpsData *data; // Stores the sensor data from the parent class
};

gpsData * DGTPA010::data = getData(); // Load the ini data

template <class T> void DGTPA010::display(T* display)
{
	data = getData(); // Update the data
	
	if(check())
	{
		// Preprocessor checks to determine the map to use from the settings configured in: Config.h
		#if ANTLER_LAKE
		bool bounds = antlerLake->drawCursor(display, data);
		#else
			#if UOFA_2
			bool bounds = uofa2->drawCursor(display, data);
			#else
			bool bounds = uofa->drawCursor(display, data);
			#endif
		#endif
		
			if(!bounds)
			{
				// If the coordanates from the GPS are out of the selected map bounds, tell us
				display->fillRect(101, 0, display->width(), display->height(), BACKGROUND_COLOR);
				display->setCursor(135,90);
				display->setTextSize(2);
				display->setTextColor(RED);
				display->print("Out of Map");
				display->setCursor(190,130);
				display->print("Boundry");
				Displays::resetText(display);
			}
	}
	else
	{
		// If we dont have a GPS signal, tell us
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
