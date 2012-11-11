#include "User.h"
#include <string>
using namespace std;

user::user()
{
  scheduled_pick_time = 0;
  scheduled_drop_time = 0;
  actual_pick_time = 0;
  actual_drop_time = 0;
  pick_location = "";
  drop_location = "";
  entry_num = "";
  rfid_tag = 0;
  user_status = -1;
  rfidsend = false;
}

void user::parse(String SingleUserString)
{
  // @Saurabh :   FIXME
  //              It would be easier if you just pass string for one user into this 
  //              function and parse it for the current user. That's why I have 
  //              coded it like this
  
  size_t pos;
  String str = SingleUserString;
  String temp;
  char chr[str.length()+1];
  pos = str.indexOf(":");  
  str = str.substring(pos+3);
  pos = str.indexOf(",");
  temp = str.substring(0,pos);
  temp.toCharArray(chr,temp.length()+1);
  scheduled_pick_time = atol(chr);
  pos = str.indexOf(":");  
  str = str.substring(pos+3);
  pos = str.indexOf(",");
  temp = str.substring(0,pos);
  temp.toCharArray(chr,temp.length()+1);
  scheduled_drop_time = atol(chr);
  pos = str.indexOf(":");  
  str = str.substring(pos+3);
  pos = str.indexOf(",");
  temp = str.substring(0,pos);
  temp.toCharArray(chr,temp.length()+1);
  actual_pick_time = atol(chr);
  pos = str.indexOf(":");  
  str = str.substring(pos+3);
  pos = str.indexOf(",");
  temp = str.substring(0,pos);
  temp.toCharArray(chr,temp.length()+1);
  actual_drop_time = atol(chr);
  pos = str.indexOf(":");  
  str = str.substring(pos+3);
  pos = str.indexOf(",");
  //temp = str.substring(0,pos)
  pick_location = str.substring(0,pos);
  pos = str.indexOf(":");  
  str = str.substring(pos+3);
  pos = str.indexOf(",");
  //temp = str.substring(0,pos)
  drop_location = str.substring(0,pos);
  pos = str.indexOf(":");  
  str = str.substring(pos+3);
  pos = str.indexOf(",");
  //temp = str.substring(0,pos)
  entry_num = str.substring(0,pos);
  pos = str.indexOf(":");  
  str = str.substring(pos+3);
  pos = str.indexOf(",");
  temp = str.substring(0,pos);
  temp.toCharArray(chr,temp.length()+1);
  rfid_tag = atoi(chr);
  pos = str.indexOf(":");  
  str = str.substring(pos+3);
  pos = str.indexOf(",");
  temp = str.substring(0,pos);
  temp.toCharArray(chr,temp.length()+1);
  user_status = atoi(chr);
  pos = str.indexOf(":");  
  str = str.substring(pos+3);
  pos = str.indexOf(",");
 // temp = str.substring(0,pos)
  if(str.substring(0,pos)=="true")
  rfidsend = true;
  else
  rfidsend = false;
}


