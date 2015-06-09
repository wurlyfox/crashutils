#ifndef BASIC_CONTROL_H
#define BASIC_CONTROL_H

#include "basic_window.h"

class basic_control : public basic_window
{
  public:
  
  basic_control() : basic_window() {}
  virtual ~basic_control() {}
  
  LRESULT DefProc(HWND hWnd, INT uMsg, WPARAM wParam, LPARAM lParam)
  {
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }
  
  virtual void create(DWORD dwExStyle, int x, int y, int width, int height, int id)
  {
    basic_window::create("", x, y, width, height);
    
    hSelf = CreateWindowEx(dwExStyle, className, "", 
      WS_CHILD | WS_VISIBLE, 
      x, y, width, height, hParent, (HMENU)id, hInstance, this);
      
    if (!hSelf)
    {
      destroy();
      MessageBox(NULL, "Control Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
    }
  }
  
  virtual void SetFocus()
  {
    ::SetFocus(hSelf);
  }
  
  virtual LRESULT onCreate(UINT uMsg, WPARAM wParam, LPARAM lParam) {};
  virtual LRESULT onCommand(UINT uMsg, WPARAM wParam, LPARAM lParam) {};
  virtual LRESULT onNotify(UINT uMsg, WPARAM wParam, LPARAM lParam) {};
  virtual LRESULT onPaint(UINT uMsg, WPARAM wParam, LPARAM lParam) {};
  virtual LRESULT onClick(UINT uMsg, WPARAM wParam, LPARAM lParam) {};
  virtual LRESULT onRClick(UINT uMsg, WPARAM wParam, LPARAM lParam) {};
  virtual LRESULT onMouse(UINT uMsg, WPARAM wParam, LPARAM lParam) {};  
  virtual LRESULT onDestroy(UINT uMsg, WPARAM waram, LPARAM lParam) {};
  
  virtual LRESULT onOther(UINT uMsg, WPARAM wParam, LPARAM lParam) {};
	
	virtual void ShowSelfHideSiblings()
	{
	  ShowWindow(SW_SHOW);
	
    if (parent)
    {
      window *nextSibling = parent->children;
      while (nextSibling)
      {
        if (nextSibling != this)
          ::ShowWindow(nextSibling->hSelf, SW_HIDE);
      
        nextSibling = nextSibling->sibling;
      }
    }
  }
};

#endif