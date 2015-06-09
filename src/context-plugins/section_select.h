#ifndef CONTEXT_PLUGIN_SECTION_SELECT_H
#define CONTEXT_PLUGIN_SECTION_SELECT_H

#include "graphics/context_plugin.h"

#include "../message.h"

#include "select.h"

class context_plugin_section_select : public context_plugin
{
  protected:
  
  context_plugin_select *parent;
  
  public:
  
  context_plugin_section_select(context *ctxt) : context_plugin(ctxt) {};

  void onInit();
  void onRender();
  void onPoint(int x, int y);
  
};

#endif