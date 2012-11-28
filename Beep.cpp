#include "Beep.h"

Beep::Beep(int beep_pin)
{
  pin = beep_pin;
}

void Beep::begin()
{
  pinMode(pin, OUTPUT);
  pinMode(13, OUTPUT);
}

void Beep::setBeep(int beep_type)
{
  if(beep_type == PRIORITY_BEEP)
  {
    priority_beep = true;
    priority_start = millis();
    priority_end = priority_start + PRIORITY_BEEP_PERIOD;
  }
  else if(beep_type == NON_PRIORITY_BEEP)
  {
    non_priority_beep = true;
    non_priority_start = millis();
    non_priority_end = non_priority_start + NON_PRIORITY_BEEP_PERIOD;
  }
}

void Beep::execute()
{
  long time = millis();
  if(priority_beep)
  {
    if(time > priority_start && time < priority_end)
    {
      digitalWrite(pin, HIGH);
      digitalWrite(13, HIGH);
    } else if (time > priority_end)
    {
      priority_beep = false;
      if(!(non_priority_beep && time < non_priority_end))
      {
        digitalWrite(13, LOW);
        digitalWrite(pin, LOW);
      }
    }
  } else if(non_priority_beep)
  {
    if(time > non_priority_start && time < non_priority_end)
    {
      digitalWrite(pin, HIGH);
      digitalWrite(13, HIGH);
    } else if (time > non_priority_end)
    {
      non_priority_beep = false;
      if(!(priority_beep && time < priority_end))
      {
        digitalWrite(13, LOW);
        digitalWrite(pin, LOW);
      }
    }
  }
}
