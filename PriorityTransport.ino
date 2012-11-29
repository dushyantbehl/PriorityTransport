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

// GM865
int onPin = 22;                      // pin to toggle the modem's on/off
char PIN[1] = "";                // replace this with your PIN
//Position position;                   // stores the GPS position
LCD lcd(users);
Adafruit_NFCShield_I2C nfc(IRQ, RESET, &beep, users, &lcd);
GPRS modem(&Serial2, users,&myGPS, rfid, &lcd);   // modem is connected to Serial3
char cmd;                            // command read from terminal
static long TIME_DELAY1 = 60000;     //Time Delay for GPRS thread

void setup(void) {
  Serial.begin(115200);
  beep.begin();
  nfc.begin(); // Setup NFC - PN532
  modem.begin();
  myGPS.begin();
  Serial.print("Setup Completed\n");
}

#define LOOP_THRESHOLD_RFID 2

void loop(void) {
  modem.run();
    beep.execute();
    nfc.perform();
    myGPS.gpsloop();
    lcd.display(3);
}
