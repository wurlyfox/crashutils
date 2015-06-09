#ifndef CHECK_H
#define CHECK_H

#include "raw_control.h"

class check : public raw_control
{
  protected:
  
  bool checked;
  
  unsigned long flag;
  unsigned long *data;
  
  public:
  
  check(int checkID, int x, int y, int w, int h, HWND parent, char *text=0, DWORD dwStyle=0);

  virtual void handle(LPARAM lParam);
  
  void useFlag(unsigned long *_data, unsigned char bit);
  
};

#endif