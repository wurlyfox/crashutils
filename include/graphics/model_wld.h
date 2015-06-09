#ifndef MODEL_WLD_H
#define MODEL_WLD_H

#include "../crash/entry.h"

#include "prim_alloc.h"
#include "texture_buffer.h"
#include "types.h"

#define SCALE_WORLD 8

class model_wld
{
  public:
  int polyCount;
  int polyTexCount;
  int polyNonCount:15;
  int positionMode:1;
  int vertCount;
  
  polygon *polygons;
  vertex *vertices;
  #ifdef GL_VERTEX_ARRAY_ENABLE
  fpoint2 *texcoords;
  #endif
  
  fregion bound;
  vector location;
  
  void load(entry *wgeo, texture_buffer *texbuf, prim_alloc *prims);
  virtual void loadPolygons(entry *wgeo, texture_buffer *texbuf, prim_alloc *prims);
  virtual void loadVertices(entry *wgeo, prim_alloc *prims);
  
  virtual void draw() {};             // draw with no translation
  virtual void drawAt(vector loc) {}; // draw with translation by loc
  void drawAtLocation();              // draw with translation by location (draw at location)

  bool selected;
  virtual void select()   { selected = true; }   //causes alt selected color for draw
  virtual void deselect() { selected = false; }  
};

#endif