#include "raw_control.h"

#define RADIO_USEFIELD 0
class radio : public raw_control
{
  private:
  
  bool checked;

  unsigned long *data;
  unsigned long flag;
  unsigned long notflag;
  unsigned long field;
    
  public:
  
  radio(int radioID, int x, int y, int w, int h, HWND parent, char *text=0, DWORD dwStyle=0);

  void useFlag(unsigned long *_data, 
                signed char  bit=-33, signed char bitb=-33,
                signed char bitc=-33, signed char bitd=-33);
  void useField(signed char  bit=-33, signed char bitb=-33,
                signed char bitc=-33, signed char bitd=-33);

  void handle(LPARAM lParam);                
};
