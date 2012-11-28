#ifndef _GPS_H_
#define _GPS_H_
#include "TinyGPS.h"

#include <SoftwareSerial.h>
#include "Arduino.h"

#define _MAX_LENGTH 60 //Specify the length of GPS array

// Structure storing position and time
typedef struct {
	long lat;  //+ for Northern hemisphere, - for Southern hemisphere
	long lon;  //+ for East of Greenwich, - for West of Greenwich
        float date; //Date given in float representation as ddmmyy
	float time; //Time given in float represented as hhmmss.sss
} myPosition;

class GPS{
	
	public:
	GPS(HardwareSerial* modemPin); //Constructor

	bool feedGPS();	//Fuction to feed input to GPS, returs True 
					//when a correct sentence is received

	void gpsdump();	//Function to Dump the GPS data to the GPS position array

	void begin();	//Function to setup the GPS Class
	
	void gpsloop();	//The loop function to update the GPS array from time to time 1s
	
	int getHead();	
	void setHead(int val);
	myPosition *positions(int);
	bool isValid();
		
	private:
	myPosition pos[_MAX_LENGTH];    //GPS data array and its iterator
	int head ;
        bool waiting ;

	bool GPS_VALID_FLAG;			//Flag to check if GPS data is vaild or not
	
	HardwareSerial *modemPort;
	TinyGPS gps;
	long lat, lon ;
	unsigned long age, start;
	int year;
	byte month, day, hour, minute, second, hundredths;
	unsigned short sentences, failed;
	float ddmmyy, hhmmss ;

};

#endif
