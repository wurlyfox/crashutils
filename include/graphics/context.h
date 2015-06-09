#ifndef CONTEXT_H
#define CONTEXT_H

#include "context_plugin.h"
#include "scene.h"

class context_plugin;
class context
{
  friend class scene;
  
  protected:
  
  int viewportWidth;
  int viewportHeight;
  
  scene *contextScene;
  
  // implementation can allocate/use this accordingly
  context_plugin *root;
  
  public:
  
  context()
  {
    viewportWidth = 0;
    viewportHeight = 0;
    
    contextScene = 0;
    
    root = 0;
  }
  
  virtual void render();
  
  virtual void draw() { contextScene->draw(); }
  virtual void update() {};
  virtual bool setActive() {};
  
  virtual void setProjection(int mode) {};
  
  void setScene(scene *newContextScene) { contextScene = newContextScene; }
  scene *getScene() { return contextScene; }
  
  virtual void share(context *src) {};
  
  // for plugins:
  void usePlugins(context_plugin *newRoot);
  context_plugin *getPlugins() { return root; }
  
  // context needs to be able to determine, given its scene's
  // camera, the 2 dimensional 'display/screen' coordinates 
  // (i.e. within its display) that a given 3 dimensional point projects to
  virtual void getViewport(int &width, int &height);
  virtual void setViewport(int width, int height);
  
  virtual dpoint project2d(double X, double Y, double Z) {};
  
  //... etc  
};


#endif