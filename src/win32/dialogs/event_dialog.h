#ifndef EVENT_DIALOG_H
#define EVENT_DIALOG_H

#include "../src/dialog.h"
#include "../../crash/crash_system.h"

#define ID_TEXT           200
#define ID_EVENTBOX       201
#define ID_EVENTSENDER    202
#define ID_EVENTRECIPIENT 203
#define ID_EVENTARG1      204
#define ID_EVENTARG2      205
#define ID_EVENTARG3      206
#define ID_EVENTARG4      207
#define ID_RADIOARG1      208
#define ID_RADIOARG2      209
#define ID_RADIOARG3      210
#define ID_RADIOARG4      211
#define ID_RADIOARGN      212

class event_dialog : public dialog
{
  private:
  
  userEvent *event;
  
  public:
  
  virtual void init(HINSTANCE _hInstance, HWND _hParent, userEvent *_event)
  {
    event = _event;
    
    event->count = 0;
  }
  
  virtual LPCDLGTEMPLATE DlgTemplate(void *mem);

  virtual BOOL onInitDialog(INT uMsg, WPARAM wParam, LPARAM lParam);
  virtual BOOL     onCreate(INT uMsg, WPARAM wParam, LPARAM lParam);
  virtual BOOL    onCommand(INT uMsg, WPARAM wParam, LPARAM lParam);

};

#endif