#ifndef _DataLog_H
#define _DataLog_H

#include <Arduino.h>

class DataLog {
	public:
		DataLog();
		template <class T> void addData(T data);
		void addData(float data);
		void addData(double data);
		int saveData();
		String packet;
		static void invalidateData()
		{
			validData = false;
		}
		static void revalidateData()
		{
			validData = true;
		}
		static bool safeData()
		{
			return validData;
		}
		static history printedTime;
	private:
		static char* FileName;
		static bool validData;
	};
	
	char* DataLog::FileName = "DATAFILE.CSV";
	bool DataLog::validData = true;
	history DataLog::printedTime = {0,0};
	
	DataLog::DataLog()
	{	
		packet = "";
		
		#if DELETE_DATAFILE_ON_STARTUP && !DISABLE_SDCARD_WRITING
		while(SD.exists(FileName))
		{
			/* If so then delete it */
			#if SERIAL_PRINT_ENABLE
			Serial.print(FileName);
			Serial.println(" already exists...DELETING");
			#endif
			SD.remove(FileName);
		}
		#endif
		
		#if SERIAL_PRINT_ENABLE && !DISABLE_SDCARD_WRITING
		Serial.println("Data init.");
		#endif
	}

	template <class T> void DataLog::addData(T data)
	{
		packet += String(data);
		packet += ",";
		
		#if SERIAL_PRINT_ENABLE
		Serial.println("Data added.");
		#endif
	}
	
	void DataLog::addData(float data)
	{
		addData((double) data);
	}
	
	void DataLog::addData(double data)
	{
		char cString[50];
		
		dtostrf(data, 5, 4, cString);
		
		addData(cString);
	}
	
	int DataLog::saveData()
	{
		if(DISABLE_SDCARD_WRITING)
		{
			return 3;
		}
		
		digitalWrite(DATAWRITE_LED, HIGH);
		#if SERIAL_PRINT_ENABLE
		Serial.print("Data save started: ");
		Serial.println(FileName);
		#endif
		
		
		File dataFile = SD.open(FileName, FILE_WRITE);
	
		if(dataFile)
		{
			// packet += String(42);
			// packet += ",";
			
			dataFile.println(packet);
			dataFile.close();
			
			packet = "";
			
			#if SERIAL_PRINT_ENABLE
			Serial.println("Data save finished.");
			#endif
			
			digitalWrite(DATAWRITE_LED, LOW);
			return 0;
		}
		else
		{
			#if SERIAL_PRINT_ENABLE
			Serial.println("Data save failed.");
			#endif
			
			dataFile.close();
			
			digitalWrite(DATAWRITE_LED, LOW);
			return 1;
		}
	}

#endif