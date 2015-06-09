#ifndef MODEL_MSC_H
#define MODEL_MSC_H

#include "prim_alloc.h"
#include "types.h"

#define SCALE_MISC 8

class model_msc
{
  public:
  int lineCount;
  int triangleCount;
  int quadCount;
  
  fline *lines;
  ftriangle *triangles;
  fquad *quads;
  
  vector location;
  fcolor color;
  
  fline *newLines(prim_alloc *prims, int count);
  ftriangle *newTriangles(prim_alloc *prims, int count);
  fquad *newQuads(prim_alloc *prims, int count); 
  
  virtual void draw() {};             // draw with no translation
  virtual void drawAt(vector loc) {}; // draw with translation by loc
  void drawAtLocation();              // draw with translation by location (draw at location)
};

#endif