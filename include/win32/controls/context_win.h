#ifndef CONTEXT_WIN_H
#define CONTEXT_WIN_H

#include "../basic_control.h"

class context_win : public basic_control
{
  protected:
  
  HDC	hDC;    // Private GDI Device Context
  
  void created();
  bool opengl();
  
  void (*createCallback)() = 0;

  public:
  
  context_win() : basic_control()
  {
    hDC = NULL;
    
    className = "CONTEXT";
  }
  
  virtual void create(DWORD dwExStyle, int x, int y, int width, int height, int id);
  virtual void destroy();

  void SwapBuffers();
  
  void setCreateCallback(void (*callback)()) { createCallback = callback; }
  
  virtual LRESULT onCreate(UINT uMsg, WPARAM wParam, LPARAM lParam)
  {
    if (createCallback)
      (*createCallback)();
  }
  virtual LRESULT onCommand(UINT uMsg, WPARAM wParam, LPARAM lParam) {};
  virtual LRESULT onNotify(UINT uMsg, WPARAM wParam, LPARAM lParam) {};
  virtual LRESULT onPaint(UINT uMsg, WPARAM wParam, LPARAM lParam) {};
  virtual LRESULT onClick(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual LRESULT onRClick(UINT uMsg, WPARAM wParam, LPARAM lParam) {};
  virtual LRESULT onMouse(UINT uMsg, WPARAM wParam, LPARAM lParam) {};  
  virtual LRESULT onDestroy(UINT uMsg, WPARAM waram, LPARAM lParam) {};
};

#endif