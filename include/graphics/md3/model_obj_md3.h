#ifndef MODEL_OBJ_MD3_H
#define MODEL_OBJ_MD3_H

#include "texture_buffer_md3.h"

#include "graphics/model_obj.h"
#include "formats/md3.h"
#include "crash/eid.h"

class model_obj_md3 : public model_obj
{
  private:
  
  void bakePolygonTextures(texture_buffer_md3 *texbuf);

  public:
  
  void exportMD3(entry *svtx, texture_buffer_md3 *texbuf, prim_alloc *prims, char *filename);
};

#endif