#include "mdi_window.h"

bool mdi_window::registerClass(const char *_className)
{
  className = _className;
  
  WNDCLASSEX wc;

  if (!GetClassInfoEx(hInstance, _className, &wc))
  {
    wc.cbSize        =              sizeof(WNDCLASSEX);
    wc.style         =                               0;
    wc.lpfnWndProc   =         (WNDPROC) WndProcRouter;
    wc.cbClsExtra    =                               0;
    wc.cbWndExtra    =                               0;
    wc.hInstance     =                       hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       =     LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground =        (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  =                            NULL;  //no menu for now
    wc.lpszClassName =                       className;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) // Attempt To Register The Window Class
    {
      char error[50];
      sprintf(error, "Failed To Register %s Window Class.", className);
      
      MessageBox(NULL, error, "ERROR", MB_OK | MB_ICONEXCLAMATION);

      return false;	
    }
  }
  
  return true;
}

void mdi_window::create(const char* title, int x, int y, int width, int height)
{
  basic_window::create(title, x, y, width, height);
  
  CLIENTCREATESTRUCT ccs;

  hFrame = CreateWindowEx(
      WS_EX_APPWINDOW,
      className,
      title,
      WS_OVERLAPPEDWINDOW,
      x, y, width, height,
      NULL, NULL, hInstance, this);
      
  if (hFrame)
  {
    hSelf = CreateWindowEx(WS_EX_CLIENTEDGE, "mdiclient", NULL,
    WS_CHILD | WS_CLIPCHILDREN |  WS_VISIBLE, 
    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
    hFrame, NULL, hInstance, (LPSTR)&ccs);
    
    if (!hSelf)
    {
      destroy();
      MessageBox(NULL, "MDI Client Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
    }
  }
  else
  {
    destroy();
    MessageBox(NULL, "MDI Frame Window Creation Error.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
  }
}

void mdi_window::destroy()
{					  
  if (hFrame && !DestroyWindow(hFrame))	// Are We Able To Destroy The Window?
  {    
    char error[50];
    sprintf(error, "Could Not Release %s frame handle...", className);
    
    MessageBox(NULL, error, "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
    hFrame = NULL;						  
  }

  basic_window::destroy();
}

WPARAM mdi_window::messageLoop()
{
  MSG	msg;  
  
  bool done = false;
  while (!done)
  {     
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	               
    {
      if (msg.message == WM_QUIT)
        done = true;					
      else							
      {
        if (!TranslateMDISysAccel(&msg))
        {
          TranslateMessage(&msg);				// Translate The Message
          DispatchMessage(&msg);				// Dispatch The Message
        }
        
        switch (msg.message)
        {
          case WM_KEYDOWN: keys[msg.wParam] =  true; break;
          case WM_KEYUP:   keys[msg.wParam] = false; break;
        }
      }
    }
  }
  
  return msg.wParam;
}