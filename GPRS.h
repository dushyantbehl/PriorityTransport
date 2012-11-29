#ifndef GPRS_h
#define GPRS_h

#include <Wire.h>
#include "User.h"
#include "Arduino.h"
#include "GPS.h"
#include "LCD.h"

#define setupstate 0
#define sendstate 1
#define waitingtosendstate 2
#define STATE_NONE 0
#define STATE_ON 1
#define STATE_INITIALIZED 2
#define STATE_REGISTERED 4
#define STATE_POSFIX 8
#define BUF_LENGTH 1000
#define ON true
#define OFF false
#define N_GPS 60
#define N_RFID 10
#define setuptimeout 8000
#define sendtimeout 40000
#define cycletimeout 60000
#define sendcount 3
#define setupcount 7


typedef struct {
  int uid;
  long time;
  boolean to_send;
} RFID;

class GPRS
{
	public:
	GPRS(HardwareSerial *modemPort, user *, GPS*, RFID*, LCD*);
	void  requestModem(const String command, uint16_t timeout);
        void begin();
	boolean run();
        void long_parse(String);

	private:

        String checkstring;
        int ok_variable;
        user * userdata;
        GPS    *gpsdata;
        RFID* rfiddata;
        LCD* lcd;
        
        int runcount;
	int count;
	int gprsstate;
	long timestart;
	long cycletimestart;
	int timeout;
	bool waiting;
	HardwareSerial *modempin;
	//char check[100];
	void reset();
	int ATindex;
	String ATsetupcommand[7];
	String ATsendcommand[2];
	boolean checktimeout(long timeout1);
	int readterminal();
	void setup();
        void send();
        void receive();	
};

#endif
