#ifndef CONTEXT_PLUGIN_SECTION_VISUAL_H
#define CONTEXT_PLUGIN_SECTION_VISUAL_H

#include "graphics/context_plugin.h"
#include "../message.h"

#include "crash/entry_cache.h"

struct model_path
{
  model_msc *path;
  fline *trail;
  ftriangle *arrows;
};

class context_plugin_section_visual : public context_plugin
{
  protected:
  
  context_plugin_visual *parent;
  
  static itemCache<model_path> pathCache;
  
  public:
  
  context_plugin_section_visual(context *ctxt) : context_plugin(ctxt) {};

  void onInit();
  void messageRouter(int msg, param lparam, param rparam);
  
  void buildPath(entry *zone, unsigned char *section, unsigned short min, unsigned short max);

};

#endif