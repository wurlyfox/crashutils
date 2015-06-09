#ifndef MDI_CHILD_WINDOW_H
#define MDI_CHILD_WINDOW_H

#include "basic_window.h"

class mdi_child_window : public basic_window
{
  protected:
  
  LRESULT DefProc(HWND hWnd, INT uMsg, WPARAM wParam, LPARAM lParam)
  {
    return DefMDIChildProc(hWnd, uMsg, wParam, lParam);
  }
  
  public:
  
  virtual void create(const char* title, int x, int y, int width, int height);
  virtual void destroy();
     
  virtual void SetFocus()
  {
    ::ShowWindow(hSelf, SW_SHOW);
    ::SendMessage(hParent, WM_MDIACTIVATE, (LPARAM)hSelf, 0);
  }
};

#endif