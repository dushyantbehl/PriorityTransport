#include <LiquidCrystal.h>

//#include "TinyGPS.h"

#include <Wire.h>
#include <SoftwareSerial.h>

#include "Arduino.h"

#include "RFID.h"
#include "GPRS.h"
#include "LCD.h"

// PN5324
#define IRQ   (2)
#define RESET (3)

const int buzzerpin = 8;

GPS myGPS(&Serial1);

#define BEEP_PULSE_SHORT 125
#define BEEP_PULSE_LONG 400
boolean validate = true;

user users[3];
RFID rfid[10];

Beep beep(buzzerpin);
Adafruit_NFCShield_I2C nfc(IRQ, RESET, &beep);

// GM865
int onPin = 22;                      // pin to toggle the modem's on/off
char PIN[1] = "";                // replace this with your PIN
//Position position;                   // stores the GPS position
LCD lcd(users);
GPRS modem(&Serial2, users,&myGPS, rfid, &lcd);   // modem is connected to Serial3
char cmd;                            // command read from terminal
static long TIME_DELAY1 = 60000;     //Time Delay for GPRS thread

void setup(void) {
  users[0].pick_location = "SATPU";
  users[0].drop_location = "HIMAD";
  users[0].scheduled_pick_time = 2230;
  users[0].scheduled_drop_time = 2240;
  users[0].entry_num = "2010CS50293";
  Serial.begin(115200);
  beep.begin();
  nfc.begin(); // Setup NFC - PN532
  modem.begin();
  myGPS.begin();
  Serial.print("Setup Completed\n");
}

#define LOOP_THRESHOLD_RFID 2

void loop(void) {
  long time = millis();
  beep.execute();
  nfc.perform();
  time = millis() - time;
  if(time > LOOP_THRESHOLD_RFID)
  {
    Serial.print(" LOOP TIME [NFC] : ");
    Serial.println(time);
    delay(100);
  }
  myGPS.gpsloop();
  modem.run();
  //lcd.display(3);
}
