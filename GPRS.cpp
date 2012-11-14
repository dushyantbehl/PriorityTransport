#include "GPRS.h"

//#define DEBUG_TERMINAL_CHAR
//#define DEBUG_TERMINAL_STRING
//#define DEBUG_TERMINAL_BREAK
//#define DEBUG_TERMINAL_RETURNING
//#define DEBUG_RUN_COUNT
//#define DEBUG_TRIMMING
//#define DEBUG_BUFFER
#define DEBUG_SEND_PRINT
#define DEBUG_OK_PRINT

GPRS::GPRS(HardwareSerial *modemPort, user * users, GPS * gps, RFID *rfid)
	{
		modempin = modemPort;
                userdata = users;
                gpsdata = gps;
                rfiddata = rfid;
		waiting = OFF;
		count =0;
                ATindex = 0;
                runcount = 0;
                ATsetupcommand[0] = "AT";
                ATsetupcommand[1] = "AT+IPR=19200";
                ATsetupcommand[2] = "AT+CMEE=2";
                ATsetupcommand[3] = "AT+CREG?";
                ATsetupcommand[4] = "AT+FLO=0";
                ATsetupcommand[5] = "AT+CGDCONT=1,\"IP\",\"airtelgprs.com\"";
                ATsetupcommand[6] = "AT#GPRS=1";
                //ATsendcommand[0] = "AT#SKTD=0,80,\"www.utkarshsins.com\",255,0";
                ATsendcommand[0] = "AT#SD=2,0,80,\"www.utkarshsins.com\"";
                ATsendcommand[1] = "POST /priority/arduino.php HTTP/1.1\r\nHOST: www.utkarshsins.com\r\nUser-Agent: HTTPTool/1.1\r\nContent-Type: application/x-www-form-urlencoded";
	}

void GPRS::begin()
{
  modempin->begin(19200);
}
void GPRS::reset()
{
	gprsstate = setupstate;
	ATindex=0;
	waiting=OFF;
	timeout = setuptimeout;
	timestart=0;
	count =0;
}

boolean	GPRS::checktimeout(long timeout1)
	{
                return (millis() - timestart) <= timeout1;
	}
	
int	GPRS::readterminal()
	{
		int x=0;
		while(modempin->available())
		{
                        char check = modempin->read();
			checkstring += check;
                        #ifdef DEBUG_TERMINAL_CHAR
                        Serial.print("[TERMINAL] reading : ");
                        if(check == '\r')
                          Serial.println("\\r");
                        else if(check == '\n')
                          Serial.println("\\n");
                        else
                          Serial.println(check);
                        #endif
			x++;
		}
              
                #ifdef DEBUG_BUFFER
                {
                  String debugbuffer = checkstring;
                  debugbuffer.replace("\r","\\r");
                  debugbuffer.replace("\n","\\n");
                  Serial.println("[BUFFER] " + debugbuffer);
                }
                #endif
                
                if(checkstring.indexOf("\r\n")!=-1)
                {
                  // Serial.println("Found : " + checkstring);
                  #ifdef DEBUG_TRIMMING
                  Serial.println(checkstring.substring(0,checkstring.indexOf("\r\n")) + "\\r\\n" + checkstring.substring(checkstring.indexOf("\r\n")+2));
                  #endif
                  String checkconds = checkstring.substring(0,checkstring.indexOf("\r\n"));
                  if(checkconds.indexOf("NO") == -1 && checkconds.indexOf("OK") == -1 && checkconds.indexOf("CONNECT") == -1)
                  {
                    checkstring = checkstring.substring(checkstring.indexOf("\r\n")+2);
                    #ifdef DEBUG_TRIMMING
                    Serial.println("Trimmed First : "+checkstring);
                    Serial.println("Trimming End");
                    #endif
                  }
                }
                #ifdef DEBUG_TERMINAL_STRING
                Serial.print("[TERMINAL] string : ");
                Serial.println(checkstring);
                #endif
                #ifdef DEBUG_TERMINAL_BREAK
                Serial.println("[TERMINAL] break");
                #endif
		if(checkstring.indexOf("NO") != -1)
        				{
        						#ifdef DEBUG_TERMINAL_RETURNING
        						Serial.println("[TERMINAL] Returning : 0");
        						#endif
        						if(checkstring.indexOf("\r\n")!=-1)
        						{
        							#ifdef DEBUG_TRIMMING
        							Serial.println("Trimming : " + checkstring + "\t=>\t" + checkstring.substring(checkstring.indexOf("\r\n")+2));
        							Serial.println("Trimming End");
        							#endif
        							checkstring = checkstring.substring(checkstring.indexOf("\r\n")+2);
        						}
						return 0;
                }
		else if(checkstring.indexOf("OK\r\n") != -1)
                {
                        #ifdef DEBUG_OK_PRINT
                        Serial.println("OK");
                        #endif
                        
                        #ifdef DEBUG_TERMINAL_RETURNING
                        Serial.println("[TERMINAL] Returning : 1");
                        #endif
                        
                        if(checkstring.indexOf("\r\n")!=-1)
                        {
                          #ifdef DEBUG_TRIMMING
                          Serial.println("Trimming : " + checkstring + "\t=>\t" + checkstring.substring(checkstring.indexOf("\r\n")+2));
                          Serial.println("Trimming End");
                          #endif
                          checkstring = checkstring.substring(checkstring.indexOf("OK\r\n")+4);
                        }
						return 1;
                }
                else if(checkstring.indexOf("CONNECT\r\n") != -1)
                {
                        #ifdef DEBUG_OK_PRINT
                        Serial.println("CONNECT");
                        #endif
                        
                        #ifdef DEBUG_TERMINAL_RETURNING
                        Serial.println("[TERMINAL] Returning : 3");
                        #endif
                        
                        if(checkstring.indexOf("\r\n")!=-1)
                        {
                          #ifdef DEBUG_TRIMMING
                          Serial.println("Trimming : " + checkstring + "\t=>\t" + checkstring.substring(checkstring.indexOf("\r\n")+2));
                          Serial.println("Trimming End");
                          #endif
                          checkstring = checkstring.substring(checkstring.indexOf("CONNECT\r\n")+9);
                        }
			return 3;
                }
                else
                {
                        #ifdef DEBUG_TERMINAL_RETURNING
                        Serial.print("[TERMINAL] String : ");
                        Serial.println(checkstring);
                        Serial.println("[TERMINAL] Returning : 2");
                        #endif
			return 2;
                }
	}
		
void	GPRS::setup()
	{
		if(waiting==OFF)
		{
			//char buf[BUF_LENGTH];
                        requestModem(ATsetupcommand[ATindex], 1000);
			timestart = millis();
			waiting = ON;
			timeout = setuptimeout;		
		}
		else
		{
			if(modempin->available())
			{
				int variable = readterminal();
				if(variable==1) {
					waiting = OFF;
                                        count =0;
                                        if(ATindex == 4)
                			  Serial.println("Started GM865");
                                        else if(ATindex == 6)
                                        {
                                          Serial.println("Connected to Server");
                                          gprsstate = sendstate;
                                        }
                                        if(ATindex<6)
                			  ATindex++;
                                        else
                                        {
                                          ATindex = 0;
                                        }	
                                }
				else if(variable==0)
				{
					if(count < 3)
					{
						waiting = OFF;
						count++;
					}
					else
						reset();
				}
			}
			if(!checktimeout(timeout))
			{
				reset();
			}
		}
	}
	

// TODO:
// @Saurabh - Think of a way to find out that the http request
// should have ended and now the modem should switch the state
// to terminate the connection
void	GPRS::send()
	{
		if(waiting==OFF)
		{
			//char buf[BUF_LENGTH];
                        if(ATindex < 1)
          			requestModem(ATsendcommand[ATindex], 1000);
                        else 
                        {
                          // TODO: ESCAPE SEQUENCE + TERMINATION AT COMMAND
                          // @Akshay - Put this in the state machine 
                          // Replace the delays with timeouts
                          Serial.println("TERMINATING CONNECTION");
                          delay(2000);
                          Serial.println("+++");
                          modempin->print("+++");
                          delay(2000);
                          requestModem("AT#SH=2", 100);
                          
                          cycletimestart = millis();
                          gprsstate=waitingtosendstate;
                        }
                        
			waiting = ON;
                        if(ATindex == 2)
                        {
			    //Serial.println("sending request ...");
                            String string = "";
                            int k,j;
                            for(k=0;k<N_GPS;k++) 
                             {
                                string += "&latitude"+String(k+1, DEC)+"="+String(gpsdata[k].latitude, DEC)+"&longitude"+String(k+1, DEC)+"="+String(gpsdata[k].longitude, DEC)+"&gpstime"+String(k+1, DEC)+"="+String(gpsdata[k].timestamp, DEC);
                             }
                            for(j=0;j<N_RFID;j++) 
                             {
                               int count = 1;
                               if(rfiddata[j].to_send == true)
                               string += "uid"+String(count++, DEC)+"="+String(rfiddata[j].uid, DEC)+"&rfidtime"+String(count++, DEC)+"="+String(rfiddata[j].time, DEC);
                             }
                             int DATA_LENGTH = string.length();
                             {
                                  String lengthed = "Content-Length: " + String(DATA_LENGTH, DEC) + "\r\n\r\n";
                                  char buff[lengthed.length()+1];
                                  lengthed.toCharArray(buff,lengthed.length()+1);
                                  modempin->print(buff);
                                  Serial.println(buff);
                             }
                            char buff[string.length()+1];
                            string.toCharArray(buff,string.length()+1);
                            modempin->print(buff);
                            Serial.println(buff);
  
                            //modempin->print("\r\n");
                        } 
			timeout = sendtimeout;
		}
		else
		{
			if(modempin->available())
			{
				int variable = readterminal();
				if(variable==3)
                                {
                                	waiting = OFF;
                                      if(ATindex==0)
                                      {
                                          ATindex++;
                                          
                                      }
                                      else
                                      {
                                        Serial.println("Request Sent.");
                                        gprsstate = waitingtosendstate;
                                        ATindex = 0;
                                      }
                                }
				else if(variable==0)
				{
					if(count < 3)
					{
						waiting = OFF;
						count++;
					}
					else
                                        {
						reset();
                                        }
				}
                                else if(variable==1)
                                {
                                  ATindex = 0;
                                  waiting = OFF;
                                }
                                  
			}
			if(!checktimeout(timeout))
			{
				reset();
			}
		}
	}
	


void GPRS::requestModem(const String command, uint16_t timeout)
{
			
 // byte count = 0;
  //char *found = 0;
  
  //*buf = 0;
  Serial.println("[SENDING] " + command + "\n[state] = " + gprsstate);
  modempin->print(command);
  modempin->print('\r');
  modempin->print('\n');
  /*count = getsTimeout(buf, timeout);
  if (count) {
    if (check) {
      found = strstr(buf, "\r\nOK\r\n");
      if (found) {
	Serial.println("->ok");
      }
      else {
	Serial.print("->not ok: ");
	Serial.println(buf);  
      } 
    }
    else {
      Serial.print("->buf: ");
      Serial.println(buf);  
    }
  }
  else {
    Serial.println("->no respone");
  }*/
  //return count;
}

void	GPRS::run()
	{
                #ifdef DEBUG_RUN_COUNT
                runcount++;
                Serial.print("[RUNCOUNT] ");
                Serial.println(runcount);
                #endif
                
		if(gprsstate==setupstate)
		{
			setup();
		}
		else if(gprsstate==sendstate)
		{
			//cycletimestart = millis();
			send();
		}
		else if(gprsstate==waitingtosendstate)
		{
			if(millis()-cycletimestart>=cycletimeout)
				gprsstate = sendstate;
		}
	}


/*byte  getsTimeout(char *buf, uint16_t timeout) {
  byte count = 0;
  long timeIsOut = 0;
  char c;
  *buf = 0;
  timeIsOut = millis() + timeout;
  while (timeIsOut > millis() && count < (BUF_LENGTH - 1)) {  
    if (modem->available()) {
      count++;
      c = modem->read();
      *buf++ = c;
      timeIsOut = millis() + timeout;
    }
  }
  if (count != 0) {
    *buf = 0;
    count++;
  }
  return count;
}*/

/*void  init() {
  Serial.println("initializing modem ...");
  char buf[BUF_LENGTH];
  //char cmdbuf[30] = "AT+CPIN=";
  //strcat(cmdbuf, pin);
  requestModem("AT", 1000, true, buf);
  requestModem("AT+IPR=19200", 1000, true, buf);
  requestModem("AT+CMEE=2", 1000, true, buf);
  // requestModem(cmdbuf, 1000, true, buf);
  state |= STATE_INITIALIZED;
  Serial.println("done");
}
*/

/*void  version() {
  char buf[BUF_LENGTH];
  Serial.println("version info ...");
  requestModem("AT+GMI", 1000, false, buf);
  requestModem("AT+GMM", 1000, false, buf);
  requestModem("AT+GMR", 1000, false, buf);
  requestModem("AT+CSQ", 1000, false, buf);
  Serial.println("done");
}


void  sendSMS(char *number, char *message) {
  char buf[BUF_LENGTH];
  char cmdbuf[30] = "AT+CMGS=\"";
  Serial.println("sending SMS ...");
  requestModem("AT+CMGF=1", 1000, true, buf);             // send text sms
  strcat(cmdbuf, number);
  strcat(cmdbuf, "\"");
  requestModem(cmdbuf, 1000, true, buf);
  modem->print(message);
  //modem->print(0x1a, BYTE);
  getsTimeout(buf, 2000);
  Serial.println(buf);
  Serial.println("done");
}
*/

/*void  checkNetwork() {
  char buf[BUF_LENGTH];
  char result;
  Serial.println("checking network ...");
  requestModem("AT+CREG?", 1000, false, buf);
  result = buf[20];
  if (result == '1') {
    state |= STATE_REGISTERED;
  }
  else {
    state &= ~STATE_REGISTERED;
  }
  Serial.println("done");
}*/


/*
 * eplus
 *   internet.eplus.de, eplus, eplus
 * o2
 *   internet, <>, <>
 */
/*void  initGPRS() {
  char buf[BUF_LENGTH];
  Serial.println("initializing GPRS ...");
  requestModem("AT+FLO=0", 1000, false, buf);
  requestModem("AT+CGDCONT=1,\"IP\",\"airtelgprs.com\"", 1000, false, buf);   
  // requestModem("AT#USERID=\"\"", 1000, false, buf);
  // requestModem("AT#PASSW=\"\"", 1000, false, buf);
  // Serial.println("done");
}    


void  enableGPRS() {
  char buf[BUF_LENGTH];
  Serial.println("switching GPRS on ...");
  requestModem("AT#GPRS=1", 1000, false, buf);
  Serial.println("done");
}


void  disableGPRS() {
  char buf[BUF_LENGTH];
  Serial.println("switching GPRS off ...");
  requestModem("AT#GPRS=0", 1000, false, buf);
  Serial.println("done");
}


boolean  openHTTP(char *domain) {
  char buf[BUF_LENGTH];
  char cmdbuf[50] = "AT#SKTD=0,80,\"";
  byte i = 0;
  boolean connect = false;
  Serial.println("opening socket ...");
  strcat(cmdbuf, domain);
  strcat(cmdbuf, "\",0,0\r");
  requestModem(cmdbuf, 1000, false, buf);
  do {
    getsTimeout(buf, 1000);
    Serial.print("buf:");
    Serial.println(buf);
    if (strstr(buf, "CONNECT")) {;
      connect = true;
      break;
    }
  } while (i++ < 10);
  if (!connect) {
    Serial.println("failed");
  }
  return (connect);
}
*/
/*
void  send(char *buf) {
    Serial.print(buf);
    modem->print(buf);
}


char * receive(char *buf) {
  getsTimeout(buf, 1000);
  return buf;
}*/


/*void  warmStartGPS() {
  char buf[BUF_LENGTH];
  Serial.println("warm start GPS ...");
  requestModem("AT$GPSR=2", 1000, false, buf);
  Serial.println("done");
}


Position  requestGPS(void) {
  char buf[150];
  Serial.println("requesting GPS position ...");
  requestModem("AT$GPSACP", 2000, false, buf);
  if (strlen(buf) > 29) {
    actPosition.fix = 0;	// invalidate actual position			
    parseGPS(buf, &actPosition);
    if (actPosition.fix > 0) {
      Serial.println(actPosition.fix);
      state |= STATE_POSFIX;
    }
  }
  else {
    actPosition.fix = 0;
    Serial.println("no fix");
    state &= ~STATE_POSFIX;
  }
  return actPosition;
}


Position  getLastPosition() {
  return actPosition;
}


/*
 * Parse the given string into a position record.
 * example:
 * $GPSACP: 120631.999,5433.9472N,00954.8768E,1.0,46.5,3,167.28,0.36,0.19,130707,11\r
 */
/*void  parseGPS(char *gpsMsg, Position *pos) {

  char time[7];
  char lat_buf[12];
  char lon_buf[12];
  char alt_buf[7];
  char fix;
  char date[7];
  char nr_sat[4];
	
  gpsMsg = skip(gpsMsg, ':');                // skip prolog
  gpsMsg = readToken(gpsMsg, time, '.');     // time, hhmmss
  gpsMsg = skip(gpsMsg, ',');                // skip ms
  gpsMsg = readToken(gpsMsg, lat_buf, ',');  // latitude
  gpsMsg = readToken(gpsMsg, lon_buf, ',');  // longitude
  gpsMsg = skip(gpsMsg, ',');                // hdop
  gpsMsg = readToken(gpsMsg, alt_buf, ',');  // altitude
  fix = *gpsMsg++;                           // fix, 0, 2d, 3d
  gpsMsg++;
  gpsMsg = skip(gpsMsg, ',');                // cog, cource over ground
  gpsMsg = skip(gpsMsg, ',');                // speed [km]
  gpsMsg = skip(gpsMsg, ',');                // speed [kn]
  gpsMsg = readToken(gpsMsg, date, ',');     // date ddmmyy
  gpsMsg = readToken(gpsMsg, nr_sat, '\n');  // number of sats

  if (fix != '0') {
    parsePosition(pos, lat_buf, lon_buf, alt_buf);
    pos->fix = fix;
  }

}*/


/*
 * Skips the string until the given char is found.
 */
/*char * skip(char *str, char match) {
  uint8_t c = 0;
  while (true) {
    c = *str++;
    if ((c == match) || (c == '\0')) {
      break;
    }
  }
  return str;
}*/


/*
 * Reads a token from the given string. Token is seperated by the 
 * given delimiter.
 */
/*char * readToken(char *str, char *buf, char delimiter) {
  uint8_t c = 0;
  while (true) {
    c = *str++;
    if ((c == delimiter) || (c == '\0')) {
      break;
    }
    else if (c != ' ') {
      *buf++ = c;
    }
  }
  *buf = '\0';
  return str;
}
*/

/*
 * Parse and convert the position tokens. 
 */
/*void  parsePosition(Position *pos, char *lat_str, char *lon_str, char *alt_str) {
  char buf[10];
  parseDegrees(lat_str, &pos->lat_deg, &pos->lat_min);
  parseDegrees(lon_str, &pos->lon_deg, &pos->lon_min);
  readToken(alt_str, buf, '.');
  pos->alt = atol(buf);
}*/


/*
 * Parse and convert the given string into degrees and minutes.
 * Example: 5333.9472N --> 53 degrees, 33.9472 minutes
 * converted to: 53.565786 degrees 
 */
/*void  parseDegrees(char *str, int *degree, long *minutes) {
  char buf[6];
  uint8_t c = 0;
  uint8_t i = 0;
  char *tmp_str;
	
  tmp_str = str;
  while ((c = *tmp_str++) != '.') i++;
  strlcpy(buf, str, i-1);
  *degree = atoi(buf);
  tmp_str -= 3;
  i = 0;
  while (true) {
    c = *tmp_str++;
    if ((c == '\0') || (i == 5)) {
      break;
    }
    else if (c != '.') {
      buf[i++] = c;
    }
  }
  buf[i] = 0;
  *minutes = atol(buf);
  *minutes *= 16667;
  *minutes /= 1000;
}*/
