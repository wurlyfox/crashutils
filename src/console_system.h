#ifndef CONSOLE_SYSTEM_H
#define CONSOLE_SYSTEM_H

#include "system.h"
#include "time.h"

#include "message.h"

class console_system : public csystem
{
  protected:
  
  time *timer = 0;
  
  void messageRouter(csystem *src, int msg, param lparam, param rparam);
  
  public:
  
  console_system(csystem *par)
  {
    par->addChild(this);
    
    timer = 0;
    postMessage(MSG_NEW_TIMER, (param)&timer);
    timer->init(60, 0.002f);
  }
};

#endif