#ifndef CONTEXT_PLUGIN_POLYGON_SELECT_H
#define CONTEXT_PLUGIN_POLYGON_SELECT_H

#include "graphics/context_plugin.h"

#include "select.h"

class context_plugin_polygon_select : public context_plugin
{
  protected:
  
  context_plugin_select *parent;
  
  static int selectionCount;
  static ftriangle *selections;
  
  public:
  
  context_plugin_polygon_select(context *ctxt) : context_plugin(ctxt) {};
  
  void onInit();
  
  void onRender();
  void onHover(int x, int y);
  
};

#endif