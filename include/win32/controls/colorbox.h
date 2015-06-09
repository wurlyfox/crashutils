#ifndef COLORBOX_H
#define COLORBOX_H

#include "../basic_control.h"

class colorbox : public basic_control
{
  protected:
  
  HDC hDC;
  
  unsigned long RGB;
  
  public:
  
  colorbox() : basic_control()
  {    
    hDC = NULL;
    RGB = 0;
    
    className = "COLORBOX";
  }
  ~colorbox() {};
  
  virtual void create(DWORD dwExStyle, int _x, int _y, int _id);
  virtual void destroy();
  
  LRESULT onPaint(UINT uMsg, WPARAM wParam, LPARAM lParam);
  LRESULT onClick(UINT uMsg, WPARAM wParam, LPARAM lParam);
  
  void chooseColor();

  long getColor();
  void setColor(long value);
};

#endif