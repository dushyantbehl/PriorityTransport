#ifndef __USER_H__
#define __USER_H__

#include <WString.h>

#define USER_STATUS_NOTENTER -1
#define USER_STATUS_INVAN 0
#define USER_STATUS_DROPPED 1

class user
{
    public:
      user();
      void parse(String);
    
    private:
      long scheduled_pick_time;
      long scheduled_drop_time;
      long actual_pick_time;
      long actual_drop_time;
      String pick_location;
      String drop_location;
      String entry_num;
      int rfid_tag;
      int user_status;
      bool rfidsend;
};

#endif __USER_H__
