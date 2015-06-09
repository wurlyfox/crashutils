#include "controls/colorbox.h"

void colorbox::create(DWORD dwExStyle, int _x, int _y, int _id)
{
  basic_control::create(WS_EX_CLIENTEDGE | dwExStyle, _x, _y, 15, 15, _id);
  
  if (!(hDC = GetDC(hSelf)))   
  {
    destroy();
    MessageBox(NULL, "Can't Create A Device Context.", "ERROR", MB_OK|MB_ICONEXCLAMATION);				
  }
}    
    
void colorbox::destroy()
{
  if (hDC)
  {
    if (!ReleaseDC(hSelf, hDC))
      MessageBox(NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
  }
  
  basic_control::destroy();
}

LPARAM colorbox::onPaint(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  RECT area;
  GetClientRect(&area);
  
  HBRUSH backColor = CreateSolidBrush(RGB);
  
  PAINTSTRUCT ps;
  
  HDC hDC = BeginPaint(hSelf, &ps);
  FillRect(hDC, &area, backColor);
	EndPaint(hSelf, &ps);
  
  DeleteObject(backColor);
}

LPARAM colorbox::onClick(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  chooseColor();
  
  InvalidateRect(hSelf, NULL, true);
}

void colorbox::chooseColor()
{
  CHOOSECOLOR colorChooser;
  COLORREF custColors[16];
  
  ZeroMemory(&colorChooser, sizeof(colorChooser));
  colorChooser.lStructSize = sizeof(colorChooser);
  colorChooser.lpCustColors = (LPDWORD) custColors;
  colorChooser.rgbResult = RGB;
  colorChooser.hwndOwner = hParent;
  colorChooser.Flags     = CC_ANYCOLOR | CC_FULLOPEN;
  
  ChooseColor(&colorChooser);
  
  setColor(colorChooser.rgbResult);
}
  
void colorbox::setColor(long color)
{
  RGB = color;
  
  InvalidateRect(hSelf, NULL, true);
}

long colorbox::getColor()
{
  return RGB;
}
