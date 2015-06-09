#include "graphics/gl/geometry_gl.h"

#ifdef GL_ALLOC_STATIC
model_wld_gl geometry_gl::worldBuffer_s[0x100];
model_obj_gl geometry_gl::objectBuffer_s[0x100];
model_spr_gl geometry_gl::spriteBuffer_s[0x100];
model_frg_gl geometry_gl::fragmentBuffer_s[0x100];
model_msc_gl geometry_gl::miscBuffer_s[0x100];
prim_alloc geometry_gl::primAlloc_s;
texture_buffer_gl geometry_gl::textureBuffer_s;
unsigned long geometry_gl::worldCount_s;
unsigned long geometry_gl::objectCount_s;
unsigned long geometry_gl::spriteCount_s;
unsigned long geometry_gl::fragmentCount_s;
unsigned long geometry_gl::miscCount_s;
#endif

geometry_gl::geometry_gl(NSD *_nsd, NSF *_nsf) : geometry(_nsd, _nsf)
{
  #ifdef GL_ALLOC_STATIC
  worldCount_s = 0;
  objectCount_s = 0;
  spriteCount_s = 0;
  fragmentCount_s = 0;
  miscCount_s = 0;
  #endif
   
  for (int lp = 0; lp < 0x100; lp++)
  {
    #ifdef GL_ALLOC_STATIC
    worldBuffer[lp] = &worldBuffer_s[worldCount_s++];
    objectBuffer[lp] = &objectBuffer_s[objectCount_s++];
    spriteBuffer[lp] = &spriteBuffer_s[spriteCount_s++];
    fragmentBuffer[lp] = &fragmentBuffer_s[fragmentCount_s++];
    miscBuffer[lp] = &miscBuffer_s[miscCount_s++];    
    #else
    worldBuffer[lp]  = new model_wld_gl;
    objectBuffer[lp] = new model_obj_gl;
    spriteBuffer[lp] = new model_spr_gl;
    fragmentBuffer[lp] = new model_frg_gl;
    miscBuffer[lp] = new model_msc_gl;
    #endif
  }
 
  #ifdef GL_ALLOC_STATIC
  primAlloc = &primAlloc_s;
  textureBuffer = &textureBuffer_s;
  textureBuffer->setNSDNSF(_nsd, _nsf);
  textureBuffer->initAll();
  #else
  primAlloc    = new prim_alloc; 
  textureBuffer = new texture_buffer_gl(_nsd, _nsf);
  #endif
}
  
geometry_gl::~geometry_gl()
{
  #ifndef GL_ALLOC_STATIC
  for (int lp = 0; lp < 0x100; lp++)
  {
    delete worldBuffer[lp];
    delete objectBuffer[lp];
    delete spriteBuffer[lp];
    delete fragmentBuffer[lp];
    delete miscBuffer[lp];
  }
  
  delete primAlloc;
  delete textureBuffer;
  #endif
}