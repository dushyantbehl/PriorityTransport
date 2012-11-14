#ifndef GPRS_h
#define GPRS_h

#include <Wire.h>
#include "User.h"
#include "Arduino.h"
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
#define N_GPS 5
#define N_RFID 1
#define setuptimeout 10000
#define sendtimeout 60000
#define cycletimeout 60000


typedef struct {
	long latitude;
	long longitude;
        long timestamp;
} GPS;

typedef struct {
  int uid;
  long time;
  boolean to_send;
} RFID;

class GPRS
{
	public:
	GPRS(HardwareSerial *modemPort, user *, GPS *, RFID*);
	void  requestModem(const String command, uint16_t timeout);
        void begin();
	void run();

	private:

        String checkstring;
        
        user * userdata;
        GPS * gpsdata;
        RFID* rfiddata;
        
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
