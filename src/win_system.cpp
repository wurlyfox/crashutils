#include "win_system.h"

LRESULT main_window_com::onOther(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if ((uMsg >= WM_APP) && (uMsg < 0xC000))
  {
    if ((int)wParam != 1)
    {
      param *params = (param*)lParam;
      param lparam = lParam ? params[0] : 0;
      param rparam = lParam ? params[1] : 0;
      
      parentSystem->postMessage((int)uMsg, lparam, rparam, true);
    }
  }
}

void win_system::messageRouter(csystem *src, int msg, param lparam, param rparam)
{
  if (src == this) { return; }
  csystem::messageRouter(src, msg, lparam, rparam);
  
  switch (msg)
  {
    // crashutils global messages
		case MSG_START: onStart(); break;
    case MSG_INIT_NSD: onInitNSD((NSF*)lparam, (NSD*)rparam); break;
    
    // window system local messages
    case WSM_CREATE: onCreate(); break;
  }
  
  if (mainWindow)
  {    
    param params[2];
    params[0] = lparam;
    params[1] = rparam;
    
    main_window_com *mwc = (main_window_com*)mainWindow;
    mwc->BroadcastMessage((UINT)msg, (WPARAM)1, (LPARAM)params);
  }
}

void win_system::onCreate()
{
  HINSTANCE hInstance = GetModuleHandle(NULL);
  
  mainWindow = new main_window_com(this);
  mainWindow->init("MAIN", hInstance, NULL);
  mainWindow->create("CrashUtils", 300, 300, 1280, 800);
  mainWindow->SetWindowPos(200, 30, 1280, 800);
  mainWindow->ShowWindow(SW_SHOW/*nCmdShow*/);
  mainWindow->UpdateWindow();
  
  AllowSetForegroundWindow(ASFW_ANY);
}

void win_system::onStart()
{  
  mainWindow->messageLoop();
}

void win_system::onInitNSD(NSF *nsf, NSD *nsd)
{
  // instantiate the child windows:
  // - nsf    window: for nsf/nsd list
  // - model  window: for viewing zone/models and information
  // - gool   window: for viewing gool executable entries
  // - object window: for examining game object instances
  nsf_window *nsfWindow       = new nsf_window;
  model_window *modelWindow   = new model_window;
  gool_window *goolWindow     = new gool_window;
  //object_window *objectWindow = new object_window;
  
  // add each window as a child to the main window
  mainWindow->addChild(nsfWindow);
  mainWindow->addChild(modelWindow);
  mainWindow->addChild(goolWindow);
  //mainWindow->addChild(objectWindow);
  
  // create each child window
  nsfWindow->create(    "NSD/NSF file info",   0,   0,  838, 300);
  modelWindow->create(         "Model View",  25,  25, 1204, 700);
  goolWindow->create(    "Code Information",  50,  50,  980, 480);
  //objectWindow->create("Object Information", 848, 480,  512, 300);
  
  // hide all but the nsf window
  modelWindow->ShowWindow(SW_HIDE);
  goolWindow->ShowWindow(SW_HIDE);
  //objectWindow->ShowWindow(SW_HIDE); 
}