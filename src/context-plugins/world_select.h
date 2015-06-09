#ifndef CONTEXT_PLUGIN_WORLD_SELECT_H
#define CONTEXT_PLUGIN_WORLD_SELECT_H

#include "graphics/context_plugin.h"

#include "select.h"

class context_plugin_world_select : public context_plugin
{
  protected:
  
  context_plugin_select *parent;
  
  public:
  
  context_plugin_world_select(context *ctxt) : context_plugin(ctxt) {};

  //void onInit();
  void onPoint(int x, int y);
  
};

#endif