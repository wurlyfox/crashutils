#ifndef MDI_WINDOW_H
#define MDI_WINDOW_H

#include "basic_window.h"

class mdi_window : public basic_window
{
  protected:
  
  HWND hFrame;
  bool keys[0x100] = {0};

  virtual bool registerClass(const char *_className);

  LRESULT DefProc(HWND hWnd, INT uMsg, WPARAM wParam, LPARAM lParam)
  {  
    return DefFrameProc(hWnd, hSelf, uMsg, wParam, lParam);
  }
  
  public:
  
  virtual void SetWindowPos(int x, int y, int width, int height)
  {
    ::SetWindowPos(hFrame, NULL, x, y, width, height, SWP_SHOWWINDOW);
  }
  
  virtual void ShowWindow(int nCmdShow)
  {
    ::ShowWindow(hFrame, nCmdShow);
  }
  
  virtual void UpdateWindow()
  {
    ::UpdateWindow(hFrame);
  }
  
  virtual void create(const char* title, int x, int y, int width, int height);
  virtual void destroy();

  virtual void SendMessage(INT uMsg, WPARAM wParam, LPARAM lParam)
  {
    ::SendMessage(hFrame, uMsg, wParam, lParam);
  }

  bool TranslateMDISysAccel(LPMSG msg)
  {
    return ::TranslateMDISysAccel(hSelf, msg);
  }
  
  WPARAM messageLoop();
  virtual void dispatchIteration() {};
};

#endif