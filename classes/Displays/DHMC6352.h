#ifndef _DHMC6352_H_
#define _DHMC6352_H_

class DHMC6352 : HMC6352 {
public:
	template <class T> static void display(T* display); // Display the interface
	static void reset(); // Reset the interface
protected:
	static const circle bigCompass; // Our large circle for which the indicator is displayed on the outside
	static circle smlCompass; // Our compass heading
	static circle oldCompass; // Our previous compass heading
	static angle* HMC6352data; // The data for the compass heading
	static bool lcPrint; // The large circles history parameter
	static bool warnPrint; // The warning history parameter
};

const circle DHMC6352::bigCompass = {210, 125, 63}; // Declare our large circle such that it displays nicely on the TFTLCD
angle* DHMC6352::HMC6352data = getData(); // Load initial data

// Static declerations/allocations
circle DHMC6352::smlCompass;
circle DHMC6352::oldCompass;
bool DHMC6352::lcPrint = true;
bool DHMC6352::warnPrint = false;

template <class T> void DHMC6352::display(T* display)
{
	if(!ADXL345::check()) // Check to see if the accel is centered, if not, tell us
	{
		digitalWrite(GYRO_WARNING_LED, LOW);
		if(warnPrint == false)
		{
			display->setCursor(135,10);
			display->setTextSize(2);
			display->setTextColor(RED);
			display->print("Please level");
			display->setCursor(175,25);
			display->print("device");
			Displays::resetText(display);
			
		}
		digitalWrite(GYRO_WARNING_LED, HIGH);
		
		warnPrint = true;
		
		return;
	}
	else if(warnPrint == true) // If the warning was posted, then clear it, we would have returned already if we were still in error
	{
		display->setCursor(135,10);
		display->setTextSize(2);
		display->setTextColor(BACKGROUND_COLOR);
		display->print("Please level");
		display->setCursor(175,25);
		display->print("device");
		Displays::resetText(display);
		digitalWrite(GYRO_WARNING_LED, LOW);
	}
	
	
	if(sensorSecond) // Activated once a second
	{
		sensorSecond = 0; // Reset the parameter
		
		smlCompass.r = 5; // Set the radius of the small indicator
		
		display->setCursor(bigCompass.y, bigCompass.x); // Print the old over with the background color
		display->setTextColor(BACKGROUND_COLOR,BACKGROUND_COLOR);
		display->print(HMC6352data->d);
		display->print(" degrees");
		
		#if SERIAL_PRINT_ENABLE
		Serial.print("OLD: "); Serial.print(HMC6352data->d); Serial.println(" degrees ");
		#endif
			
		HMC6352data = getData(); // Update the data
		
		display->setCursor(bigCompass.y, bigCompass.x); // Print the new data
		display->setTextColor(WHITE,BACKGROUND_COLOR);
		display->print(HMC6352data->d);
		display->print(" degrees");
		
		#if SERIAL_PRINT_ENABLE
		Serial.print("NEW: "); Serial.print(HMC6352data->d); Serial.println(" degrees");
		#endif
		
		// Compute the position of the indicator on a circle
		smlCompass.y = cos(HMC6352data->r) * (bigCompass.r + smlCompass.r + 3);
		smlCompass.x = sin(HMC6352data->r) * (bigCompass.r + smlCompass.r + 3);
				
		if(oldCompass.x != smlCompass.x) // If the value has changed...
		{
			if(lcPrint)
			{
				display->drawCircle(bigCompass.x, bigCompass.y, bigCompass.r, WHITE); // Draw the big white circle
				lcPrint = false;
			}
				
			// Display the indicator by removing the old one and printing the new one
			display->fillCircle(oldCompass.x + bigCompass.x, oldCompass.y + bigCompass.y, oldCompass.r,BACKGROUND_COLOR);
			display->fillCircle(smlCompass.x + bigCompass.x, smlCompass.y + bigCompass.y, smlCompass.r, RED);
			
			oldCompass = smlCompass; // Update the value
		}
	}
}

void DHMC6352::reset()
{
	oldCompass.x = smlCompass.x - 1; // Reset the value to a different value from the tested agaisnst one
	lcPrint = true; // 
}

#endif
