#ifndef _DHMC6352_H_
#define _DHMC6352_H_

class DHMC6352 : HMC6352 {
public:
	template <class T> static void display(T* display);
	static void reset();
protected:
	static const circle bigCompass;
	static circle smlCompass;
	static circle oldCompass;
	static angle* HMC6352data;
	static bool lcPrint;
};

const circle DHMC6352::bigCompass = {210, 125, 63};
circle DHMC6352::smlCompass;
circle DHMC6352::oldCompass;
angle* DHMC6352::HMC6352data = getData();

bool DHMC6352::lcPrint = true;

template <class T> void DHMC6352::display(T* display)
{
	if(!ADXL345::check())
	{
		digitalWrite(GYRO_WARNING_LED, HIGH);
		display->setCursor(135,10);
		display->setTextSize(2);
		display->setTextColor(RED);
		display->print("Please level");
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
		display->print("Please level");
		display->setCursor(175,25);
		display->print("device");
		Displays::resetText(display);
	}
	
	
	if(sensorSecond)
	{
		sensorSecond = 0;
		
		smlCompass.r = 5;
		
		display->setCursor(bigCompass.y, bigCompass.x);
		display->setTextColor(BACKGROUND_COLOR,BACKGROUND_COLOR);
		display->print(HMC6352data->d);
		display->print(" degrees");
		
		#if SERIAL_PRINT_ENABLE
		Serial.print("OLD: "); Serial.print(HMC6352data->d); Serial.println(" degrees ");
		#endif
			
		HMC6352data = getData();
		
		display->setCursor(bigCompass.y, bigCompass.x);
		display->setTextColor(WHITE,BACKGROUND_COLOR);
		display->print(HMC6352data->d);
		display->print(" degrees");
		
		#if SERIAL_PRINT_ENABLE
		Serial.print("NEW: "); Serial.print(HMC6352data->d); Serial.println(" degrees");
		#endif
		
		smlCompass.y = cos(HMC6352data->r) * (bigCompass.r + smlCompass.r + 2);
		smlCompass.x = sin(HMC6352data->r) * (bigCompass.r + smlCompass.r + 2);
				
		if(oldCompass.x != smlCompass.x)
		{
			if(lcPrint)
			{
				display->drawCircle(bigCompass.x, bigCompass.y, bigCompass.r, WHITE);
				lcPrint = false;
			}
				
			
			display->fillCircle(oldCompass.x + bigCompass.x, oldCompass.y + bigCompass.y, oldCompass.r,BACKGROUND_COLOR);
			display->fillCircle(smlCompass.x + bigCompass.x, smlCompass.y + bigCompass.y, smlCompass.r, RED);
					
			oldCompass = smlCompass;
		}
	}
}

void DHMC6352::reset()
{
	oldCompass.x = smlCompass.x - 1;
	lcPrint = true;
}

#endif
