// left-child, right-sibling representation

#ifndef CONTEXT_PLUGIN_H
#define CONTEXT_PLUGIN_H

#include "context.h"
#include "scene.h"
#include "geometry.h"

#define CPM_INIT   0x9001
#define CPM_RENDER 0x9002
#define CPM_HOVER  0x9003
#define CPM_POINT  0x9004

#ifndef TYPE_PARAM
#define TYPE_PARAM
typedef unsigned long param;
#endif

class context;
class context_plugin
{
  friend class context;
	friend class render_manager;
  protected:
 
  context *parentContext;
  
  context_plugin *parent;
  context_plugin *sibling;
  context_plugin *children;
  
  scene *contextScene;
  geometry *geom;
  
  virtual void messageRouter(int msg, param lparam, param rparam);
  
  virtual void onInit() {};
  virtual void onRender() {};
  virtual void onHover(int x, int y) {};
  virtual void onPoint(int x, int y) {};

  public:
  context_plugin();
  context_plugin(context *ctxt);
  ~context_plugin();
  
  void addChild(context_plugin *child);
  context_plugin *getChild(int index);
	
	void setContext(context *ctxt);
	
  void postMessage(int msg, param lparam=0, param rparam=0);
};

#endif