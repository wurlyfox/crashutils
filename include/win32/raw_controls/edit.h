#ifndef EDIT_H
#define EDIT_H

#include "raw_control.h"

#define UDM_SETRANGE32 (WM_USER+111)
#define UDM_SETPOS32 (WM_USER+113)
#define UDM_GETPOS32 (WM_USER+114)

class edit : public raw_control
{
  protected:
  
  HWND spin;
  int spinID;
    
  char *charValue;
  short *shortValue;
  long *longValue;
  
  long scale;

  public:
  
  edit(int editID, int x, int y, int w, int h, HWND parent, char *text=0, DWORD dwStyle=0);
  ~edit();
  
  void addSpin(int spinID, int min=0, int max=0);
  void setSpinRange(int min, int max);
  
  void useValue( long *_value, long _scale=1);
  void useValue(short *_value, long _scale=1);
  void useValue( char *_value, long _scale=1);

  long getValue();
  long getValue(bool &valid);
  void setValue(char value);
  void setValue(short value);
  void setValue(long value);
  
  void handle(WPARAM wParam, LPARAM lParam);
};

#endif