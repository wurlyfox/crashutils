#include "camera_control.h"

void context_plugin_camera_control::messageRouter(int msg, param lparam, param rparam)
{
  if (!(contextScene->getSceneFlags() & SCENE_SCENEFLAGS_SHAREDGEOM))
  {
    switch (msg)
    {
      case MSG_HANDLE_KEYS: onHandleKeys((bool*)lparam); break;
      
      case CSM_CONTROL_TOGGLE_ACTIVE: onToggleMode(); break;
    }
  }
  
  context_plugin::messageRouter(msg, lparam, rparam);
}

void context_plugin_camera_control::onToggleMode()
{
  mode = !mode;
}

void context_plugin_camera_control::onHandleKeys(bool *keys)
{
  sceneCamera = contextScene->getCamera();
  
  bool render = false;
  
  if ((mode == 0) || ((mode == 1) && keys[VK_SHIFT]))
  {
    if (keys['O'])          { sceneCamera->rotY(-1.0); render = true; }
    if (keys['L'])          { sceneCamera->rotY(1.0);  render = true; }
    if (keys[VK_SEMICOLON]) { sceneCamera->rotX(1.0);  render = true; }
    if (keys['K'])          { sceneCamera->rotX(-1.0); render = true; }
                           
    if (keys['A'])          { sceneCamera->move(1.0);  render = true; }
    if (keys['Z'])          { sceneCamera->move(-1.0); render = true; }
  }
  
  if (keys[VK_NUMPAD7])  { sceneCamera->rotZ(-1.0); render = true; }
  if (keys[VK_NUMPAD9])  { sceneCamera->rotZ(1.0);  render = true; }
  if (keys[VK_NUMPAD6])  { sceneCamera->panX(1.0);  render = true; }
  if (keys[VK_NUMPAD4])  { sceneCamera->panX(-1.0); render = true; }
  if (keys[VK_NUMPAD8])  { sceneCamera->panY(-1.0); render = true; }
  if (keys[VK_NUMPAD2])  { sceneCamera->panY(1.0);  render = true; }   

  
  if (keys[VK_ADD])      { sceneCamera->updateRates(0.04f);  render = true; }
  if (keys[VK_SUBTRACT]) { sceneCamera->updateRates(-0.04f); render = true; }
   
  if (keys['P'])         { sceneCamera->reset(); render = true; }

  if (render)
    postMessage(GSM_INVALIDATE);
    
  //if (keys['Q']) { ((model_window*)(children->sibling))->viewLevel(); render = true; }
}