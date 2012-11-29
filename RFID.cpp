#include "Arduino.h"

#include <Wire.h>

#include "RFID.h"

byte pn532ack[] = {0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00};
byte pn532response_firmwarevers[] = {0x00, 0xFF, 0x06, 0xFA, 0xD5, 0x03};

// Uncomment these lines to enable debug output for PN532(I2C) and/or MIFARE related code
// #define PN532DEBUG
// #define MIFAREDEBUG

#define PN532_PACKBUFFSIZ 64
byte pn532_packetbuffer[PN532_PACKBUFFSIZ];

// Constructor
Adafruit_NFCShield_I2C::Adafruit_NFCShield_I2C(uint8_t irq, uint8_t reset, Beep * beep, user * usersin, LCD * lcdin) {
  _irq = irq;
  _reset = reset;

  pinMode(_irq, INPUT);
  pinMode(_reset, OUTPUT);
  
  beeper = beep;
  state = STATE_INACTIVE;
  
  users = usersin;
  lcd = lcdin;
}

// Setups the HW
void Adafruit_NFCShield_I2C::begin() {
  Wire.begin();
  pinMode(buzzerpin, OUTPUT); 
  
  // Reset the PN532  
  digitalWrite(_reset, HIGH);
  digitalWrite(_reset, LOW);
  delay(400);
  digitalWrite(_reset, HIGH);
  
  uint32_t versiondata = getFirmwareVersion();
  if (! versiondata) 
  {
    // Raise Buzzer Exception
    Serial.println("Didn't find PN532");
    state = STATE_ERROR;
  }
  else
  {
    setPassiveActivationRetries(0xFF);  // Number of retry attempts 
    SAMConfig();                        // configure board to read RFID tags
    Serial.println("Started PN5432");
    state = STATE_ACTIVE_NOTWAITING;
  }
}
 
// Check Hardware
uint32_t Adafruit_NFCShield_I2C::getFirmwareVersion(void) {
  uint32_t response;

  pn532_packetbuffer[0] = PN532_COMMAND_GETFIRMWAREVERSION;

  if (! sendCommandCheckAck(pn532_packetbuffer, 1))
    return 0;
  wirereaddata(pn532_packetbuffer, 12);
  
  // check some basic stuff
  if (0 != strncmp((char *)pn532_packetbuffer, (char *)pn532response_firmwarevers, 6)) {
    #ifdef PN532DEBUG
    Serial.println("Firmware doesn't match!");
	#endif
    return 0;
  }
  
  response = pn532_packetbuffer[7];
  response <<= 8;
  response |= pn532_packetbuffer[8];
  response <<= 8;
  response |= pn532_packetbuffer[9];
  response <<= 8;
  response |= pn532_packetbuffer[10];

  return response;
}


boolean Adafruit_NFCShield_I2C::SAMConfig(void) {
  pn532_packetbuffer[0] = PN532_COMMAND_SAMCONFIGURATION;
  pn532_packetbuffer[1] = 0x01; // normal mode;
  pn532_packetbuffer[2] = 0x14; // timeout 50ms * 20 = 1 second
  pn532_packetbuffer[3] = 0x01; // use IRQ pin!
  
  if (! sendCommandCheckAck(pn532_packetbuffer, 4))
     return false;

  // read data packet
  wirereaddata(pn532_packetbuffer, 8);
  
  return  (pn532_packetbuffer[6] == 0x15);
}

boolean Adafruit_NFCShield_I2C::setPassiveActivationRetries(uint8_t maxRetries) {
  pn532_packetbuffer[0] = PN532_COMMAND_RFCONFIGURATION;
  pn532_packetbuffer[1] = 5;    // Config item 5 (MaxRetries)
  pn532_packetbuffer[2] = 0xFF; // MxRtyATR (default = 0xFF)
  pn532_packetbuffer[3] = 0x01; // MxRtyPSL (default = 0x01)
  pn532_packetbuffer[4] = maxRetries;

#ifdef MIFAREDEBUG
  Serial.print("Setting MxRtyPassiveActivation to "); Serial.print(maxRetries, DEC); Serial.println(" ");
#endif  
  if (! sendCommandCheckAck(pn532_packetbuffer, 5))
    return 0x0; // no ACK
  return 1;
}

/***** ISO14443A Commands ******/

/**************************************************************************/
/*! 
    Waits for an ISO14443A target to enter the field
    
    @param  cardBaudRate  Baud rate of the card
    @param  uid           Pointer to the array that will be populated
                          with the card's UID (up to 7 bytes)
    @param  uidLength     Pointer to the variable that will hold the
                          length of the card's UID.
    
    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/

void Adafruit_NFCShield_I2C::perform()
{
  if(state == STATE_ERROR || state == STATE_INACTIVE)
    return;
  else if(state == STATE_ACTIVE_WAITING)
  {
    if(((uint8_t) digitalRead(_irq)) == 1)
      return;
    else
    {
      // CARD FOUND - DO SOMETHING
      state = STATE_ACTIVE_NOTWAITING;
      Serial.println("Found Card");
      uint8_t cardbaudrate = PN532_MIFARE_ISO14443A;
      wirereaddata(pn532_packetbuffer, 64);  
      /* ISO14443A card response should be in the following format:
      byte            Description
      -------------   ------------------------------------------
      b0..6           Frame header and preamble
      b7              Tags Found
      b8              Tag Number (only one used in this example)
      b9..10          SENS_RES
      b11             SEL_RES
      b12             NFCID Length
      b13..NFCIDLen   NFCID                                      */
      Serial.print("Found "); Serial.print(pn532_packetbuffer[7], DEC); Serial.println(" tags");
      if (pn532_packetbuffer[7] != 1) 
        return;
      
      uint8_t uidLength = pn532_packetbuffer[12];
      uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
      
      Serial.print("UID:"); 
      for (uint8_t i=0; i < pn532_packetbuffer[12]; i++) 
      {
        uid[i] = pn532_packetbuffer[13+i];
//        Serial.print(" 0x");Serial.print(uid[i], HEX);
      }
      int uidval = uid[0]+uid[1]*8+uid[2]*16;
        Serial.println(uidval);
        
      
      for(int i =0; i<4; i++)
      {
        if(i==3)
        {
          Serial.println("FOUND NON_PRIORITY CUSTOMER");
          beeper->setBeep(NON_PRIORITY_BEEP);
          break;
        }
  
        if(users[i].rfid_tag == String(uidval,DEC))
        {
          Serial.println("FOUND PRIORITY CUSTOMER");
          users[i].rfidsend = true;
          users[i].user_status++;
          Serial.print("User_Status : ");
          Serial.print(users[i].user_status);
          lcd->invalidate();
          beeper->setBeep(PRIORITY_BEEP);
          break;
        }
      }
      
      pn532_packetbuffer[0] = PN532_COMMAND_INLISTPASSIVETARGET;
      pn532_packetbuffer[1] = 1;  // max 1 cards at once (we can set this to 2 later)
      pn532_packetbuffer[2] = cardbaudrate;

      if (!sendCommandCheckAck(pn532_packetbuffer, 3))
      {
        #ifdef PN532DEBUG
        Serial.println("No card(s) read");
        #endif
        return; // no cards read
      }
      state = STATE_ACTIVE_WAITING;
      return;
    } 
  }
  else if(state == STATE_ACTIVE_NOTWAITING)
  {
    uint8_t cardbaudrate = PN532_MIFARE_ISO14443A;  
    pn532_packetbuffer[0] = PN532_COMMAND_INLISTPASSIVETARGET;
    pn532_packetbuffer[1] = 1;  // max 1 cards at once (we can set this to 2 later)
    pn532_packetbuffer[2] = cardbaudrate;
  
    if (!sendCommandCheckAck(pn532_packetbuffer, 3))
    {
      #ifdef PN532DEBUG
      Serial.println("No card(s) read");
      #endif
      return; // no cards read
    }
    state = STATE_ACTIVE_WAITING;
    Serial.println("");
    return;
  }
}

// Recieve the acknowledgement : Otherwise won't work !!
boolean Adafruit_NFCShield_I2C::sendCommandCheckAck(uint8_t *cmd, uint8_t cmdlen, uint16_t timeout) {
  uint16_t timer = 0;
  
  // write the command
  wiresendcommand(cmd, cmdlen);
  
  // Wait for chip to say its ready!
  while (((uint8_t) digitalRead(_irq)) == 1) {
    if (timeout != 0) {
      timer+=10;
      if (timer > timeout)
        return false;
    }
    delay(10);
  }
  
  #ifdef PN532DEBUG
  Serial.println("IRQ received");
  #endif
  
  // read acknowledgement
  if (!readackframe()) {
    #ifdef PN532DEBUG
    Serial.println("No ACK frame received!");
    #endif
    return false;
  }

  return true; // ack'd command
}


/************** high level I2C */


/**************************************************************************/
/*! 
    @brief  Tries to read the PN532 ACK frame (not to be confused with 
	        the I2C ACK signal)
*/
/**************************************************************************/
boolean Adafruit_NFCShield_I2C::readackframe(void) {
  uint8_t ackbuff[6];
  
  wirereaddata(ackbuff, 6);
    
  return (0 == strncmp((char *)ackbuff, (char *)pn532ack, 6));
}

/**************************************************************************/
/*! 
    @brief  Reads n bytes of data from the PN532 via I2C

    @param  buff      Pointer to the buffer where data will be written
    @param  n         Number of bytes to be read
*/
/**************************************************************************/
void Adafruit_NFCShield_I2C::wirereaddata(uint8_t* buff, uint8_t n) {
  uint16_t timer = 0;
  
  // CHECKME :   delay(2); 

#ifdef PN532DEBUG
  Serial.print("Reading: ");
#endif
  // Start read (n+1 to take into account leading 0x01 with I2C)
  Wire.requestFrom((uint8_t)PN532_I2C_ADDRESS, (uint8_t)(n+2));
  // Discard the leading 0x01
  Wire.read();
  for (uint8_t i=0; i<n; i++) {
    delay(1);
    buff[i] = ((uint8_t) Wire.read());
#ifdef PN532DEBUG
    Serial.print(" 0x");
    Serial.print(buff[i], HEX);
#endif
  }
  // Discard trailing 0x00 0x00
  // Wire.read();
    
#ifdef PN532DEBUG
  Serial.println();
#endif
}

/**************************************************************************/
/*! 
    @brief  Writes a command to the PN532, automatically inserting the
            preamble and required frame details (checksum, len, etc.)

    @param  cmd       Pointer to the command buffer
    @param  cmdlen    Command length in bytes 
*/
/**************************************************************************/
void Adafruit_NFCShield_I2C::wiresendcommand(uint8_t* cmd, uint8_t cmdlen) {
  uint8_t checksum;

  cmdlen++;
  
#ifdef PN532DEBUG
  Serial.print("\nSending: ");
#endif

  // CHECKME :   delay(2);     // or whatever the delay is for waking up the board

  // I2C START
  Wire.beginTransmission(PN532_I2C_ADDRESS);
  checksum = PN532_PREAMBLE + PN532_PREAMBLE + PN532_STARTCODE2;
  Wire.write((uint8_t) PN532_PREAMBLE);
  Wire.write((uint8_t) PN532_PREAMBLE);
  Wire.write((uint8_t) PN532_STARTCODE2);

  Wire.write((uint8_t) cmdlen);
  Wire.write((uint8_t) ~cmdlen + 1);
 
  Wire.write((uint8_t) PN532_HOSTTOPN532);
  checksum += PN532_HOSTTOPN532;

  // Serial.println("Starting");
#ifdef PN532DEBUG
  Serial.print(" 0x"); Serial.print(PN532_PREAMBLE, HEX);
  Serial.print(" 0x"); Serial.print(PN532_PREAMBLE, HEX);
  Serial.print(" 0x"); Serial.print(PN532_STARTCODE2, HEX);
  Serial.print(" 0x"); Serial.print(cmdlen, HEX);
  Serial.print(" 0x"); Serial.print(~cmdlen + 1, HEX);
  Serial.print(" 0x"); Serial.print(PN532_HOSTTOPN532, HEX);
  Serial.println();
#endif
  // Serial.println("Starting");

  for (uint8_t i=0; i<cmdlen-1; i++) {
   Wire.write((uint8_t) cmd[i]);
   checksum += cmd[i];
#ifdef PN532DEBUG
   Serial.print(" 0x"); Serial.print(cmd[i], HEX);
   Serial.println();
#endif
  }
  
  // Serial.println("Starting endloop");
  Wire.write((uint8_t) ~checksum);
  Wire.write((uint8_t) PN532_POSTAMBLE);
  
  // Serial.println("Starting");
  // I2C STOP
  Wire.endTransmission();
#ifdef PN532DEBUG
  Serial.print(" 0x"); Serial.print(~checksum, HEX);
  Serial.print(" 0x"); Serial.print(PN532_POSTAMBLE, HEX);
  Serial.println();
#endif
} 

