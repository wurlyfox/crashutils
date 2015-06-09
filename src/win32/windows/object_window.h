#ifndef OBJECT_WINDOW_H
#define OBJECT_WINDOW_H

#include "win32/mdi_child_window.h"

#include "win32/raw_controls/hexedit.h"
#include "win32/raw_controls/textedit.h"
#include "../raw_controls/objectlist.h"
#include "win32/raw_controls/propertyGrid.h"
#include "../raw_controls/list.h"

#include "../../formats/crash/nsd.h"
#include "../../formats/crash/nsf.h"
#include "../../formats/crash/entry.h"
#include "../../formats/crash/eid.h"
#include "../../formats/crash/r3000a.h"

#include "../../crash/src/goolengine.h"
//#include "../../crash/threads.h"       //so the gool interpreter can be resumed

#define IDC_OBJPROPS   101
#define IDC_OBJMEM     103

class object_window : public mdi_child_window
{
  private:
  
  NSF *nsf;
  NSD *nsd;
  
  //controls
  HWND objectProps;
  hexEdit *objectMem;
 
  //members
  object *debugObject;
  char objectProperties[20][50];
  
  //methods
  void initObjectProperties();
  
  public:
    
  object_window()
  {
    className = "OBJECT";
  }
  
  LRESULT  onCreate(UINT uMsg, WPARAM wParam, LPARAM lParam);
  LRESULT   onPaint(UINT uMsg, WPARAM wParam, LPARAM lParam);
  LRESULT   onOther(UINT uMsg, WPARAM wParam, LPARAM lParam);
  
  void updateDebugInfo(object *obj);
  void updateObjectProperties(object *obj);
  
  void SetNSFNSD(NSF *_nsf, NSD *_nsd);  
};

#endif