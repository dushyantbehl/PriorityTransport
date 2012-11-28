#include "GPS.h"

GPS::GPS(HardwareSerial *modemPin){
	modemPort = modemPin ;
	head = -1 ;
	GPS_VALID_FLAG = false ;
        waiting = true ;
}

//Function to setup the GPS Class
void GPS::begin(){
	modemPort->begin(4800) ;
	head = -1 ;
	GPS_VALID_FLAG = false ;
        Serial.print("GPS device Started\n");
	return ;
}

int GPS::getHead(){
	return head ;
}

void GPS::setHead(int val){
	head = val ;
	return ;
}

myPosition * GPS::positions(int k){
        int index = head + 1 ;
        for( int j=0; j<=k ;j++){
          index = (index+1)%_MAX_LENGTH ;
        }
      return &pos[index] ;  	
}

bool GPS::isValid(){
	return GPS_VALID_FLAG ;
}

//Returns true when a valid GPS sentence is received
bool GPS::feedGPS(){
	if (modemPort->available())
	{
		if (gps.encode(modemPort->read())){
                Serial.println("GPS : New Line Received");
	        waiting = false ;
                return true;
                }
                else return false ;
  	}
  	return false;
}

//Dump the GPS data to the GPS positions array
void GPS::gpsdump(){
	
	//Get the latitude and longitude from the data
           gps.get_position(&lat, &lon, &age);
		if (age == TinyGPS::GPS_INVALID_AGE){
			//Not a valid data 	
			GPS_VALID_FLAG = false;
			return;
		}
		else if( age > 10000 ){
			//More then 10 seconds old data.............possible stale data 
			GPS_VALID_FLAG = false;
			return;
		}

	GPS_VALID_FLAG = true ;
	
    gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
		if (age == TinyGPS::GPS_INVALID_AGE){
			//Not a valid data 	
			GPS_VALID_FLAG = false;
			return;
		}
		else if( age > 10000 ){
			//More then 10 seconds old data.............possible stale data 
			GPS_VALID_FLAG = false;
			return;
		}

	GPS_VALID_FLAG = true ;

	//Convert the date and time to a usable format
	int yy = year%100 ;
	int mm = (int)month%100 ;
	int dd = (int)day%100 ;

	int hh = (int)hour%100;
	int minu = (int)minute%100;
	int ss = (int)second%100;

	hhmmss = (hh*10000.00) + (minu*100.00) + (ss) ;
	ddmmyy = (dd*10000.00) + (mm*100.00) + (yy) ;

        if(hhmmss-pos[head].time < 1) {
        return ;}

	//Adding the received data to queue 
	myPosition temp ;
	temp.lat = lat ;
	temp.lon = lon ;
	temp.date = ddmmyy ;
	temp.time = hhmmss ;
	head = (head+1)%_MAX_LENGTH ;
	pos[head] = temp ;
        
        /*Serial.println("Data Dumped to the Array");
        Serial.print("lat:- ");
        Serial.print(pos[head].lat);
        Serial.print(" lon:- ");
        Serial.print(pos[head].lon);
        Serial.print(" date:- ");
        Serial.print(pos[head].date);
        Serial.print(" time:- ");
        Serial.print(pos[head].time);
        Serial.println();
        Serial.println(GPS_VALID_FLAG); */
                
	return ;
}

void GPS::gpsloop(){

	feedGPS();
        if(waiting == false){
	gpsdump();
        waiting = true ;
        }	
        return;
}	
