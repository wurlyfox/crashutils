#ifndef MODEL_WINDOW_NEW_H
#define MODEL_WINDOW_NEW_H

#include "win32/mdi_child_window.h"

#include "win32/controls/context_win.h"

#include "zone_window.h"

//#include "../../formats/crash/eid.h"

#define IDC_RENDER              101
#define IDC_ZONEWINDOW          102

class model_window : public mdi_child_window
{
  private:
  
  context_win *mainContext;
  //HWND temp[6];
  
  public:
   
  model_window()
  {
    className = "MODEL";
  }
  
  LRESULT onCreate(UINT uMsg, WPARAM wParam, LPARAM lParam);
  
  LRESULT onOther(UINT uMsg, WPARAM wParam, LPARAM lParam)
  {
    if ((uMsg >= WM_APP) && (uMsg < 0xC000))
    {
      param *params = (param*)lParam;
      param lparam = lParam ? params[0] : 0;
      param rparam = lParam ? params[1] : 0;
      
      switch (uMsg)
      {      
        case MSG_LOAD_LEVEL: onLoadLevel(); break;
        
        case MSG_SELECT_SVTX: onSelectModel((entry*)lparam, (int)rparam); break;
        case MSG_SELECT_WGEO: onSelectModel((entry*)lparam); break;
        case MSG_SELECT_ZONE: onSelectZone((entry*)lparam); break;
        //case MSG_SELECT_SECTION: onSelectSection((unsigned char*)lparam); break;
        //case MSG_SELECT_ENTITY: onSelectEntity((unsigned char*)lparam); break;

        //default: onExternal((int)uMsg, lparam, rparam); break;
      }
    } 
  }
  
  void onLoadLevel();
  void onSelectZone(entry *zone);
  void onSelectModel(entry *tgeo, int frame);
  void onSelectModel(entry *wgeo);
  
	/*
  void viewModelEntry(entry *tgeo, int frame);
  void viewModelEntry(entry *wgeo);
  void viewZoneEntry(entry *zdat);
  void viewLevel();
	*/
};               



#endif