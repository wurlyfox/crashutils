#include "mdi_child_window.h"

void mdi_child_window::create(const char* title, int x, int y, int width, int height)
{
  basic_window::create(title, x, y, width, height);
  
  MDICREATESTRUCT mcs;

  mcs.szTitle =                 title;
  mcs.szClass =             className;
  mcs.hOwner  =             hInstance; 
  mcs.x       =                     x;
  mcs.y       =                     y;
  mcs.cx      =                 width;
  mcs.cy      =                height;
  mcs.style   =   MDIS_ALLCHILDSTYLES;
  mcs.lParam  =          (LPARAM)this;
  
  //Create The Child Window
  hSelf = (HWND)::SendMessage(hParent, WM_MDICREATE, 0, (LONG)&mcs);
			
  if (!hSelf)
  {
    destroy();
    MessageBox(NULL, "MDI Child Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
  }
}

void mdi_child_window::destroy()
{
  if (hSelf && !DestroyWindow(hSelf))	// Are We Able To Destroy The Window?
  {    
    char error[50];
    sprintf(error, "Could Not Release %s window handle...", className);
    
    MessageBox(NULL, error, "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
    hSelf = NULL;						  
  }
  
  basic_window::destroy();
}