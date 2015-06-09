#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "win32/mdi_window.h"
#include "win32/mdi_child_window.h"

#include "../../message.h"

#define ID_SEPARATOR 101
#define ID_FILE_EXIT 102
#define ID_FILE_OPEN 103
#define ID_GOOL_STEP 104
#define ID_GOOL_CONTROL 105
#define ID_VIEW_OCTREE 106
#define ID_VIEW_CAMPTH 107
#define ID_VIEW_WIREFRAME 108
#define ID_VIEW_CAMERA_MODELMODE 109
#define ID_VIEW_CAMERA_FLIGHTMODE 110
#define ID_VIEW_CAMERA_PATHMODE 111
#define ID_VIEW_CAMERA_CRASHMODE 112

class main_window : public mdi_window
{
  private:
  bool menu_flags[10];
  
  public:
  
  main_window();
    
  virtual LRESULT  onCreate(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual LRESULT onCommand(UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual LRESULT onDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif