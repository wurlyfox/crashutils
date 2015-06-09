#ifndef WIN_SYSTEM_H
#define WIN_SYSTEM_H

#include "system.h"
#include "message.h"

#include "win32/windows/main_window.h"
#include "win32/windows/nsf_window.h"
#include "win32/windows/model_window.h"
#include "win32/windows/gool_window.h"
//#include "win32/windows/object_window.h"


class win_system : public csystem 
{
  private:
  
  main_window *mainWindow;
  
  void messageRouter(csystem *src, int msg, param lparam, param rparam);

	void onCreate();
	void onStart();
	void onInitNSD(NSF *nsf, NSD *nsd);
	
  public:
  
  win_system(csystem *par) 
  { 
    par->addChild(this);
    
    mainWindow = 0;
    postMessage(WSM_CREATE); 
  }
};

class main_window_com : public main_window
{
  private:
  
  csystem *parentSystem;
  csystem *srcSystem;
  
  public:
  
  main_window_com(csystem *p) : main_window()
  {
    parentSystem = p;
  }
  
  LRESULT onOther(UINT uMsg, WPARAM wParam, LPARAM lParam);
  
  void setSource(csystem *src) { srcSystem = src; }
  void clrSource()             { srcSystem =   0; }
};



#endif