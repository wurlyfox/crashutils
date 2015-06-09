#ifndef TEXTURE_BUFFER_GL_H
#define TEXTURE_BUFFER_GL_H

#include "graphics/texture_buffer.h"

#define GL_TEXIMAGE_W 1024
#define GL_TEXIMAGE_H 1024

#define GL_COORDX(P) ((float)P/GL_TEXIMAGE_W)    
#define GL_COORDY(P) (1-((float)P/GL_TEXIMAGE_H))
#define CR_COORDX(P) ((int)(P*GL_TEXIMAGE_W))
#define CR_COORDY(P) ((int)((1-P)*GL_TEXIMAGE_H))

class texture_buffer_gl : public texture_buffer
{
  private:
  
  bool inited;
 	unsigned int globalTex;

  public:
  
  texture_buffer_gl() {};
  texture_buffer_gl(NSD *_nsd, NSF *_nsf) : texture_buffer(_nsd, _nsf) 
  {
    inited = false;
    initAll();
  };
  ~texture_buffer_gl() {};

  void subTextureCreate(textureCacheEntry *tex);

	void init();
	void storeBlock(unsigned long *pixBuf, int x, int y, int w, int h);
	fquad2 getBlockCoords(point A, point B, point C, point D);
};

#endif