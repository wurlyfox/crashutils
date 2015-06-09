#ifndef MODEL_FRG_H
#define MODEL_FRG_H

#include "prim_alloc.h"
#include "texture_buffer.h"
#include "types.h"

class model_frg
{
  public:
  bool render:1;
  int positionMode:1;
  int quadCount;
  int vertCount;
  
  quad *quads;
  vertex *vertices;
  
  vector location;
  fvector scale;
  
  void load(unsigned long texEID, int count, squad2 *quadInfo, unsigned char *texInfoArray, unsigned long scale, texture_buffer *texbuf, prim_alloc *prims);

  virtual void draw() {};             // draw with no translation
  virtual void drawAt(vector loc) {}; // draw with translation by loc
  void drawAtLocation();              // draw with translation by location (draw at location)
};

#endif
