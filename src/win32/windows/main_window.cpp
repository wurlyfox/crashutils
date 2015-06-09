#include "main_window.h"

bool *keys;

main_window::main_window()
{
  registerClass("MAIN");
  
  for (int lp = 0; lp < 10; lp++)
    menu_flags[lp] = false;
    
  ::keys = keys;
}

LRESULT main_window::onCreate(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  HMENU hMenu, hSubMenu, hSubSubMenu;
  //HICON hIcon, hIconSm;

  hMenu = CreateMenu();

  hSubMenu = CreatePopupMenu();
  AppendMenu(hSubMenu, MF_STRING, ID_FILE_OPEN, "Open");
  AppendMenu(hSubMenu, MF_SEPARATOR, ID_SEPARATOR, "");
  AppendMenu(hSubMenu, MF_STRING, ID_FILE_EXIT, "Exit");
  AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "File");

  hSubMenu = CreatePopupMenu();
  AppendMenu(hSubMenu, MF_STRING, ID_VIEW_OCTREE, "Show Zone Collision Octrees");
  AppendMenu(hSubMenu, MF_STRING, ID_VIEW_CAMPTH, "Show Section Camera Paths");
  AppendMenu(hSubMenu, MF_STRING, ID_VIEW_WIREFRAME, "Wireframe");
  
  hSubSubMenu = CreatePopupMenu();
  AppendMenu(hSubMenu, MF_STRING | MF_POPUP, (UINT)hSubSubMenu, "Camera");
  AppendMenu(hSubSubMenu, MF_STRING, ID_VIEW_CAMERA_MODELMODE, "Model Mode");
  AppendMenu(hSubSubMenu, MF_STRING | MF_CHECKED, ID_VIEW_CAMERA_FLIGHTMODE, "Flight Mode");
  AppendMenu(hSubSubMenu, MF_STRING, ID_VIEW_CAMERA_CRASHMODE, "Crash Mode");
  menu_flags[5] = true;
  
  AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "View");
  
  hSubMenu = CreatePopupMenu();
  AppendMenu(hSubMenu, MF_STRING, ID_GOOL_CONTROL, "Control");
  AppendMenu(hSubMenu, MF_STRING, ID_GOOL_STEP, "Step Forward");
  AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "GOOL");

  SetMenu(hSelf, hMenu);
  /*hIcon = LoadImage(NULL, "menu_two.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
  if(hIcon)
    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
  else
    MessageBox(hwnd, "Could not load large icon!", "Error", MB_OK | MB_ICONERROR);

  hIconSm = LoadImage(NULL, "menu_two.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
  if(hIconSm)
    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSm);
  else
    MessageBox(hwnd, "Could not load small icon!", "Error", MB_OK | MB_ICONERROR);*/
}

LRESULT main_window::onCommand(UINT uMsg, WPARAM wParam, LPARAM lParam)
{  
  unsigned long params[2] = { 0, 0 };

  switch(LOWORD(wParam))
  {
    case ID_FILE_EXIT:
      ExitProcess(0);
    break; 
    case ID_FILE_OPEN:
    {
      char fileName[MAX_PATH] = "";
      FileOpenBox("NSD Files (*.nsd)\0*.nsd\0All Files (*.*)\0*.*\0", fileName, false);
      params[0] = (unsigned long)fileName;
      SendMessageRoot(MSG_LOAD_NSD, (WPARAM)0, (LPARAM)params);
    }
    break;
  }
  
  switch(LOWORD(wParam))
  {
    case ID_GOOL_STEP:
    {
      //SendMessage(objhWnd, WM_ACTIVATE, 0, 0);
      //resumeGoolSystem();
    }
    break;
    case ID_VIEW_OCTREE:
    {
      if (menu_flags[0] = (!menu_flags[0]))
        CheckMenuItem(GetMenu(hFrame), ID_VIEW_OCTREE, MF_CHECKED);
      else
        CheckMenuItem(GetMenu(hFrame), ID_VIEW_OCTREE, MF_UNCHECKED);
      
      SendMessageRoot(CPM_VISUAL_ZONE_TOGGLE_OCTREES, (WPARAM)0, (LPARAM)params);
    }
    break;
    
    case ID_VIEW_CAMERA_MODELMODE:
    {
      menu_flags[4] = true;
      
      CheckMenuItem(GetMenu(hFrame), ID_VIEW_CAMERA_MODELMODE, MF_CHECKED);
  
      if (menu_flags[5] == true)
        CheckMenuItem(GetMenu(hFrame), ID_VIEW_CAMERA_FLIGHTMODE, MF_UNCHECKED);
       if (menu_flags[7] == true)
        CheckMenuItem(GetMenu(hFrame), ID_VIEW_CAMERA_CRASHMODE, MF_UNCHECKED);
        
      menu_flags[5] = false;
      menu_flags[6] = false;
      menu_flags[7] = false;
    
      params[0] = 0;
      SendMessageRoot(CPM_CAMERA_SELECT, (WPARAM)0, (LPARAM)params);
    }
    break;
    case ID_VIEW_CAMERA_FLIGHTMODE:
    {
      menu_flags[5] = true;
      
      CheckMenuItem(GetMenu(hFrame), ID_VIEW_CAMERA_FLIGHTMODE, MF_CHECKED);
  
      if (menu_flags[4] == true)
        CheckMenuItem(GetMenu(hFrame), ID_VIEW_CAMERA_MODELMODE, MF_UNCHECKED);
      if (menu_flags[7] == true)
        CheckMenuItem(GetMenu(hFrame), ID_VIEW_CAMERA_CRASHMODE, MF_UNCHECKED);
        
      menu_flags[4] = false;
      menu_flags[6] = false;
      menu_flags[7] = false;
    
      params[0] = 1;
      SendMessageRoot(CPM_CAMERA_SELECT, (WPARAM)0, (LPARAM)params);
    }
    break;
    case ID_VIEW_CAMERA_CRASHMODE:
    {
      menu_flags[7] = true;
      
      CheckMenuItem(GetMenu(hFrame), ID_VIEW_CAMERA_CRASHMODE, MF_CHECKED);
  
      if (menu_flags[4] == true)
        CheckMenuItem(GetMenu(hFrame), ID_VIEW_CAMERA_MODELMODE, MF_UNCHECKED);
      if (menu_flags[5] == true)
        CheckMenuItem(GetMenu(hFrame), ID_VIEW_CAMERA_FLIGHTMODE, MF_UNCHECKED);

      menu_flags[4] = false;
      menu_flags[5] = false;
      menu_flags[6] = false;
    
      params[0] = 2;
      SendMessageRoot(CPM_CAMERA_SELECT, (WPARAM)0, (LPARAM)params);
    }
    break;
    
    case ID_VIEW_CAMPTH:
    {
      if (menu_flags[1] = (!menu_flags[1]))
        CheckMenuItem(GetMenu(hFrame), ID_VIEW_CAMPTH, MF_CHECKED);
      else
        CheckMenuItem(GetMenu(hFrame), ID_VIEW_CAMPTH, MF_UNCHECKED);
    
      SendMessageRoot(CPM_VISUAL_SECTION_TOGGLE, (WPARAM)0, (LPARAM)params);
    }
    break;
    case ID_VIEW_WIREFRAME:
    {
      if (menu_flags[2] = (!menu_flags[2]))
        CheckMenuItem(GetMenu(hFrame), ID_VIEW_WIREFRAME, MF_CHECKED);
      else
        CheckMenuItem(GetMenu(hFrame), ID_VIEW_WIREFRAME, MF_UNCHECKED);
      
      SendMessageRoot(CPM_VISUAL_WIREFRAME_TOGGLE, (WPARAM)0, (LPARAM)params);
    }
    break;
    case ID_GOOL_CONTROL:
    {
      if (menu_flags[3] = (!menu_flags[3]))
      {
        CheckMenuItem(GetMenu(hFrame), ID_GOOL_CONTROL, MF_CHECKED);

        menu_flags[7] = true;
      
        CheckMenuItem(GetMenu(hFrame), ID_VIEW_CAMERA_CRASHMODE, MF_CHECKED);
    
        if (menu_flags[4] == true)
          CheckMenuItem(GetMenu(hFrame), ID_VIEW_CAMERA_MODELMODE, MF_UNCHECKED);
        if (menu_flags[5] == true)
          CheckMenuItem(GetMenu(hFrame), ID_VIEW_CAMERA_FLIGHTMODE, MF_UNCHECKED);

        menu_flags[4] = false;
        menu_flags[5] = false;
        menu_flags[6] = false;
      
        params[0] = 2;
        SendMessageRoot(CPM_CAMERA_SELECT, (WPARAM)0, (LPARAM)params);
      }
      else
      {
        CheckMenuItem(GetMenu(hFrame), ID_GOOL_CONTROL, MF_UNCHECKED);
        
        menu_flags[5] = true;
      
        CheckMenuItem(GetMenu(hFrame), ID_VIEW_CAMERA_FLIGHTMODE, MF_CHECKED);
    
        if (menu_flags[4] == true)
          CheckMenuItem(GetMenu(hFrame), ID_VIEW_CAMERA_MODELMODE, MF_UNCHECKED);
        if (menu_flags[7] == true)
          CheckMenuItem(GetMenu(hFrame), ID_VIEW_CAMERA_CRASHMODE, MF_UNCHECKED);
          
        menu_flags[4] = false;
        menu_flags[6] = false;
        menu_flags[7] = false;
      
        params[0] = 1;
        SendMessageRoot(CPM_CAMERA_SELECT, (WPARAM)0, (LPARAM)params); 
      }
    
      SendMessageRoot(CSM_CONTROL_TOGGLE_ACTIVE, (WPARAM)0, (LPARAM)params);
    }   
    break;    
  } 
}

LRESULT main_window::onDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  PostQuitMessage(0);	
}