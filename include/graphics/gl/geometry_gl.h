#ifndef GEOMETRY_GL_H
#define GEOMETRY_GL_H

#include "graphics/geometry.h"

#include "model_wld_gl.h"
#include "model_obj_gl.h"
#include "model_spr_gl.h"
#include "model_frg_gl.h"
#include "model_msc_gl.h"
#include "texture_buffer_gl.h"

#define GL_ALLOC_STATIC

class geometry_gl : public geometry
{
  public:
  
  #ifdef GL_ALLOC_STATIC
  static model_wld_gl worldBuffer_s[0x100];
  static model_obj_gl objectBuffer_s[0x100];
  static model_spr_gl spriteBuffer_s[0x100];
  static model_frg_gl fragmentBuffer_s[0x100];
  static model_msc_gl miscBuffer_s[0x100];
  static prim_alloc primAlloc_s;
  static texture_buffer_gl textureBuffer_s;
  static unsigned long worldCount_s;
  static unsigned long objectCount_s;
  static unsigned long spriteCount_s;
  static unsigned long fragmentCount_s;
  static unsigned long miscCount_s;
  #endif
  
  geometry_gl(NSD *_nsd, NSF *_nsf);
  ~geometry_gl();
};

#endif