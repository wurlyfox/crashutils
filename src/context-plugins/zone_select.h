#ifndef CONTEXT_PLUGIN_ZONE_SELECT_H
#define CONTEXT_PLUGIN_ZONE_SELECT_H

#include "graphics/context_plugin.h"

#include "select.h"

struct zone_outline
{
  unsigned char pointCount;
  dpoint outline[8];
  entry *zone;
};

class context_plugin_zone_select : public context_plugin
{
  protected:
  
  context_plugin_select *parent;
  
  int zoneOutlineCount;
  zone_outline zoneOutline[0x100];
  
  static int selectionCount;
  static fquad *selectionFaces;
  static fline *selectionLines;
  
  public:
  
  context_plugin_zone_select(context *ctxt) : context_plugin(ctxt) {};

  void onInit();
  void onRender();
  void onHover(int x, int y);
  void onPoint(int x, int y);

};

#endif