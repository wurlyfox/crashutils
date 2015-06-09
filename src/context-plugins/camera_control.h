#ifndef CONTEXT_PLUGIN_CAMERA_CONTROL_H
#define CONTEXT_PLUGIN_CAMERA_CONTROL_H

#include "graphics/context_plugin.h"

#include "../message.h"

#define VK_SHIFT    0x10
#define VK_LEFT	      37
#define VK_UP	        38
#define VK_RIGHT    	39
#define VK_DOWN	      40
#define VK_NUMPAD0	0x60
#define VK_NUMPAD1	0x61
#define VK_NUMPAD2	0x62
#define VK_NUMPAD3	0x63
#define VK_NUMPAD4	0x64
#define VK_NUMPAD5	0x65
#define VK_NUMPAD6	0x66
#define VK_NUMPAD7	0x67
#define VK_NUMPAD8	0x68
#define VK_NUMPAD9	0x69
#define VK_ADD	    0x6B
#define VK_SUBTRACT	0x6D

#define VK_SEMICOLON 0xBA
#define VK_COMMA 0xBC
#define VK_PERIOD 0xBE
#define VK_SLASH 0xBF

#define VK_LSHIFT 0xA0
#define VK_RSHIFT 0xA1

class context_plugin_camera_control : public context_plugin
{
  private:
  
  int mode;
  camera *sceneCamera;
  
  void messageRouter(int msg, param lparam, param rparam);
  
  void onToggleMode();
  void onHandleKeys(bool *keys);

  public:
  
  context_plugin_camera_control(context *ctxt) : context_plugin(ctxt)
  {
    mode = 0;
    sceneCamera = contextScene->getCamera();
  }  
};

#endif