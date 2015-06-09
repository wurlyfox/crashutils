#ifndef DIALOG_H
#define DIALOG_H

#include "window.h"

class dialog : public window
{
  private:
  static BOOL CALLBACK DlgProcRouter(HWND hWnd, INT uMsg, WPARAM wParam, LPARAM lParam);
    
  public:
  
  virtual void create(INT_PTR &out);
  virtual void destroy() {};
  
  virtual LPCDLGTEMPLATE DlgTemplate(void *mem) {};
  virtual BOOL DlgProc(HWND hWnd, INT uMsg, WPARAM wParam, LPARAM lParam);
  
  virtual BOOL onInitDialog(INT uMsg, WPARAM wParam, LPARAM lParam) { return TRUE; }
  virtual BOOL     onCreate(INT uMsg, WPARAM wParam, LPARAM lParam) { return TRUE; }
  virtual BOOL    onCommand(INT uMsg, WPARAM wParam, LPARAM lParam) { return FALSE; }
  virtual BOOL      onOther(INT uMsg, WPARAM wParam, LPARAM lParam) { return FALSE; }
  
};

#endif