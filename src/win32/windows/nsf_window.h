#ifndef NSF_WINDOW_H
#define NSF_WINDOW_H

#include "win32/mdi_child_window.h"

#include "crash/nsd.h"
#include "crash/nsf.h"

#include "../raw_controls/entrylist.h"

#include "../../message.h"

#define ID_SEPARATOR               101
#define IDC_MAINTREE               102
#define IDC_CHUNKTYPEEDIT          103
#define IDC_CHUNKENTRYCOUNTEDIT    104
#define IDC_CHUNKCHECKSUMEDIT      105
#define IDC_ENTRYTYPEEDIT          106
#define IDC_ENTRYITEMCOUNTEDIT     107
#define IDC_UNUSEDEDIT             108
#define IDC_GLOBALTREE             109
#define IDC_LOOKUPTREE             110
#define ID_MODEL_VIEW              111
#define ID_GOOL_VIEW               112
#define ID_MODEL_EXPORT_AS_MD3     120
#define ID_MODEL_EXPORT_AS_COLLADA 121

class nsf_window : public mdi_child_window
{
  private:
  
  NSF *nsf;
  NSD *nsd;
  
  //controls
  entryList *mainList;
  entryList *globalList;
  entryList *lookupList;

  //menus
  HMENU hCodeMenu;
  HMENU hModelMenuA;
  HMENU hModelMenuB;
  
  public:
  
  nsf_window()
  {
    className = "NSF";
  }
  
  LRESULT  onCreate(UINT uMsg, WPARAM wParam, LPARAM lParam);
  LRESULT  onNotify(UINT uMsg, WPARAM wParam, LPARAM lParam);
  LRESULT onCommand(UINT uMsg, WPARAM wParam, LPARAM lParam);
  
  void SetNSDNSF(NSD *_nsd, NSF *_nsf);
};

#endif