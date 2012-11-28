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

GPRS::GPRS(HardwareSerial *modemPort, user * users, GPS *gps, RFID *rfid, LCD *lcdin)
	{
		modempin = modemPort;
                userdata = users;
                gpsdata = gps;
                rfiddata = rfid;
                lcd = lcdin;
		waiting = OFF;
		count =0;
                ATindex = 0;
                runcount = 0;
                ok_variable = 0;
                ATsetupcommand[0] = "AT";
                ATsetupcommand[1] = "AT+IPR=19200";
                ATsetupcommand[2] = "AT+CMEE=2";
                ATsetupcommand[3] = "AT+CREG?";
                ATsetupcommand[4] = "AT+FLO=0";
                ATsetupcommand[5] = "AT+CGDCONT=1,\"IP\",\"airtelgprs.com\"";
                ATsetupcommand[6] = "AT#GPRS=1";
                //ATsendcommand[0] = "AT#SKTD=0,80,\"www.utkarshsins.com\",255,0";
                ATsendcommand[0] = "AT#SD=2,0,80,\"www.boozepanda.com\"";
                ATsendcommand[1] = "POST /terms/priority/listener.php HTTP/1.1\r\nHOST: www.boozepanda.com\r\nUser-Agent: HTTPTool/1.1\r\nContent-Type: application/x-www-form-urlencoded";
                //ATsendcommand[2] = "";
		//ATsendcommand[3] = "AT#SH=2";
	}

void GPRS::begin()
{
  modempin->begin(19200);
}
void GPRS::reset()
{
	ATindex=0;
	waiting=OFF;
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
                        //Serial.println("Modem is available");
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
                
                if(gprsstate == sendstate && ATindex  == 2)
                {
                   if((checkstring.indexOf("OK")!=-1 || checkstring.indexOf("NO CARRIER")!=-1) && ok_variable == 0)
                   {
                            ok_variable++;
                            if(checkstring.indexOf("OK")!=-1)
                            {
                              checkstring = checkstring.substring(checkstring.indexOf("OK") +2);
                            }
                            else if(checkstring.indexOf("NO CARRIER")!=-1)
                            {
                              checkstring = checkstring.substring(checkstring.indexOf("NO CARRIER") +10);
                            }
                   }          
                }
                
                if(checkstring.indexOf("\r\n")!=-1)
                {
                  // Serial.println("Found : " + checkstring);
                  #ifdef DEBUG_TRIMMING
                  Serial.println(checkstring.substring(0,checkstring.indexOf("\r\n")) + "\\r\\n" + checkstring.substring(checkstring.indexOf("\r\n")+2));
                  #endif
                  String checkconds = checkstring.substring(0,checkstring.indexOf("\r\n"));
                  if(checkconds.indexOf("NO") == -1 && checkconds.indexOf("ERROR") == -1 && checkconds.indexOf("OK") == -1 && checkconds.indexOf("CONNECT") == -1 && checkconds.indexOf("activated") == -1 && checkconds.indexOf("failed") == -1&& checkconds.indexOf("resolve DN") == -1&& checkconds.indexOf("inserted") == -1)
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
                                                        if(ok_variable == 1)
                                                        {
                                                          checkstring = checkstring.substring(checkstring.indexOf("NO CARRIER") +10);
                                                          return 1;
                                                        }
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
                else if(checkstring.indexOf("ERROR\r\n") != -1)
        				{
        						#ifdef DEBUG_TERMINAL_RETURNING
        						Serial.println("[TERMINAL] Returning : 0");
        						#endif
                                                        
        							#ifdef DEBUG_TRIMMING
        							Serial.println("Trimming : " + checkstring + "\t=>\t" + checkstring.substring(checkstring.indexOf("\r\n")+2));
        							Serial.println("Trimming End");
        							#endif
        							checkstring = checkstring.substring(checkstring.indexOf("\r\n")+2);
						return 0;
                }
		else if(checkstring.indexOf("OK\r\n") != -1 && !(gprsstate== sendstate && ATindex ==1))
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
                else if(checkstring.indexOf("activated\r\n") != -1)
                {
                  Serial.println("................. in the loop... already activated.......................");
                     //if(checkstring.indexOf("\r\n") != -1)
                     //{
                       checkstring = checkstring.substring(checkstring.indexOf("activated")+11);
                     //}
                      return 4;
                }
                else if(checkstring.indexOf("failed\r\n") != -1)
                {
                  Serial.println("................ in the loop ......failed .......... ");
                       checkstring = checkstring.substring(checkstring.indexOf("failed")+8);
                      return 5;
                }
                else if(checkstring.indexOf("resolve DN\r\n") != -1)
                {
                      Serial.println("................ in the loop ......resolve DN .......... ");
                      checkstring = checkstring.substring(checkstring.indexOf("resolve DN")+12);
                      return 6;
                }
                else if(checkstring.indexOf("inserted\r\n") != -1)
                {
                   checkstring = checkstring.substring(checkstring.indexOf("inserted")+8);
                   return 7;
                }
                else
                {
                    if(checkstring.indexOf("Server Error")!=-1)
                    {
                        if(checkstring.indexOf("</html>")!=-1)
                        {
                            checkstring = checkstring.substring(checkstring.indexOf("</html>")+7);
                            return 1;
                        }
                    }
                    
                    if(checkstring.indexOf("Not Found")!=-1)
                    {
                        if(checkstring.indexOf("</html>")!=-1)
                        {
                            checkstring = checkstring.substring(checkstring.indexOf("</html>")+7);
                            return 1;
                        }
                    }
                    
                    if(checkstring.indexOf("Not Implemented")!=-1)
                    {
                        if(checkstring.indexOf("</html>")!=-1)
                        {
                            checkstring = checkstring.substring(checkstring.indexOf("</html>")+7);
                            return 1;
                        }
                    }
                     
                    if(checkstring.indexOf("[") != -1)
                    {
                      if(checkstring.indexOf("]") != -1)
                      {
                          checkstring = checkstring.substring(checkstring.indexOf("["),checkstring.indexOf("]")+1);
                          //long_parse(checkstring);
                          checkstring = checkstring.substring(checkstring.indexOf("]")+1);
                          return 1;
                      }
                    }
                        #ifdef DEBUG_TERMINAL_RETURNING
                        Serial.print("[TERMINAL] String : ");
                        Serial.println(checkstring);
                        Serial.println("[TERMINAL] Returning : 2");
                        #endif
			return 2;
                }
	}

void GPRS::long_parse(String FullString)
{
  String temp = FullString;
  String str;
  size_t pos1;
  size_t pos2;
  int count = 0;
  while(temp!= "]" && temp!="[]")
  {
     pos1 = temp.indexOf("{"); 
     pos2 = temp.indexOf("}");
     str = temp.substring(pos1+1,pos2-1);
     userdata[count++].parse(str);
     temp = temp.substring(pos2+1);
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
                                //Serial.println("GPRS modem pin is here");
				int variable = readterminal();
                                //Serial.print("variable = ");
                                //?Serial.println(variable);
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
                                        if(ATindex< setupcount-1)
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
                        if(ATindex<2)
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
                          lcd->invalidate();
                          lcd->display(3);
                          requestModem("AT#SH=2", 100);
                          //gprsstate=waitingtosendstate;
                        }
                        if(ATindex==1)
                        {
                          ok_variable = 0;
                          cycletimestart = millis();
                        }
                          // TODO: ESCAPE SEQUENCE + TERMINATION AT COMMAND
                          // @Akshay - Put this in the state machine 
                          // Replace the delays with timeouts
			   //gprsstate=waitingtosendstate;
                        
                        
			waiting = ON;
                        if(ATindex == 1)
                        {
			    Serial.println("sending request ...");
                            String temp_string;
                            int k,j,DATA_LENGTH = 0, temp_length = 0;
                            
                             for(k=0;k<N_GPS;k++) 
                             {
                            //   Serial.print("k = ");
                            //   Serial.println(k);
                               myPosition *temp = gpsdata->positions(k);
                               temp_string = "&latitude"+String(k, DEC)+"="+String((long)(temp->lat), DEC)+"&longitude"+String(k, DEC)+"="+String((long)(temp->lon), DEC)+"&date"+String(k, DEC)+"="+String((long)(temp->date), DEC)+"&gpstime"+String(k, DEC)+"="+String((long)(temp->time), DEC) + "&busid"+String(k, DEC)+"=2";
                                temp_length += temp_string.length();
                             }

                             
                             for(j=0;j<N_RFID;j++) 
                             {
                               int count = 1;
                               if(rfiddata[j].to_send == true){
                               temp_string = "uid"+String(count++, DEC)+"="+String(rfiddata[j].uid, DEC)+"&rfidtime"+String(count++, DEC)+"="+String(rfiddata[j].time, DEC);
                               temp_length += temp_string.length();
                               }
                             }
                              if((gpsdata->positions(0))->lat!=0)
                              DATA_LENGTH = temp_length;
                             {
                                  String lengthed = "Content-Length: " + String(DATA_LENGTH, DEC) + "\r\n\r\n";
                                  char buff[lengthed.length()+1];
                                  lengthed.toCharArray(buff,lengthed.length()+1);
                                  modempin->print(buff);
                                  Serial.print(buff);
                             }
                      if((gpsdata->positions(0))->lat!=0)
                      {       
                            for(k=0;k<N_GPS;k++) 
                             {
                            //   Serial.print("k = ");
                            //   Serial.println(k);
                               myPosition *temp = gpsdata->positions(k);
                               temp_string = "&latitude"+String(k, DEC)+"="+String((long)(temp->lat), DEC)+"&longitude"+String(k, DEC)+"="+String((long)(temp->lon), DEC)+"&date"+String(k, DEC)+"="+String((long)(temp->date), DEC)+"&gpstime"+String(k, DEC)+"="+String((long)(temp->time), DEC) + "&busid"+String(k, DEC)+"=2";
                                char buff[temp_string.length()+1];
                                temp_string.toCharArray(buff,temp_string.length()+1);
                                modempin->print(buff);
                                Serial.print(buff);
                             }
                         //   Serial.println(string);
                          //  Serial.println(string.length());
                            for(j=0;j<N_RFID;j++) 
                             {
                               int count = 0;
                               if(rfiddata[j].to_send == true){
                               temp_string = "uid"+String(count, DEC)+"="+String(rfiddata[j].uid, DEC)+"&rfidtime"+String(count++, DEC)+"="+String(rfiddata[j].time, DEC);
                                char buff[temp_string.length()+1];
                                temp_string.toCharArray(buff,temp_string.length()+1);
                                modempin->print(buff);
                                Serial.println(buff);
                               }
                             }
                      }
                          //  Serial.println(string);
                          //  Serial.println(string.length());
  
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
                                    ATindex++;
                                }
				else if(variable==0 || variable == 6)
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
                                  if(ATindex<sendcount-1)
					ATindex++;
                                  else{
                                  ATindex=0;
                                  gprsstate = waitingtosendstate;
                                  }
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


