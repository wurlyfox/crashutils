#ifndef BASIC_WINDOW_H
#define BASIC_WINDOW_H

#include "window.h"

class basic_window : public window
{
  protected:
  static LRESULT CALLBACK WndProcRouter(HWND hWnd, INT uMsg, WPARAM wParam, LPARAM lParam);
  
  const char *className;
  virtual bool registerClass(const char *_className);
  
  public:
  
  basic_window() : window() {}
  virtual ~basic_window() { destroy(); }
  
  virtual void init(const char *_className, HINSTANCE _hInstance, HWND _hParent);
  virtual void create(const char* title, int x, int y, int width, int height);
  virtual void destroy();
  
  virtual void SetFocus()
  {
    ::SetFocus(hSelf);
  }
  
  virtual void SetWindowText(const char *title)
  {
    ::SetWindowText(hSelf, title);
  }
  
  virtual void SetWindowPos(int x, int y, int width, int height)
  {
    ::SetWindowPos(hSelf, NULL, x, y, width, height, SWP_SHOWWINDOW);
  }
  
  virtual void ShowWindow(int nCmdShow)
  {
    ::ShowWindow(hSelf, nCmdShow);
  }
  
  virtual void UpdateWindow()
  {
    ::UpdateWindow(hSelf);
  }

  bool FileOpenBox(const char *filter, char *fileName, bool save);
 
  virtual void SendMessage(INT uMsg, WPARAM wParam, LPARAM lParam)
  {
    ::SendMessage(hSelf, uMsg, wParam, lParam);
  }
  
  virtual void PostMessage(INT uMsg, WPARAM wParam, LPARAM lParam)
  {  
    ::PostMessage(hSelf, uMsg, wParam, lParam);
  }
 
  virtual void SendMessageRoot(INT uMsg, WPARAM wParam, LPARAM lParam);
  virtual void PostMessageRoot(INT uMsg, WPARAM wParam, LPARAM lParam);
  virtual void BroadcastMessageRoot(INT uMsg, WPARAM wParam, LPARAM lParam);
  virtual void BroadcastMessage(INT uMsg, WPARAM wParam, LPARAM lParam);
    
  LRESULT WndProc(HWND hWnd, INT uMsg, WPARAM wParam, LPARAM lParam);
  virtual LRESULT DefProc(HWND hWnd, INT uMsg, WPARAM wParam, LPARAM lParam)
  {
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
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

  // misc shortcuts
  virtual HWND createText(DWORD dwStyle, int x, int y, int w, int h, char *text);
  virtual HWND createGroupBox(DWORD dwStyle, int x, int y, int w, int h, char *text);

  virtual void mouseMenu(HMENU menu);
};

#endif