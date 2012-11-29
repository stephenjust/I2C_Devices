// #include "Config.h"

class Displays
{
public:
	template <class T> static void print(T* d, vector* vect)
	{
		#if ALL_LABELS
		print(d, "Vector ");
		#endif
		
		print(d, "x: ");
		print(d, vect->x);
		print(d, " y: ");
		print(d, vect->y);
		print(d, " z: ");
		print(d, vect->z);
	}
	template <class T> static void print(T* display, therm* temp)
	{
		#if ALL_LABELS
		print(display, "Temp: ");
		#endif
		
		print(display, temp->temp);
		print(display, " C ");
		print(display, temp->voltage);
		print(display, " V");
	}
	template <class T, class D> static void print(T* display, D data)
	{
		display->print(data);
	}
	template <class T, class D> static void println(T* display, D data)
	{
		print(display, data);
		display->println("");
	}
private:
	
};
// 
// class InfoPane : Display
// {
// public:
// 	void setTitle();
// 	DataPane * addData();
// 	void setColor();
// private:
// 	
// };
// 
// 
// 
// class DataPane : InfoPane
// {
// public:
// 	void setLabel();
// 	void setData();
// }