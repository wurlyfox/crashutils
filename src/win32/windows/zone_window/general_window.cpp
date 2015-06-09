#include "general_window.h"

general_window::~general_window()
{
  for (int lp=0; lp<5; lp++)
    delete checks[lp];
    
  for (int lp=0; lp<3; lp++)
  {
    delete location[lp];
    delete dimension[lp];
  }
}
    
LRESULT general_window::onCreate(UINT uMsg, WPARAM wParam, LPARAM lParam)
{                        
  createGroupBox(0, 20,  10, 360, 165, "Properties");
  
  createText(0, 30, 32, 120, 12, "Location: ");
  createText(0, 30, 62, 120, 12, "Dimensions: ");
  
  raw_group_createparams params = {   3,  1, 
                                    IDC_EDITLOCATIONX, 
                                    180, 30, 60, 20,
                                      2,  2, 2,  2,
                                      0,  0 };
                                    
  raw_control::createGroup(location, params, hSelf);
  for (int lp=0; lp<3; lp++)
    location[lp]->addSpin(IDC_SPINLOCATIONX+lp, -100000, 100000);

  params.baseID = IDC_EDITDIMENSIONW;
  params.y      = 60;
  
  raw_control::createGroup(dimension, params, hSelf);
  for (int lp=0; lp<3; lp++)
    dimension[lp]->addSpin(IDC_SPINDIMENSIONW+lp, 0, 10000);

  
  checks[0] = new check(  IDC_CHECKBOTTOMSOLID, 30, 95, 320, 12, hSelf,
                        "Bottom of zone != solid ground (causes death if fallen below)");
  checks[1] = new check(     IDC_CHECKTOPSOLID, 30, 110, 320, 12, hSelf,
                        "Top of zone = solid ceiling (top of zone = top of level)");
  checks[2] = new check(IDC_CHECKCONTAINSWATER, 30, 125, 320, 12, hSelf,
                        "Contains water (can cause drowning type death)");
  checks[3] = new check( IDC_CHECKNOSOLIDWALLS, 30, 140, 320, 12, hSelf, 
                        "Does not contain solid walls");
  checks[4] = new check(IDC_CHECKNOCHECKPOINTS, 30, 155, 320, 12, hSelf,
                        "Does not save checkpoint state");

  createGroupBox(0, 20, 180, 360, 160, "Statistics");
}

void general_window::onExternal(int msg, param lparam, param rparam)
{   
  if (msg == WSM_SELECT_WINDOW_GENERAL) { select(); }
}

void general_window::onSelectZone(entry *zone)
{
  readCrashZoneHeader(zone, header);
  readCrashZoneSpace(zone, space);
  
  location[0]->useValue(&space.X);
  location[1]->useValue(&space.Y);
  location[2]->useValue(&space.Z);
  
  dimension[0]->useValue((long*)&space.width);
  dimension[1]->useValue((long*)&space.height);
  dimension[2]->useValue((long*)&space.depth);
  
  checks[0]->useFlag(&header.flags, 2);
  checks[1]->useFlag(&header.flags, 18);
  checks[2]->useFlag(&header.flags, 3);
  checks[3]->useFlag(&header.flags, 21);
  checks[4]->useFlag(&header.flags, 14);
  
  //parent->mainScene->
  //location[0].setValue(
}



//  AUTOCHECKBOX    "", 0, 92, 17, 29, 8, 0, WS_EX_LEFT
//  CONTROL         IDB_BITMAP3, 0, WC_STATIC, SS_BITMAP, 107, 12, 21, 20, WS_EX_LEFT
//  AUTOCHECKBOX    "", 0, 142, 17, 29, 8, 0, WS_EX_LEFT
//  CONTROL         IDB_BITMAP5, 0, WC_STATIC, SS_BITMAP, 157, 12, 21, 20, WS_EX_LEFT
//  AUTOCHECKBOX    "", 0, 192, 17, 29, 8, 0, WS_EX_LEFT
//  CONTROL         IDB_BITMAP4, 0, WC_STATIC, SS_BITMAP, 207, 12, 21, 20, WS_EX_LEFT