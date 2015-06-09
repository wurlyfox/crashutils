#include "raw_controls/drop.h"
#include <stdio.h>

drop::drop(int dropID, int x, int y, int w, int h, HWND _parent) :
raw_control(dropID, WC_COMBOBOX, "", CBS_DROPDOWN | CBS_HASSTRINGS,
            x, y, w, h, _parent)
{
	itemCount = 0;
  selIndex = -1;
}

void drop::addItem(char *str, void *data)
{
  SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)str);
  SendMessage(hwnd, CB_SETITEMDATA, (LPARAM)itemCount, (WPARAM)data);
  
  itemCount++;
}

void drop::selectItem(int index)
{
  SendMessage(hwnd, CB_SETCURSEL, (WPARAM)(index), 0);
}

int drop::getSelectedItem()
{
  return ComboBox_GetCurSel(hwnd);
}

void drop::occupy(int min, int max)
{
  for (int lp=min; lp<=max; lp++)
  {
    char itemString[10];
    sprintf(itemString, "%i\n", lp);
    addItem(itemString, (void*)lp);
  }
}