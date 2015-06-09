#ifndef WINDOW_H
#define WINDOW_H

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include <stdio.h>

class window
{
  protected:
  
  HINSTANCE hInstance;
  HWND hParent;
  
  public:
  
  HWND hSelf;  //:(
   
  window *parent;
  window *children;
  window *sibling;
  
  window()
  {
    hInstance = NULL;
    hSelf = NULL;
    hParent = NULL;
    
    parent = 0;
    children = 0;
    sibling = 0;
  }
  virtual ~window();
  
  virtual void init(HINSTANCE _hInstance)
  {
    hInstance = _hInstance;
  }
  
  virtual void create() {};
  virtual void destroy() {};
  
  //virtual LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {};

  virtual void GetClientRect(RECT *rc)
  {
    ::GetClientRect(hSelf, rc);
  }
  
  window *addChild(window *child);
  window *getChild(int index);
  window *getChild(HWND hwnd);
};

#endif

