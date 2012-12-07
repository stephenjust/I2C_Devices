#ifndef _DataLog_H
#define _DataLog_H

#ifndef _CONFIG_H_

// Ensures that these defines ar available if the Config.h file is not present. These should be moved to their own file if you are modularizing

#define SD_CS 33  // Chip select line for SD card
#define DISABLE_SDCARD_WRITING 0
#define DELETE_DATAFILE_ON_STARTUP 0
#define SERIAL_PRINT_ENABLE 0

#endif

class DataLog {
	public:
		DataLog(char * filename);
		template <class T> void addData(T data);
		void addData(float data);
		void addData(double data);
		int saveData();
		String packet; // Use of this object does cause a memory allocation whenever new data is added to the string, this results in the length of the data packet to now become limited to the size of the available ram rather than the 
		static history printedTime;
	private:
		static char FileName[50];
};
	
char DataLog::FileName[50]; // The filename written to the SD card, defined on startup
history DataLog::printedTime = {0,0}; // Used to ensure that multiple measurements are not taken more than once per second

DataLog::DataLog(char * filename)
{
	for(int i = 0; i < strlen(filename)*sizeof(char); i++)
		FileName[i] = filename[i];
	
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
	char * cString = (char *) malloc(sizeof(char)*50); // Allocate memory to a C string
		
	dtostrf(data, 5, 4, cString); // Convert the inputted double into the C string
		
	addData(cString); // Add the C string to the data packet
	
	free(cString); // Unallocate the memeory for the C string
}
	
int DataLog::saveData()
{
	if(DISABLE_SDCARD_WRITING)
	{
		packet = ""; // Clear the packet string
		return 0;
	}
		
	digitalWrite(DATAWRITE_LED, HIGH);
		
	#if SERIAL_PRINT_ENABLE
	Serial.print("Data save started: ");
	Serial.println(FileName);
	#endif
		
	File dataFile = SD.open(FileName, FILE_WRITE);
		
	if(dataFile)
	{
		// Adding a packet length as the check digit to ensure that all data was written to the sd card sucesfully that was contained in the packet string
		packet += packet.length();
		packet += ",";
			
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