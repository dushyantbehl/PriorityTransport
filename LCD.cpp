       
#include <LiquidCrystal.h> 
#include "Arduino.h"
#include "LCD.h"

LCD::LCD(user *people)
{
    user_aka = people;
    redisplay = false;
	// display(user_aka);
  }

int LCD::lcd_pin(int lcd_no)
{
switch(lcd_no)
	{
//	case 5:
//	return(2);
//	break;
	
	case 1:
	//return(22);
	return(34);
	break;
	
	case 2:
	//return(38);
	return(51);
	break;
	
	case 3:
	//return(23);
	return(37);
	break;
	
	case 0:
	//return(39);
	return(50);
	break;
			
	}
}

void LCD::invalidate()
{
  redisplay = true;
}

void LCD::display(int no_of_lcd)
{
  if(redisplay)
  {
  Serial.println("Inside the display loop");
  int index=0;
  //displayuserdata_1(user_aka[index].entry_num, user_aka[index].scheduled_pick_time, user_aka[index].scheduled_drop_time, user_aka[index].pick_location, user_aka[index].drop_location);
  
  while(index<no_of_lcd)
  	{
  	displayuserdata_2to5(user_aka[index].entry_num, user_aka[index].scheduled_pick_time, user_aka[index].scheduled_drop_time, user_aka[index].pick_location, user_aka[index].drop_location,index);
  	index++;
          }
          redisplay = false;
  }
}

//void LCD::displayuserdata_1(String id, int picktime , int droptime , String pickstop , String dropstop) // displays the 
//{
//
//LiquidCrystal lcd(9,8,7,6,5,4,3);
//
// lcd.begin(16,2);              // columns, rows.  use 16,2 for a 16x2 LCD, etc.
//  lcd.clear();
//
// lcd.setCursor(0,0);          
//  lcd.print(id);    
//    delay(200);
//	
//lcd.setCursor(12,0);           
//  lcd.print(pickstop);    
//    delay(200);
//	
///* 	lcd.setCursor(8,0);          
//  lcd.print(droptime);    
//    delay(200); */
//	
//  lcd.setCursor(0,1);          
//  lcd.print(picktime);
//     delay(200);
//
// lcd.setCursor(6,1);           
//  lcd.print(dropstop);    
//    delay(200);
//	
//	lcd.setCursor(11,1);          
//  lcd.print(droptime);    
//    delay(200);
//  
//}

String adjusted_time(long time)
{
if(time<1000)
  {
    if(time<100)
      {
      return("00"+String(time,DEC));
      }
    else
      {
     return("0"+String(time,DEC));
     }
  }
  else
  return(String(time,DEC));
}

void LCD::displayuserdata_2to5(String id, int picktime , int droptime , String pickstop , String dropstop, int lcd_no) // displays the 
{
  int pin = lcd_pin(lcd_no);  
  
  String _ptime , _dtime ;
  String _pstop , _dstop;
  
  
  _ptime = adjusted_time(picktime);
    _dtime = adjusted_time(droptime);
  
LiquidCrystal lcd(pin,pin-2,pin-4,pin-6,pin-8,pin-10,pin-12);

 lcd.begin(16,2);              // columns, rows.  use 16,2 for a 16x2 LCD, etc.
  lcd.clear();

 lcd.setCursor(0,0);          
  lcd.print(id);    
   // delay(200);
	
lcd.setCursor(12,0);           
  lcd.print(pickstop);    
   // delay(200);
	
/* 	lcd.setCursor(8,0);          
  lcd.print(droptime);    
    delay(200); */
	
  lcd.setCursor(0,1);          
  lcd.print(_ptime);
    // delay(200);

 lcd.setCursor(5,1);           
  lcd.print(_dtime);    
    //delay(200);
	
	lcd.setCursor(12,1);          
  lcd.print(dropstop);    
    //delay(200);

}



