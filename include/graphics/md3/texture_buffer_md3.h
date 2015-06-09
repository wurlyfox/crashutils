#ifndef TEXTURE_BUFFER_MD3_H
#define TEXTURE_BUFFER_MD3_H

#include "graphics/texture_buffer.h"
#include "formats/tga.h"

#define MD3_TEXIMAGE_WIDTH  1024
#define MD3_TEXIMAGE_HEIGHT 1024

struct bakedBlock
{
  bool valid:1;
  bool copied:1;
  bool textured:1;
  unsigned char srcX;
  unsigned char srcY;
  unsigned char dstX;
  unsigned char dstY;
  unsigned long color;  
};

struct bakedCoord
{
  fquad2 coords;
  bakedBlock *block;
};
  
struct block_row
{
  int x;
  int h;
  
  int cury;
};

class texture_buffer_md3 : public texture_buffer
{
  private:
  
  unsigned long pixBuf[MD3_TEXIMAGE_WIDTH*MD3_TEXIMAGE_HEIGHT];
  
  int bakedBlockCount[25];
  int bakedCoordCount;
  bakedBlock bakedBlockCache[25][256];
  bakedBlock *bakedBlocks[25][256];
  bakedCoord bakedCoords[4096];

  void copyBlock(bakedBlock *block, int w, int h, 
                 unsigned long *src, unsigned long *dst, 
                 int srcw, int srch, int dstw, int dsth,
                 int dstx, int dsty);
  void copyColorBlock(bakedBlock *block, int w, int h, 
                      unsigned long *dst, 
                      int dstw, int dsth,
                      int dstx, int dsty);

  fpoint2 getUpperLeft(texture *tex, int w, int h);

  public:
 
  texture_buffer_md3(NSD *_nsd, NSF *_nsf) : texture_buffer(_nsd, _nsf) 
  {
    init();
  }
  
  void init();
  void storeBlock(unsigned long *pixBuf, int x, int y, int w, int h);
	fquad2 getBlockCoords(point A, point B, point C, point D);
  
  texture *bakeBlock(texture *tex, float Rf, float Gf, float Bf);
  texture *colorBlock(float Rf, float Gf, float Bf);

  void rebuildBlocks();
  void updateCoords(texture *tex);
  
  void exportTGA(const char *filename);
};

#endif

