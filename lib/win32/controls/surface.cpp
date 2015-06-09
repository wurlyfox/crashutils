#include "controls/surface.h"

void surface::init(HINSTANCE _hInstance, HWND _hParent)
{
  basic_window::init("surface", _hInstance, _hParent);
}

void surface::create(DWORD dwExStyle, int x, int y, int width, int height, int id)
{
  basic_control::create(dwExStyle, x, y, width, height, id);
  
  draw = MakeSurface(hSelf, width, height, pixels);
  
  swidth  = width;
  sheight = height;
}

LRESULT surface::onPaint(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  PAINTSTRUCT ps;  
  HDC hDC = BeginPaint(hSelf, &ps);
      
  HDC hdcMem = CreateCompatibleDC(hDC);
  HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, draw);
  BitBlt(hDC, 0, 0, swidth, sheight, hdcMem, 0, 0, SRCCOPY);
  SelectObject(hdcMem, hbmOld);
  
  EndPaint(hSelf, &ps);
}

void surface::paint()
{
  InvalidateRect(hSelf, NULL, false);
}

void surface::clear()
{
  for (int lp = 0; lp < (swidth * sheight); lp++)
    pixels[lp] = 0x00FFFFFF;
}

void surface::putPixel(int x, int y, unsigned long rgb)
{
  pixels[x + (y * swidth)] = rgb;
}

void surface::putPixelMag(int x, int y, int mag, unsigned long rgb)
{
  if (mag == 0) { mag = 1; }
  x*=(mag);
  y*=(mag);
  
  for (int ylp = y; ylp < y + (mag); ylp++)
  {
    for (int xlp = x; xlp < x + (mag); xlp++)
      pixels[xlp + (ylp * swidth)] = rgb;
  }
}

HBITMAP surface::MakeSurface(HWND hWnd, int width, int height, unsigned long *&pixels)
{
  /* Use CreateDIBSection to make a HBITMAP which can be quickly
   * blitted to a surface while giving 100% fast access to pixels
   * before blit.
   */
  HBITMAP hbmp;
  
  // Desired bitmap properties
  BITMAPINFO bmi;
  bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
  bmi.bmiHeader.biWidth =     width;
  bmi.bmiHeader.biHeight =  -height; // Order pixels from top to bottom
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biBitCount = 32; // last byte not used, 32 bit for alignment
  bmi.bmiHeader.biCompression = BI_RGB;
  bmi.bmiHeader.biSizeImage = 0;
  bmi.bmiHeader.biXPelsPerMeter = 0;
  bmi.bmiHeader.biYPelsPerMeter = 0;
  bmi.bmiHeader.biClrUsed = 0;
  bmi.bmiHeader.biClrImportant = 0;
  bmi.bmiColors[0].rgbBlue = 0;
  bmi.bmiColors[0].rgbGreen = 0;
  bmi.bmiColors[0].rgbRed = 0;
  bmi.bmiColors[0].rgbReserved = 0;
  HDC hdc = GetDC(hWnd);

  // Create DIB section to always give direct access to pixels
  hbmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void**)&pixels, NULL, 0 );
  DeleteDC(hdc);

  return hbmp;  
}

