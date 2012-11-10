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
#define BUF_LENGTH 1000
#define ON true
#define OFF false
#define N_GPS 5
#define N_RFID 1
#define setuptimeout 5000
#define sendtimeout 5000
#define receivetimeout 5000
#define cycletimeout 60000

class User
{
    public:
    void parse();
    
    private:
    long pick_location[5];
    long pick_time[5];
    long drop_location[5];
    long drop_time[5];
    String entry_num;
    int rfid_tag[5];
};

class GPRS
{
	public:
	GPRS(HardwareSerial *modemPort, char *pin);
	void  requestModem(const String command, uint16_t timeout, boolean check);
	void run();

	private:
	int gprsstate;
	long timestart;
	long cycletimestart;
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