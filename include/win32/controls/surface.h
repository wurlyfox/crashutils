#ifndef SURFACE_H
#define SURFACE_H

#include "../basic_control.h"

class surface : public basic_control
{
  private:
  
  int swidth, sheight;

  HBITMAP draw;
  unsigned long *pixels;
  
  public: 
  
  surface()
  {
    className = "SURFACE";
  }
  
  virtual void init(HINSTANCE _hInstance, HWND _hParent);

  virtual void create(DWORD dwExStyle, int x, int y, int width, int height, int id);
  
  void paint();
  void clear();
  void putPixel(int x, int y, unsigned long rgb);
  void putPixelMag(int x, int y, int mag, unsigned long rgb);
  
  LRESULT   onPaint(UINT uMsg, WPARAM wParam, LPARAM lParam);
  
  static HBITMAP MakeSurface(HWND hWnd, int width, int height, unsigned long *&pixels);
};

#endif

