#ifndef SHADER_WINDOW_H
#define SHADER_WINDOW_H

#include "zone_prop_window.h"

#include "win32/controls/colorbox.h"
#include "win32/raw_controls/edit.h"
#include "win32/raw_controls/radio.h"

#include "crash/entry.h"
#include "crash/zone.h"

#define IDC_RADIONONE          101
#define IDC_RADIORIPPLE        102
#define IDC_RADIOLIGHTNING     103
#define IDC_RADIODEPTHCUEING1  104
#define IDC_RADIODEPTHCUEING2  105
#define IDC_RADIODEPTHCUEING3  106
#define IDC_EDITDEPTH          107
#define IDC_EDITCLEARHEIGHT    108
#define IDC_COLORBOXFARCOLOR   109
#define IDC_COLORBOXCLEARCOLOR 110
#define IDC_SPINDEPTH          111
#define IDC_SPINCLEARHEIGHT    112

class shader_window : public zone_prop_window
{
  private:
  
  crash_zone_header header;
  bool created;
  
  radio *effects[6];
  edit *depth;
  edit *clearHeight;
  colorbox *farColor;
  colorbox *clearColor;
  
  public:
   
  shader_window() : zone_prop_window()
  {
    className = "SHADER";
    created = false;
  }
  ~shader_window();
  
  LRESULT onCreate(UINT uMsg, WPARAM wParam, LPARAM lParam);
  LRESULT onNotify(UINT uMsg, WPARAM wParam, LPARAM lParam);
  LRESULT onCommand(UINT uMsg, WPARAM wParam, LPARAM lParam);
  
  void onExternal(int msg, param lparam, param rparam);
  void onSelectZone(entry *zone);
};               

#endif