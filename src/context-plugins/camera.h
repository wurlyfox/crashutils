#ifndef CONTEXT_PLUGIN_CAMERA_H
#define CONTEXT_PLUGIN_CAMERA_H

#include "graphics/context_plugin.h"

#include "../message.h"

class context_plugin_camera : public context_plugin
{
  private:
  
  camera *sceneCamera;
  
  camera *modelCamera;
  camera *crashViewCamera;
  camera *crashCamera;
  
  void messageRouter(int msg, param lparam, param rparam);
  
  void onInit();
  void onSelect(int cam);
  void onSyncExternal(vector *location, vector *rotation);

  public:
  
  context_plugin_camera(context *ctxt) : context_plugin(ctxt)
  {
    sceneCamera = contextScene->getCamera();
    
    modelCamera = 0;
    crashCamera = 0;
  }
};

#endif