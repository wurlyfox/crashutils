#ifndef GENERAL_WINDOW_H
#define GENERAL_WINDOW_H

#include "zone_prop_window.h"

#include "win32/raw_controls/edit.h"
#include "win32/raw_controls/check.h"

#include "crash/entry.h"
#include "crash/zone.h"

#define IDC_EDITLOCATIONX      101
#define IDC_EDITLOCATIONY      102
#define IDC_EDITLOCATIONZ      103
#define IDC_EDITDIMENSIONW     104
#define IDC_EDITDIMENSIONH     105
#define IDC_EDITDIMENSIOND     106
#define IDC_SPINLOCATIONX      107
#define IDC_SPINLOCATIONY      108
#define IDC_SPINLOCATIONZ      109
#define IDC_SPINDIMENSIONW     110
#define IDC_SPINDIMENSIONH     111
#define IDC_SPINDIMENSIOND     112
#define IDC_CHECKBOTTOMSOLID   113
#define IDC_CHECKTOPSOLID      114
#define IDC_CHECKCONTAINSWATER 115
#define IDC_CHECKNOSOLIDWALLS  116
#define IDC_CHECKNOCHECKPOINTS 117

class general_window : public zone_prop_window
{
  private:
  
  NSF *nsf;
  NSD *nsd;
  
  crash_zone_header header;
  crash_zone_space space;
  
  check *checks[5];
  edit *location[3];
  edit *dimension[3];
  
  public:
   
  general_window() : zone_prop_window()
  {
    className = "GENERAL";
  }
  ~general_window();
   
  LRESULT onCreate(UINT uMsg, WPARAM wParam, LPARAM lParam);

  void SetNSFNSD(NSF *_nsf, NSD *_nsd) { nsf = _nsf; nsd = _nsd; }  
  
  void onExternal(int msg, param lparam, param rparam);
  void onSelectZone(entry *zone);
};               

#endif