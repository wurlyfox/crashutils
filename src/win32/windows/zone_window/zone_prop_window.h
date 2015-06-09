#ifndef ZONE_PROP_WINDOW_H
#define ZONE_PROP_WINDOW_H

#include "win32/basic_control.h"

#include "crash/entry.h"
#include "../../../message.h"

typedef unsigned long param;

class zone_prop_window : public basic_control
{
  private:
  
  public:
  
  zone_prop_window() : basic_control() {};
  
  virtual LRESULT onOther(UINT uMsg, WPARAM wParam, LPARAM lParam)
  {
    if ((uMsg >= WM_APP) && (uMsg < 0xC000))
    {
      param *params = (param*)lParam;
      param lparam = lParam ? params[0] : 0;
      param rparam = lParam ? params[1] : 0;
      
      switch (uMsg)
      {      
        case MSG_SELECT_ZONE: onSelectZone((entry*)lparam); break;
        case MSG_SELECT_SECTION: onSelectSection((unsigned char*)lparam); break;
        case MSG_SELECT_ENTITY: onSelectEntity((unsigned char*)lparam); break;
        default: onExternal((int)uMsg, lparam, rparam); break;
      }
    } 
  }
  
  virtual void select() 
  {
    basic_window *sibling = (basic_window*)parent->children;
    sibling = (basic_window*)sibling->sibling; //skip overhead context window
    while (sibling)
    {
      if (sibling == this)
        sibling->ShowWindow(SW_SHOW);
      else
        sibling->ShowWindow(SW_HIDE);
        
      sibling = (basic_window*)sibling->sibling;
    }
  }
  
  virtual void onSelectZone(entry *zone) {};
  virtual void onSelectSection(unsigned char *sectionData) {};
  virtual void onSelectEntity(unsigned char* entityData) {};
  
  virtual void onExternal(int msg, param lparam, param rparam) {};
};

#endif