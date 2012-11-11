#include "User.h"

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
  
  scheduled_pick_time = 0;
  scheduled_drop_time = 0;
  actual_pick_time = 0;
  actual_drop_time = 0;
  pick_location = "SATRA";
  drop_location = "INSTI";
  entry_num = "2010CS10283";
  rfid_tag = 42;
  user_status = -1;
  rfidsend = false;
}


