#ifndef GPRS_h
#define GPRS_h

#include <Wire.h>
#include "Arduino.h"
#define setupstate 0
#define sendstate 1
#define receivestate 2
#define waitingtosendstate 3
#define STATE_NONE 0
#define STATE_ON 1
#define STATE_INITIALIZED 2
#define STATE_REGISTERED 4
#define STATE_POSFIX 8
#define BUF_LENGTH 100
#define ON true
#define OFF false
#define N_GPS 5
#define N_RFID 1
#define WAITING_TO_BOARD 0
#define IN_THE_BUS 1
#define DROPPED_AT_LOCATION 2

long latitude[5];
long longitude[5];
long time[5];
long idtime[5];
int RFID[8];

class User
{
    public:
    long pick_latitude[5];
    long pick_longitude[5];
    long pick_time[5];
    long drop_latitude[5];
    long drop_longitude[5];
    long drop_time[5];
    String entry_num;
    int status;
    int rfid_tag[8];
};


class GPRS
{
  
        public:
	GPRS(HardwareSerial *modemPort, char *pin);
	void  requestModem(const String command, uint16_t timeout, boolean check, char *buf);
	void run();

	private:
	int gprsstate;
	long timestart;
	int timeout;
	bool waiting;
	HardwareSerial *modempin;
	char pin[5];
	byte state;
	int ATindex;
	String ATsetupcommand[7];
	String ATsendcommand[2];
	boolean checktimeout(long timeout1);
	boolean readterminal();
	void setup();
        void send();
        void receive();	
};

#endif
