#include "raw_controls/raw_control.h"

HFONT raw_control::defFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

raw_control::raw_control(int _ID, char *winClass, char *text, DWORD dwStyle, int x, int y, int w, int h, HWND _parent) 
{
  ID = _ID;
  parent = _parent;
  
  HINSTANCE hInstance = GetModuleHandle(NULL);
  
  hwnd = CreateWindowEx(WS_EX_LEFT, winClass, text, 
    WS_CHILD | WS_VISIBLE | dwStyle, 
    x, y, w, h,
    parent, (HMENU)ID, hInstance, NULL);
    
  if (!hwnd)
    MessageBox(NULL, "Control Creation Error.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
  else 
    SendMessage(hwnd, WM_SETFONT, (WPARAM)defFont, TRUE);
}

raw_control::raw_control(int _ID, DWORD dwExStyle, char *winClass, char *text, DWORD dwStyle, int x, int y, int w, int h, HWND _parent) 
{
  ID = _ID;
  parent = _parent;
  
  HINSTANCE hInstance = GetModuleHandle(NULL);
  
  hwnd = CreateWindowEx(WS_EX_LEFT | dwExStyle, winClass, text, 
    WS_CHILD | WS_VISIBLE | dwStyle, 
    x, y, w, h,
    parent, (HMENU)ID, hInstance, NULL);
    
  if (!hwnd)
    MessageBox(NULL, "Control Creation Error.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
  else 
    SendMessage(hwnd, WM_SETFONT, (WPARAM)defFont, TRUE);
}

raw_control::~raw_control()
{
  if (!DestroyWindow(hwnd))
  {    
    MessageBox(NULL, "Could Not Release Control handle...", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
    hwnd = NULL;						  
  }
}