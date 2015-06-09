#ifndef MODEL_WLD_COLLADA_H
#define MODEL_WLD_COLLADA_H

#include "graphics/model_wld.h"

#include "graphics/image/texture_buffer_img.h"
#include "crash/eid.h"

typedef struct
{
  unsigned long count;
  char *name;
  int stride;
  float *source;
  char *params;
  int paramCount;
  int offset;
} mesh_source;

class model_wld_collada : public model_wld
{
  public:
  
  void exportDAE(entry *wgeo, texture_buffer_img *texbuf, prim_alloc *prims, const char *filename);
};

#endif


