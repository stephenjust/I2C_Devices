#ifndef _DADXL345_H_
#define _DADXL345_H_

class DADXL345 : ADXL345 {
public:
	template <class T> static void display(T* display); // Used to display the interface
	static void reset(); // Used to reset the interface
private:
	static vector * data; // Stores the sensor data
	static const int sensistivity = 30; // The range for which the accelrometer reports a destabilization has occured
	static const circle largeCircle; // Stores the larger circle for the interface
	static circle dataIndicator; // Stores the data for the compass being displayed
	static circle oldCompass;// Stores the old data readings, used for comparison when new ones are taken to determine if an update is requried
	static int mapped_magnitude; // Stores the mapped magnitude of the interface
};

vector * DADXL345::data = getData(); // Initial loading from a reading done against the parent class
const circle DADXL345::largeCircle = {210, 125, 63}; // The initial posision and radius of the larger compass circle

// Static declarations
circle DADXL345::dataIndicator;
circle DADXL345::oldCompass;
int DADXL345::mapped_magnitude;

template <class T> void DADXL345::display(T* display)
{
	data = getData(); // Refresh the data
	
	if(sensorSecond) // Check if it was displayed already this second
	{
		sensorSecond = 0; // Reset the variable
		dataIndicator.r = 5; // Define the radius of the data circle
		
		mapped_magnitude = map(data->magnitude, 0, sensistivity, 0, largeCircle.r);
		
		// Compute the location of the angle on the circle
		dataIndicator.x = cos(-data->rangle) * (mapped_magnitude + dataIndicator.r + 2);
		dataIndicator.y = sin(data->rangle) * (mapped_magnitude + dataIndicator.r + 2);
		
		if(oldCompass.x != dataIndicator.x)
		{
			display->drawCircle(largeCircle.x, largeCircle.y, largeCircle.r, WHITE); // Draw the large outer circle
			display->fillCircle(oldCompass.x + largeCircle.x, oldCompass.y + largeCircle.y, oldCompass.r,BACKGROUND_COLOR); // Erase old circle
			display->fillCircle(dataIndicator.x + largeCircle.x, dataIndicator.y + largeCircle.y, dataIndicator.r, YELLOW); // Draw new circle
			
			oldCompass = dataIndicator; // Update oldCompass
		}
	}
}

void DADXL345::reset()
{
	oldCompass.x = dataIndicator.x + 1; // Resets the old variable to a non equal state
}

#endif
