#ifndef _DHMC6352_H_
#define _DHMC6352_H_

class DHMC6352 : HMC6352 {
public:
	template <class T> static void display(T* display);
protected:
	static circle bigCompass;
	static circle smlCompass;
	static circle oldCompass;
	static float* degHMC6352data;
};

circle DHMC6352::bigCompass;
circle DHMC6352::smlCompass;
circle DHMC6352::oldCompass;
float* DHMC6352::degHMC6352data = getData();

template <class T> void DHMC6352::display(T* display)
{
	if(!ADXL345::check())
	{
		digitalWrite(GYRO_WARNING_LED, HIGH);
		display->setCursor(135,10);
		display->setTextSize(2);
		display->setTextColor(RED);
		display->print("Please center");
		display->setCursor(175,25);
		display->print("device");
		Displays::resetText(display);
		digitalWrite(GYRO_WARNING_LED, LOW);
		return;
	}
	else
	{
		display->setCursor(135,10);
		display->setTextSize(2);
		display->setTextColor(BACKGROUND_COLOR);
		display->print("Please center");
		display->setCursor(175,25);
		display->print("device");
		Displays::resetText(display);
	}
	
	
	// if(Sensors::getLongTime()%50 == 0)
	if(sensorSecond)
	{
		sensorSecond = 0;
		
		bigCompass.r = 63;
		smlCompass.r = 5;
		bigCompass.x = 210;
		bigCompass.y = 125;
				
		display->drawCircle(bigCompass.x, bigCompass.y, bigCompass.r, WHITE);
				
		display->setCursor(bigCompass.y, bigCompass.x);
		display->setTextColor(BACKGROUND_COLOR,BACKGROUND_COLOR);
		display->print(*degHMC6352data);
		display->print(" degrees");
		
		#if SERIAL_PRINT_ENABLE
		Serial.print("OLD: ");
		Serial.print(*degHMC6352data);
		Serial.println(" degrees ");
		#endif
			
		degHMC6352data = getData();
		
		display->setCursor(bigCompass.y, bigCompass.x);
		display->setTextColor(WHITE,BACKGROUND_COLOR);
		display->print(*degHMC6352data);
		display->print(" degrees");
		
		#if SERIAL_PRINT_ENABLE
		Serial.print("NEW: ");
		Serial.print(*degHMC6352data);
		Serial.println(" degrees");
		#endif
		
		float radianHMC6352data = *degHMC6352data * 1000/57296;
		smlCompass.y = cos(radianHMC6352data) * (bigCompass.r + smlCompass.r + 2);
		smlCompass.x = sin(radianHMC6352data) * (bigCompass.r + smlCompass.r + 2);
				
		if(oldCompass.x != smlCompass.x)
		{
			display->fillCircle(oldCompass.x + bigCompass.x, oldCompass.y + bigCompass.y, oldCompass.r,BACKGROUND_COLOR);
			display->fillCircle(smlCompass.x + bigCompass.x, smlCompass.y + bigCompass.y, smlCompass.r, RED);
					
			oldCompass = smlCompass;
		}
	}
}

#endif
