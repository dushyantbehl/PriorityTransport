       
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

	case 0:
	//return(39);
	return(50);
	break;
	
      case 1:
	//return(22);
	return(34);
	break;
	
	case 2:
	//return(38);
	return(51);
	break;
	
//	case 3:
//	//return(23);
//	return(37);
//	break;
//				
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
  int print_index=0;
  while(index<no_of_lcd)
    {
     // Serial.print("[AKANKSHA] ");
     // Serial.println(user_aka[index].user_status);
            if(user_aka[index].user_status < 1)
              displayuserdata(user_aka[index].entry_num, user_aka[index].scheduled_pick_time, user_aka[index].rfid_tag, user_aka[index].pick_location, user_aka[index].drop_location,print_index++,user_aka[index].user_status);
      	  index++;
    }
          redisplay = false;
  }
}


String adjusted_time(long time)
{
if(time<1000)
  {
    if(time<100)
      {
        if(time<10)
        return("000"+String(time,DEC));
        else
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

void LCD::displayuserdata(String id, String picktime , String rfid , String pickstop , String dropstop, int lcd_no, int userstatus) // displays the 
{
  int pin = lcd_pin(lcd_no);  
  
//  String _ptime , _dtime ;
  String _pstop , _dstop;
  
  
  //_ptime = adjusted_time(picktime);
    //_dtime = adjusted_time(droptime);
  
LiquidCrystal lcd(pin,pin-2,pin-4,pin-6,pin-8,pin-10,pin-12);

 lcd.begin(16,2);              // columns, rows.  use 16,2 for a 16x2 LCD, etc.
  lcd.clear();

 lcd.setCursor(0,0);          
  lcd.print(id);    
   // delay(200);
   
	if(userstatus == 0){
 	lcd.setCursor(8,0);          
  lcd.print("->");    }
    //delay(200); 
	
lcd.setCursor(11,0);           
  lcd.print(pickstop);    
   // delay(200);
      
	
  lcd.setCursor(0,1);          
  lcd.print(picktime);
    // delay(200);

 lcd.setCursor(6,1);           
  lcd.print(rfid);  
    //delay(200);
	
	lcd.setCursor(11,1);          
//  lcd.print(dropstop);    
    lcd.print(dropstop);    

    //delay(200);

}



