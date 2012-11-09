#ifndef GPRS_h
#define GPRS_h

#include <Wire.h>
#include "Arduino.h"
#define setupstate 0
#define sendstate 1
#define receivestate 2
#define STATE_NONE 0
#define STATE_ON 1
#define STATE_INITIALIZED 2
#define STATE_REGISTERED 4
#define STATE_POSFIX 8
#define BUF_LENGTH 100
#define ON true
#define OFF false

class GPRS
{
  
        public:
	GPRS(HardwareSerial *modemPort, char *pin);
	void  requestModem(const char *command, uint16_t timeout, boolean check, char *buf);
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
	char ATcommand[][10];
	boolean checktimeout(long timeout1);
	boolean readterminal();
	void setup();
	
	
};

#endif
