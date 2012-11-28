 
#ifndef LCD_h
#define LCD_h

#define wait_time 10

#include <WString.h>
#include <LiquidCrystal.h>

#include "Arduino.h"
#include "User.h"


class LCD
{
  public:
    LCD(user *people);	
//    void invalidate();
    void display(int no_of_lcd);   
    void invalidate();
    //void deluser(int id);
  private:
  boolean redisplay;
    //int _pin;
  void displayuserdata_2to5(String id, int picktime , int droptime , String pickstop , String dropstop, int lcd_no);
 // void displayuserdata_1(String id, int picktime , int droptime , String pickstop , String dropstop);
	int lcd_pin(int lcd_no);
	user *user_aka;
	
};

#endif
