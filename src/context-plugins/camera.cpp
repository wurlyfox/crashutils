#include "camera.h"

void context_plugin_camera::messageRouter(int msg, param lparam, param rparam)
{
  if (!(contextScene->getSceneFlags() & SCENE_SCENEFLAGS_SHAREDGEOM))
  {
    switch (msg)
    {
      case CPM_CAMERA_SELECT: onSelect((int)lparam); break;
      case GSM_CAMERA_SYNC_EXTERNAL: onSyncExternal((vector*)lparam, (vector*)rparam); break;
    }
  }
  
  context_plugin::messageRouter(msg, lparam, rparam);
}

void context_plugin_camera::onInit()
{
  sceneCamera = contextScene->getCamera();
  modelCamera = sceneCamera;
}

void context_plugin_camera::onSelect(int cam)
{  
  int mode = 0;
  camera *select = 0;

  switch (cam)
  {
    case 0: select =     modelCamera; mode = 0; break;
    case 1: select =     modelCamera; mode = 1; break;
    case 2: select = crashViewCamera; mode = 0; break;
  }
  
  if (select)
  {
    select->setMode(mode);
    contextScene->setCamera(select);
  }
}

void context_plugin_camera::onSyncExternal(vector *location, vector *rotation)
{
  if (!crashCamera)
  {
    sceneCamera     = contextScene->getCamera();
    crashCamera     = sceneCamera->newCamera();
    crashViewCamera = sceneCamera->newCamera();
    
    crashCamera->setMode(2);
    crashCamera->sync(location, rotation);
    crashViewCamera->setNested(crashCamera);
  }
}