#ifndef CONTEXT_PLUGIN_CAMERA_TRANS_H
#define CONTEXT_PLUGIN_CAMERA_TRANS_H

#include "graphics/context_plugin.h"
#include "../message.h"

class context_plugin_camera_trans : public context_plugin
{
  protected:
  
  camera *contextCamera;
  
  bool doTrans;
  bool phase;
  
  float accel;
  float speed;
  float max;
  
  float dirX; float dirY; float dirZ;
  float aprX; float aprY; float aprZ;
    
  void messageRouter(int msg, param lparam, param rparam);

  void onInit();
  void onRender();
  void onSmoothTrans(point *apr);
  
  public:
  
  context_plugin_camera_trans(context *ctxt) : context_plugin(ctxt) {};
};

#endif