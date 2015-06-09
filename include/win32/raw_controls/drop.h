#ifndef DROP_H
#define DROP_H

#include "raw_control.h"

class drop : public raw_control
{
  protected:
  
  int itemCount;
  int selIndex;
  
  public:
  
  drop(int dropID, int x, int y, int w, int h, HWND parent);
  
  //void clear();
  virtual void addItem(char *str, void *data=0);
  virtual void selectItem(int index);
  virtual  int getSelectedItem();
  virtual void occupy(int min, int max);
  
};

#endif