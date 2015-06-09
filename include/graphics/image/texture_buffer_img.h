#ifndef TEXTURE_BUFFER_IMG_H
#define TEXTURE_BUFFER_IMG_H

#include "graphics/texture_buffer.h"
#include "formats/tga.h"

#define TEXIMAGE_W  1024
#define TEXIMAGE_H 1024

#define IMG_COORDX(P) ((float)P/TEXIMAGE_W)    
#define IMG_COORDY(P) ((float)P/TEXIMAGE_H)

class texture_buffer_img : public texture_buffer
{
  private:
  
  unsigned long pixBuf[TEXIMAGE_W*TEXIMAGE_H];
  
  //void copyBlock(bakedBlock *block, int w, int h, 
  //               unsigned long *src, unsigned long *dst, 
  //               int srcw, int srch, int dstw, int dsth,
  //               int dstx, int dsty);

  //fpoint2 getUpperLeft(texture *tex, int w, int h);

  public:
 
  texture_buffer_img(NSD *_nsd, NSF *_nsf) : texture_buffer(_nsd, _nsf) 
  {
    init();
  }
  
  void init();
  void storeBlock(unsigned long *pixBuf, int x, int y, int w, int h);
	fquad2 getBlockCoords(point A, point B, point C, point D);

  //void rebuildBlocks();
  //void updateCoords(texture *tex);
  
  void exportTGA(const char *filename);
};

#endif

