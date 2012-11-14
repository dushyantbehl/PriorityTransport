#include "GPRS.h"

//#define DEBUG_TERMINAL_CHAR
//#define DEBUG_TERMINAL_STRING
//#define DEBUG_TERMINAL_BREAK
//#define DEBUG_TERMINAL_RETURNING
//#define DEBUG_RUN_COUNT
//#define DEBUG_TRIMMING
#define DEBUG_BUFFER
#define DEBUG_SEND_PRINT
#define DEBUG_OK_PRINT

GPRS::GPRS(HardwareSerial *modemPort, user * users, GPS * gps, RFID *rfid,byte resetonoffpin)
	{
		modempin = modemPort;
                userdata = users;
                gpsdata = gps;
                rfiddata = rfid;
		waiting = OFF;
		count =0;
                ATindex = 0;
                runcount = 0;
                OnOffPin = resetonoffpin;
                ATsetupcommand[0] = "AT";
                ATsetupcommand[1] = "AT+IPR=19200";
                ATsetupcommand[2] = "AT+CMEE=2";
                ATsetupcommand[3] = "AT+CREG?";
                ATsetupcommand[4] = "AT+FLO=0";
                ATsetupcommand[5] = "AT+CGDCONT=1,\"IP\",\"airtelgprs.com\"";
                ATsetupcommand[6] = "AT#GPRS=1";
                ATsendcommand[0] = "AT#SD=2,0,80,\"www.utkarshsins.com\"";
                ATsendcommand[1] = "POST /priority/arduino.php HTTP/1.1\r\nHOST: www.utkarshsins.com\r\nUser-Agent: HTTPTool/1.1\r\nContent-Type: application/x-www-form-urlencoded";
                ATsendcommand[2] = "+++\nAT#SH=2";
                Wire.begin();
                pinMode(OnOffPin, OUTPUT);
                digitalWrite(OnOffPin, HIGH);
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

void GPRS::switchOn()
{
	Serial.println("switching on");
	digitalWrite(OnOffPin, HIGH);
	delay(1000);
	digitalWrite(OnOffPin, LOW);
	delay(1000);
	Serial.println("done");
}

void GPRS::switchOff()
{
	Serial.println("switching off");
	digitalWrite(OnOffPin, HIGH);
	delay(2000);
	digitalWrite(OnOffPin, LOW);
	delay(1000);
	Serial.println("done");
}

void GPRS::resetpin()
{
         
	/*switchOff();
	delay(10000);
        switchOn();
        delay(10000);*/
        /*
        if(digitalRead(OnOffPin) == HIGH)
        {
          Serial.println("SETTING LOW");
          digitalWrite(OnOffPin, LOW);
        }
        else if(digitalRead(OnOffPin) == LOW)
        {
          Serial.println("SETTING HIGH");
          digitalWrite(OnOffPin, HIGH);
        } */
        Serial.println("RESETTING");
        digitalWrite(OnOffPin, HIGH);
        digitalWrite(OnOffPin, LOW);
        
/*        digitalWrite(OnOffPin, LOW);
        Serial.println("set to low");
	Serial.println("Reset done");*/
        delay(15000);
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
                else if(checkstring.indexOf("activated") != -1)
                {
                  Serial.println("................. in the loop.......................");
                     //if(checkstring.indexOf("\r\n") != -1)
                     //{
                       checkstring = checkstring.substring(checkstring.indexOf("activated")+9);
                     //}
                      return 4;
                }
                else if(checkstring.indexOf("failed\r\n") != -1)
                {
                     if(checkstring.indexOf("\r\n") != -1)
                     {
                       checkstring = checkstring.substring(checkstring.indexOf("failed")+8);
                     }
                      return 5;
                }
                else if(checkstring.indexOf("can not resolve DN\r\n") != -1)
                {
                     if(checkstring.indexOf("\r\n")!=-1)
                     {
                       checkstring = checkstring.substring(checkstring.indexOf("can not resolve DN\r\n")+20);
                     }
                      return 6;
                }
                else if(checkstring.indexOf("sim not inserted\r\n") != -1)
                {
                     if(checkstring.indexOf("\r\n")!=-1)
                       checkstring = checkstring.substring(checkstring.indexOf("sim not inserted\r\n")+18);
                      return 7;
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
				if(variable==1 || variable==4) {
					waiting = OFF;
                                        count =0;
                                        if(ATindex == 4)
                			  Serial.println("Started GM865");
                                        else if(ATindex == setupcount -1)
                                        {
                                          Serial.println("Connected to Server");
                                          gprsstate = sendstate;
                                          Serial.println("[STATE CHANGE]  state changed from 0 to 1");
                                        }
                                        if(ATindex < setupcount-1)
                			  ATindex++;
                                        else
                                        {
                                          ATindex = 0;
                                        }	
                                }
				else if(variable==0 || variable == 5 || variable == 6 || variable ==7)
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
                        if(ATindex != 2)
          			requestModem(ATsendcommand[ATindex], 1000);
                        else if(ATindex== 2)
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
                        if(ATindex == 1)
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
                                      if(ATindex<sendcount)
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
  Serial.println("[SENDING] " + command + "\n[state] = " + gprsstate);
  modempin->print(command);
  modempin->print('\r');
  modempin->print('\n');

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

