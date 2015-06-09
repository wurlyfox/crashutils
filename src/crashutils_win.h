#ifndef CRASHUTILS_WIN_H
#define CRASHUTILS_WIN_H

// WIN32 GUI enabled CrashUtils implementation

// here we instantiate the following:
// (class is specific to win atm)
// 
// 1) window   system
// 2) graphics system
// 3) include also a crash system since it can be displayed 

// 4) also provide all platform-dependent factory methods here

// systems are ultimately connected here; 

#include "crashutils.h"

#include "win_system.h"
#include "gfx_system.h"
#include "crash_system.h"
#include "console_system.h"

#include "context_wgl.h"
#include "win32/time.h"
#include "win32/thread.h"

class crashutils_win : public crashutils
{ 
  protected:
	
	void messageRouter(csystem *src, int msg, param lparam, param rparam);
	
  public:
  
  crashutils_win()
  {
    new console_system(this);

    new gfx_system(this);
    new win_system(this);
    new crash_system(this);
  }  
};

#endif