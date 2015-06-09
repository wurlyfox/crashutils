#include "camera_trans.h"

#include <math.h>

void context_plugin_camera_trans::messageRouter(int msg, param lparam, param rparam)
{  
  if (!(contextScene->getSceneFlags() & SCENE_SCENEFLAGS_SHAREDGEOM))
  {
    context_plugin::messageRouter(msg, lparam, rparam);

    switch (msg)
    {
      case CPM_CAMERA_SMOOTH_TRANS: onSmoothTrans((point*)lparam); break;
    }
  }
}

void context_plugin_camera_trans::onInit()
{
  contextCamera = contextScene->getCamera();
  
  doTrans = false;
}

void context_plugin_camera_trans::onRender()
{
  if (doTrans)
  {
    bool notDone = false;

    if ((speed >= max) && !phase)
    {
      accel = -accel;
      phase = true;
    }
      
    speed += accel;
    
    contextCamera->location.X += (dirX*speed);
    contextCamera->location.Y += (dirY*speed);
    contextCamera->location.Z += (dirZ*speed);
    
    if ((dirX > 0) && (contextCamera->location.X < aprX))
      notDone = true;
    else if ((dirX < 0) && (contextCamera->location.X > aprX))
      notDone = true;
      
    if ((dirY > 0) && (contextCamera->location.Y < aprY))
      notDone = true;
    else if ((dirY < 0) && (contextCamera->location.Y > aprY))
      notDone = true;
      
    if ((dirZ > 0) && (contextCamera->location.Z < aprZ))
      notDone = true;
    else if ((dirZ < 0) && (contextCamera->location.Z > aprZ))
      notDone = true;
            
    if (!notDone)
    {
      contextCamera->location.X = aprX;
      contextCamera->location.Y = aprY;
      contextCamera->location.Z = aprZ;
    
      speed = 0;
    }
    
    doTrans = notDone;
    postMessage(GSM_INVALIDATE);
  }
}

void context_plugin_camera_trans::onSmoothTrans(point *apr)
{
  doTrans = true;
  
  aprX = ((float)apr->X/0x1000)*8;
  aprY = ((float)apr->Y/0x1000)*8;
  aprZ = ((float)apr->Z/0x1000)*8;
  
  dirX = (aprX - contextCamera->location.X);
  dirY = (aprY - contextCamera->location.Y);
  dirZ = (aprZ - contextCamera->location.Z);
  
  float scale = sqrt((dirX*dirX) + (dirY*dirY) + (dirZ*dirZ));
  
  dirX /= scale;
  dirY /= scale;
  dirZ /= scale;
  
  // 50 = n = number of times a constant
  // 'accel' needs to increasingly accumulate and then 
  // decreasingly decumulate speed such that accumulation
  // of position with that updated speed each time will
  // reach 'scale'; approx 50 frames for a camera jump
  int n = 15;
  accel = (scale)/(n*n);
  
  max = accel*((n*(n+1))/2);
    
  speed = 0;
  phase = false;
  
  postMessage(GSM_INVALIDATE);
}
  
  