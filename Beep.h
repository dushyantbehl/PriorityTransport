#ifndef __BEEP_H__
#define __BEEP_H__

#include "Arduino.h"

#define PRIORITY_BEEP 0
#define NON_PRIORITY_BEEP 1

#define PRIORITY_BEEP_PERIOD 500
#define NON_PRIORITY_BEEP_PERIOD 1000

class Beep
{
  public:
   Beep(int);
   void begin();
   void setBeep(int);
   void execute();
   
  private:
    int pin;
    long priority_start, priority_end;
    long non_priority_start, non_priority_end;    
    boolean priority_beep;
    boolean non_priority_beep;
};
  
#endif
