#ifndef CAMERA_PATH_WINDOW_H
#define CAMERA_PATH_WINDOW_H

#include "zone_prop_window.h"

#include "win32/raw_controls/edit.h"
#include "win32/raw_controls/drop.h"

#include "crash/entry.h"
#include "crash/zone.h"

#define IDC_CAMERAMODEDROP     101
#define IDC_EDITPOINTDIST      102
#define IDC_SPINPOINTDIST      103
#define IDC_EDITENTRANCEPOINT  104
#define IDC_SPINENTRANCEPOINT  105
#define IDC_EDITEXITPOINT      106
#define IDC_SPINEXITPOINT      107
#define IDC_EDITCAMERAVECTORX  108
#define IDC_EDITCAMERAVECTORY  109
#define IDC_EDITCAMERAVECTORZ  110
#define IDC_SPINCAMERAVECTORX  111
#define IDC_SPINCAMERAVECTORY  112
#define IDC_SPINCAMERAVECTORZ  113
#define IDC_EDITZOOMBACK       114
#define IDC_SPINZOOMBACK       115

class camera_path_window : public zone_prop_window
{
  private:
  
  NSF *nsf;
  NSD *nsd;
  
  crash_zone_section section;
  
  drop *cameraMode;
  HWND pointCount;
  edit *pointDist;
  HWND pathLength;
  edit *entrancePoint;
  edit *exitPoint;
  edit *cameraVector[3];
  edit *zoomBack;
  
  void messageRouter(UINT uMsg, WPARAM wParam, LPARAM lParam);
  
  public:
   
  camera_path_window() : zone_prop_window()
  {
    className = "CAMERA_PATH";
  }
  ~camera_path_window() {};
   
  LRESULT onCreate(UINT uMsg, WPARAM wParam, LPARAM lParam);
  LRESULT onNotify(UINT uMsg, WPARAM wParam, LPARAM lParam);
  LRESULT onCommand(UINT uMsg, WPARAM wParam, LPARAM lParam);

  void onExternal(int msg, param lparam, param rparam);
  void onSelectSection(unsigned char *sectionData);
};               

#endif