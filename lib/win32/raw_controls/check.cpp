#include "raw_controls/check.h"

check::check(int checkID, int x, int y, int w, int h, HWND _parent, char *text, DWORD dwStyle) :
raw_control(checkID, WC_BUTTON, text ? text : (char*)"", BS_AUTOCHECKBOX | dwStyle, x, y, w, h, _parent) {}

void check::useFlag(unsigned long *_data, unsigned char bit)
{
  data = _data;
  flag = (1 << (bit-1));
  
  if (*data & flag)
    Button_SetCheck(hwnd, checked=true);
  else
    Button_SetCheck(hwnd, checked=false);
}
  
void check::handle(LPARAM lParam)
{
  if (LOWORD(lParam) == ID)
  {
    checked = (Button_GetCheck(hwnd) == BST_CHECKED);
    
    if (checked)
      *data = (*data | flag);
    else
      *data = (*data & ~flag);
  }
}
      