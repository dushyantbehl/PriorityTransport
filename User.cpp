#include "User.h"
#include <string>
using namespace std;

user::user()
{
  scheduled_pick_time = "";
  scheduled_drop_time =0;
  actual_pick_time = 0;
  actual_drop_time = 0;
  pick_location = "";
  drop_location = "";
  entry_num = "";
  rfid_tag = "";
  user_status = -1;
  rfidsend = false;
}

void user::parse(String SingleUserString)
{
  // @Saurabh =   FIXME
  //              It would be easier if you just pass string for one user into this 
  //              function and parse it for the current user. That's why I have 
  //              coded it like this
  
  size_t pos;
  String str = SingleUserString;
  String temp;
  char chr[str.length()+1];
  if(str.indexOf("pt")!=-1){
  pos = str.indexOf("=");  
  str = str.substring(pos+1);
  pos = str.indexOf(",");
  //temp = str.substring(0,pos-1);
  //temp.toCharArray(chr,temp.length()+1);
  scheduled_pick_time = str.substring(0,pos);
 }
 if(str.indexOf("pl")!=-1){
  pos = str.indexOf("=");  
  str = str.substring(pos+2);
  pos = str.indexOf(",");
  //temp = str.substring(0,pos)
  pick_location = str.substring(0,pos-1);
 }
 if(str.indexOf("dl")!=-1){
  pos = str.indexOf("=");  
  str = str.substring(pos+2);
  pos = str.indexOf(",");
  //temp = str.substring(0,pos)
  drop_location = str.substring(0,pos-1);
 }
 if(str.indexOf("en")!=-1){
  pos = str.indexOf("=");  
  str = str.substring(pos+2);
  pos = str.indexOf(",");
  //temp = str.substring(0,pos)
  if(pos-1>=7)
  entry_num = str.substring(0,7);
  else 
  entry_num = str.substring(0,pos-1);
 }
 if(str.indexOf("rt")!=-1){
  pos = str.indexOf("=");  
  str = str.substring(pos+1);
  pos = str.indexOf("}");
 // temp = str.substring(0,pos);
 // temp.toCharArray(chr,temp.length()+1);
  rfid_tag = str.substring(0,pos+1);
}
  user_status = -1;
}



