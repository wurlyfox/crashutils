#ifndef ENTITY_WINDOW_H
#define ENTITY_WINDOW_H

#include "zone_prop_window.h"

#include "crash/nsd.h"
#include "crash/nsf.h"
#include "crash/zone.h"

#include "../../raw_controls/entrydrop.h"

#define IDC_GOOLDROP  101
#define IDC_STATEDROP 102
#define IDC_GROUPDROP 103

class entity_window : public zone_prop_window
{
  private:
  
  crash_zone_entity entity;
  
  entryDrop *drop_type;
  drop *drop_subtype;
  drop *drop_group;
  HWND text_type;
  HWND text_subtype;
  HWND text_group;  
  HWND static_type;
  HWND static_subtype;
  HWND static_group;
  
  public:
   
  entity_window() : zone_prop_window()
  {
    className = "ENTITY";
  }
  
  LRESULT onCreate(UINT uMsg, WPARAM wParam, LPARAM lParam);

  void onExternal(int msg, param lparam, param rparam);
  void onInitNSD(NSF *nsf, NSD *nsd);
  
  void onSelectEntity(unsigned char *entityData);
};               

#endif