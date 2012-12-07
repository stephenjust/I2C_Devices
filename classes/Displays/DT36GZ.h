#ifndef _DT36GZ_H_
#define _DT36GZ_H_

class DT36GZ : T36GZ {
public:
	template <class T> static void display(T* display); // Display the interface
private:
	static float oldTemp; // The old temperature for comparison purposes
	static float newTemp; // The data object holding out recorded data
	static therm* data; // The data object used to collect the data/
	static const coord_t cursorPos; // The position of the cursor ini
	static const int textSize = 4; // The size of the text
};

float DT36GZ::oldTemp = -1.0; // Store the old as 0
float DT36GZ::newTemp = 0.0; // Store the new as 0
therm* DT36GZ::data = getData(); // Get the data pointer

const coord_t DT36GZ::cursorPos = {125, 105}; // Define the position on the TFTLCD screen for where we want to display our interface

template <class T> void DT36GZ::display(T* display)
{
	newTemp = data->temp; // update the data
			
	if(sensorSecond) // Update every second
	{
		sensorSecond = 0; // Reset the display parameter
				
		if(oldTemp != newTemp) // If the value has changed, update the printed value
		{
			display->setCursor(cursorPos.x, cursorPos.y);
			display->setTextSize(textSize);
			display->setTextColor(BACKGROUND_COLOR);
			Displays::print(display, oldTemp);
			Displays::print(display, " C");
			Displays::resetText(display);
				
			oldTemp = newTemp; // Store the new value into the old variable
			
			display->setCursor(cursorPos.x, cursorPos.y);
			display->setTextSize(textSize);
			Displays::print(display, data->temp);
			Displays::print(display, " C");
			Displays::resetText(display);
		}
	}
}

#endif
