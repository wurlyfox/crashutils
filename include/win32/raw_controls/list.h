#ifndef LIST_H
#define LIST_H

#define LIST_UNCHANGED  0x0000
#define LIST_RCLICK     0x0001
#define LIST_SELCHANGED 0x0002

#include <windows.h>
#include <stdio.h>

class list
{
  private:
  
  HWND hwnd;
  int ID;
  int itemCount; 
  int selectedIndex;
  
  public:
  
  list(int listID, int x, int y, int w, int h, HWND parent);

  int add(const char *header);
  int add(const char *header, unsigned long data);
  int handle(LPARAM lParam);
  unsigned long getSelectedItem();
  void clear();
  
};

#endif