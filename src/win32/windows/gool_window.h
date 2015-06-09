#ifndef GOOL_WINDOW_H
#define GOOL_WINDOW_H

#include "win32/mdi_child_window.h"

#include "win32/raw_controls/list.h"
#include "win32/raw_controls/textedit.h"
#include "win32/raw_controls/hexedit.h"

#include "../raw_controls/proplist.h"
#include "../raw_controls/entrylist.h"

#include "../../message.h"

#include "../../crash/gooldisasm/gooldisasm.h"

#define ID_GOOL_EXECEVENT 101
#define ID_GOOL_EXECSUB   102

#define IDC_FUNCTIONLIST 103
#define IDC_CODEEDIT     104
#define IDC_SUBIDLIST    105
#define IDC_EVENTIDLIST  106
#define IDC_CODEDATA     107

class gool_window : public mdi_child_window
{
  private:
  
  //controls
  propertyList *functionList;
  list *subIDlist;
  list *eventIDlist;
  hexEdit *codeData;
  
  //menus
  HMENU hEventMenu;
  HMENU hSubMenu;
  
  //gool entry and disasm
  entry *currentGool;
  goolDisasm *disassembler;
  
  public:
    
  gool_window()
  {
    className = "GOOL";
  }
  
  textEdit *codeEdit;

  LRESULT  onCreate(UINT uMsg, WPARAM wParam, LPARAM lParam);
  LRESULT  onNotify(UINT uMsg, WPARAM wParam, LPARAM lParam);
  LRESULT onCommand(UINT uMsg, WPARAM wParam, LPARAM lParam);
  LRESULT   onPaint(UINT uMsg, WPARAM wParam, LPARAM lParam);
  LRESULT   onOther(UINT uMsg, WPARAM wParam, LPARAM lParam);
  
  void onSelectGool(entry *gool);
};

#endif