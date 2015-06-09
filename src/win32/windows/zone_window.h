#ifndef ZONE_WINDOW_H
#define ZONE_WINDOW_H

#include "win32/basic_control.h"
#include "win32/controls/context_win.h"

#include "crash/nsd.h"
#include "crash/nsf.h"
#include "crash/zone.h"

#include "../raw_controls/zonelist.h"

#include "zone_window/general_window.h"
#include "zone_window/entity_window.h"
#include "zone_window/shader_window.h"
#include "zone_window/camera_path_window.h"

// window controls
#define IDC_CONTEXT_OVERHEAD 101
#define IDC_ZONELIST         102
#define IDC_GENERALWINDOW    103
#define IDC_ENTITYWINDOW     110

// window menus
#define ID_SEPARATOR         201
#define ID_ENTITY_SPAWN      201
#define ID_ENTITY_PROPERTIES 202

class zone_window : public basic_control
{
  friend class camera_path_window;
  
  private:
  
  NSF *nsf;
  NSD *nsd;
  
  //menus
  HMENU hEntityMenu;
  
  //controls
  zoneList *mainList;

  public:
   
  zone_window() : basic_control()
  {
    className = "ZONE";
  }
  
  LRESULT onCreate(UINT uMsg, WPARAM wParam, LPARAM lParam);
  LRESULT onNotify(UINT uMsg, WPARAM wParam, LPARAM lParam);
  LRESULT onCommand(UINT uMsg, WPARAM wParam, LPARAM lParam);
  LRESULT onOther(UINT uMsg, WPARAM wParam, LPARAM lParam);
  
  void onSelectZone(entry *zone);
 
  entry *currentZone;
  unsigned char *currentSection;
  //unsigned long currentProgressV;
};               

#endif