#ifndef RAW_CONTROL_H
#define RAW_CONTROL_H

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

struct raw_group_createparams
{
  int countx;
  int county;
  int baseID;
  int x;
  int y;
  int w; 
  int h;
  int padl;
  int padt;
  int padr;
  int padb;
  char *text;
  int texlen;
};

class raw_control
{
  protected:
  
  HWND hwnd;
  int ID;

  HWND parent;

  static HFONT defFont;
  
  public:

  raw_control(int _ID, char *winClass, char *text, DWORD dwExStyle, int x, int y, int w, int h, HWND _parent);
  raw_control(int _ID, DWORD dwExStyle, char *winClass, char *text, DWORD dwStyle, int x, int y, int w, int h, HWND _parent); 

  ~raw_control();
  
  template <typename T> static void createGroup(T **group, raw_group_createparams params, HWND _parent)
  {
    int count = params.countx * params.county;
    
    for (int ylp = 0; ylp < params.county; ylp++)
    {
      int ofspty = (params.padt*(ylp+1));
      int ofspby = (params.padb*ylp);
      int ofschy = (params.h*ylp);
      int ofsy = ofspty + ofschy + ofspby;
      
      for (int xlp = 0; xlp < params.countx; xlp++)
      {      
        int ofsplx = (params.padl*(xlp+1));
        int ofsprx = (params.padr*xlp);
        int ofscwx = (params.w*xlp);
        int ofsx = ofsplx + ofscwx + ofsprx;
        
        int x = params.x + ofsx;
        int y = params.y + ofsy;
        
        int index = xlp+(ylp*params.countx);
        
        T *dat;
        if (params.text)
        {
          char *text = &params.text[index*params.texlen];

          if (index == 0)
          {
            dat = new T(params.baseID + index, x, y, 
                        params.w, params.h, _parent,
                        text, WS_GROUP);
          }
          else
          {
            dat = new T(params.baseID + index, x, y, 
                        params.w, params.h, _parent,
                        text, 0);
          } 
        }
        else
        {
          if (index == 0)
          {
            dat = new T(params.baseID + index, x, y, 
                        params.w, params.h, _parent,
                        0, WS_GROUP);
          }
          else
          {
            dat = new T(params.baseID + index, x, y, 
                        params.w, params.h, _parent,
                        0, 0);
          } 
        }
        
        group[index] = dat;
      }
    }
  }    
  
  //void clear();
  virtual void handle(LPARAM lParam) {}; 
};

#endif