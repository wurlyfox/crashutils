#ifndef RENDER_MANAGER_H
#define RENDER_MANAGER_H

//#include "scene.h"
#include "context.h"

class render_manager
{
  private:
  
  //scene *renderScene;
  context *renderContext[10];
  int contextCount;
  
  bool render;
  bool wait;
  
  int drawCount;
  
  // used if another thread wishes to draw, synchronously
  // void (*contextCallback)(int context);
  void (*syncSuspendCallback)();
  void (*syncResumeCallback)();
  void (*syncCallback)();
  
  // used if graphics sys implementation requires an 'update'
  // (i.e. buffer swap) after each render
  // void (*updateCallback)();
  
  public:

  render_manager() 
  {
    //renderScene = 0;
    contextCount = 0;
    
    render = false;
    wait = false;
    
    drawCount = 0;  
  }
  virtual ~render_manager() { /*delete renderScene; */};
  
  void invalidate();
  bool draw();
  void drawSync();
 
  //void setUpdateCallback(void (*callback)()) 
  //{ 
  //  updateCallback = callback; 
  //}
  //void setContextCallback(void (*callback)(int context))
  //{
  //  contextCallback = callback;
  //}    
  void setSyncSuspendCallback(void (*callback)()) 
  { 
    syncSuspendCallback = callback; 
  }
  void setSyncResumeCallback(void (*callback)())
  {
    syncResumeCallback = callback;
  }
  void setSyncCallback(void (*callback)())
  {
    syncCallback = callback;
  }
  
  //void setContext(int context)   { (*contextCallback)(context); }
  //void setScene(scene *newScene) { renderScene = newScene; } 
  //scene *getScene()              { return renderScene; }
  void addContext(context *newContext)
  {
    renderContext[contextCount++] = newContext;
  }
  context *getContext(int index)
  {
    return renderContext[index];
  }
  
  void pluginThread();
};

#endif