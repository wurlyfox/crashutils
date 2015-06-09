#ifndef CONTEXT_PLUGIN_SELECT_H
#define CONTEXT_PLUGIN_SELECT_H

#include "graphics/context_plugin.h"
#include "../message.h"

struct selectable
{
  selectable *next;
  unsigned long z:4;
  unsigned long type:3;  //0=zone 1=model 2=world 3=object 4=section cam path 5=collision
  unsigned long item_index:9;
  unsigned long vert_index:16;
};

class context_plugin_select : public context_plugin
{
  friend class context_plugin_zone_select;
  friend class context_plugin_polygon_select;
  friend class context_plugin_section_select;
  friend class context_plugin_world_select;

  protected:
  
  model_msc *selections;
  
  int selectableCount;
  selectable selectables[MAX_POLYGONS*MAX_ZONES];
  
  context *parentContext;
  int contextWidth;
  int contextHeight;
  selectable **contextMap;
  
  selectable *closestSelectable;
  
  bool inited;
  
  public:
  
  context_plugin_select(context *ctxt) : context_plugin(ctxt)
  { 
    selections = 0;
    selectableCount = 0;
    closestSelectable = 0;
    
    parentContext = ctxt;
    contextWidth = 0;
    contextHeight = 0;
    contextMap = 0;
    
    inited = false;
  }
  
  virtual void onInit();
  virtual void onRender();
  virtual void onHover(int x, int y);
  virtual void onPoint(int x, int y) {};
  
  void recordSelectable(dpoint loc, int type, int index, int subindex);
};

#endif