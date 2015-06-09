#include "raw_controls/radio.h"

radio::radio(int radioID, int x, int y, int w, int h, HWND _parent, char *text, DWORD dwStyle) :
raw_control(radioID, WC_BUTTON, text ? text : (char*)"", BS_AUTORADIOBUTTON | dwStyle, x, y, w, h, _parent) {}

void radio::useFlag(unsigned long *_data, signed char bit, signed char bitb, signed char bitc, signed char bitd)
{
  data = _data;
  
  flag = 0;
  notflag = 0;
  field = 0;

  if (bit < 0)
    notflag |= (1 << (-bit - 1));
  else
    flag |= (1 << (bit-1));
    
  if (bitb != -33)
  {
    if (bitb < 0)
      notflag |= (1 << (-bitb - 1));
    else
      flag |= (1 << (bitb-1));
  }
  
  if (bitc != -33)
  {
    if (bitc < 0)
      notflag |= (1 << (-bitc - 1));
    else
      flag |= (1 << (bitc-1));
  }
  
  if (bitd != -33)
  {
    if (bitd < 0)
      notflag |= (1 << (-bitd - 1));
    else
      flag |= (1 << (bitd - 1));
  }
    
  checked = ((flag && (*data & flag)) || (notflag && !(*data & notflag)));
  
  if (bitb != 0 && bitc != 0 && bitd != 0)
  {
    if (checked)
      Button_SetCheck(hwnd, BST_CHECKED);
  }
}

void radio::useField(signed char bit, signed char bitb, signed char bitc, signed char bitd)
{
  field = (1 << (bit-1));
  
  if (bitb != -33)
    field |= (1 << (bitb-1));
  if (bitc != -33)
    field |= (1 << (bitc-1));
  if (bitd != -33)
    field |= (1 << (bitd-1));    
  
  checked = ((*data & field) == flag);
  
  if (checked)
    Button_SetCheck(hwnd, BST_CHECKED);
}

void radio::handle(LPARAM lParam)
{
  //TODO
}


    