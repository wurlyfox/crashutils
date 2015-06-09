#ifndef CONTEXT_PLUGIN_CAMERA_SYNC_H
#define CONTEXT_PLUGIN_CAMERA_SYNC_H

#include "graphics/context_plugin.h"
#include "../message.h"

class context_plugin_camera_sync : public context_plugin
{
  protected:
  
  context *syncContext;
  scene *syncScene;
  camera *syncSceneCamera;
  
  camera *syncCamera;
  camera *sceneCamera;

  void messageRouter(int msg, param lparam, param rparam);
  
  public:
  
  context_plugin_camera_sync(context *ctxt) : context_plugin(ctxt) 
  {    
    syncContext = 0;
    syncScene = 0;
    syncCamera = 0;
    
    sceneCamera = contextScene->getCamera();
  }

  virtual void onInit();
  virtual void onRender();
  virtual void onSync(context *syncContextA);
  virtual void onSmoothTrans(point *apr);
};

#endif