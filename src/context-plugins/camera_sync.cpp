#include "camera_sync.h"

void context_plugin_camera_sync::messageRouter(int msg, param lparam, param rparam)
{  
  switch (msg)
  {
    case CPM_CAMERA_SYNC: onSync((context*)lparam); break;
    case CPM_CAMERA_SMOOTH_TRANS: onSmoothTrans((point*)lparam); break;
  }
  
  context_plugin::messageRouter(msg, lparam, rparam);
}

void context_plugin_camera_sync::onInit()
{
  sceneCamera = contextScene->getCamera();
}

void context_plugin_camera_sync::onRender()
{
  if (syncScene)
  {
    sceneCamera = contextScene->getCamera();
    syncSceneCamera = syncScene->getCamera();
      
    if (!(syncCamera = syncSceneCamera->getNested()))
      syncCamera = syncSceneCamera;
          
    if (sceneCamera && syncCamera)
    {
      sceneCamera->setMode(1);
      sceneCamera->location.X = syncCamera->location.X;
      sceneCamera->location.Z = syncCamera->location.Z;

      sceneCamera->location.Y = 50.0;
      
      sceneCamera->rotation.X = 0;
      sceneCamera->rotation.Y = 90;
      sceneCamera->rotation.Z = 0;
    }
  }
}

void context_plugin_camera_sync::onSync(context *syncContextA)
{
  syncContext = syncContextA;
  syncScene   = syncContext->getScene();
  syncCamera  = syncScene->getCamera();
}

void context_plugin_camera_sync::onSmoothTrans(point *apr)
{
  if (syncContext)
  {
    context_plugin *syncRoot = syncContext->getPlugins();
    syncRoot->postMessage(CPM_CAMERA_SMOOTH_TRANS, (param)apr);
  }
}