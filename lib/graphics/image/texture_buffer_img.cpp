#include "graphics/image/texture_buffer_img.h"

#include <stdlib.h>

void texture_buffer_img::init()
{
  for (int lp = 0; lp < TEXIMAGE_W*TEXIMAGE_H; lp++)
    this->pixBuf[lp] = 0;
}

void texture_buffer_img::storeBlock(unsigned long *pixBuf, int x, int y, int w, int h)
{
  //here we want to store the block directly in a memory pixel buffer, rather than 
  //in video hw framebuffer
  
  int dstw = TEXIMAGE_W;
  
  for (int ylp = 0; ylp < h; ylp++)
  {
    for (int xlp = 0; xlp < w; xlp++)
    {
      unsigned long pix = pixBuf[xlp + (ylp * w)];
      
      this->pixBuf[(x + xlp) + ((y + ylp) * dstw)] = pix | 0xFF000000;
    }
  }
}

fquad2 texture_buffer_img::getBlockCoords(point A, point B, point C, point D)
{
  //here we will return the coords directly
  fquad2 blockCoords = { { IMG_COORDX(A.X), IMG_COORDY(A.Y) },
                         { IMG_COORDX(B.X), IMG_COORDY(B.Y) },
                         { IMG_COORDX(C.X), IMG_COORDY(C.Y) },
                         { IMG_COORDX(D.X), IMG_COORDY(D.Y) } };
                         
  return blockCoords;
}

/*
void texture_buffer_md3::rebuildBlocks()
{
  //figure out dimensions of the new pixel buffer
  int bufw = TEXIMAGE_W;  //for now
  int bufh = TEXIMAGE_H; //for now
  
  //create temp pixel buffer for storing the replacement contents
  unsigned long *tempBuf = (unsigned long*)malloc(bufw*bufh*sizeof(unsigned long));
  
  int curx = 0;
  int cury = 0;
  
  block_row rows[5];
  rows[0].x = 0;
  rows[0].h = 1024;
  rows[0].cury = 0;
  
  int cur_row = 0;
  int row_cury = 0;
  for (int hlp = 0; hlp < 5; hlp++)
  {
    int h = (4 << (4-hlp));
    for (int wlp = 0; wlp < 5; wlp++)
    {
      int w = (4 << (4-wlp));

      int groupIndex = (hlp*5)+wlp;
      for (int lp = 0; lp < bakedBlockCount[groupIndex]; lp++)
      {
        //copy pixels from baked block X, Y to curx, cury
        bakedBlock *block = bakedBlocks[groupIndex][lp];
        
        if (block->textured)
        {
          copyBlock(block, w, h, pixBuf, tempBuf, 
                    MD3_TEXIMAGE_W, MD3_TEXIMAGE_H, 
                    bufw, bufh,
                    curx, cury);
        }
        else
        {
          copyColorBlock(block, w, h, tempBuf, 
                         bufw, bufh,
                         curx, cury);
        }
        
        //update with the block's new copied location
        block->dstX = curx >> 2;
        block->dstY = cury >> 2;                  
        
        curx += w;
        
        if (curx >= bufw)          //if we exceed the width of the buffer/right bound of the row
        {
          curx = rows[cur_row].x;  //wrap around back to the beginning of the row of empty space
          cury += h;               //and down
          
          row_cury += h;
        }
        
        if (row_cury >= rows[cur_row].h) //if we exceed the bottom bound of the row
        {
          cur_row--;
          
          curx     = rows[cur_row].x;     //reset to left of previous row
          row_cury = rows[cur_row].cury;  //back to the point of progress down within that row 
        }
      }
    }
    
    if (curx != 0)
    {
      cur_row++;
      rows[cur_row].x    = curx;
      rows[cur_row].cury = row_cury;
      rows[cur_row].h    = h;
      
      row_cury = 0;
    }
  }   
  
  //replace the original buf with a copy of the temp buf
  //free(pixBuf);
  //pixBuf = (unsigned long*)malloc(bufw*bufh*sizeof(unsigned long));
  for (int lp = 0; lp < bufw*bufh; lp++)
    pixBuf[lp] = tempBuf[lp];
  
  free(tempBuf);  
}            

void texture_buffer_md3::copyBlock(bakedBlock *block, int w, int h, 
                                   unsigned long *src, unsigned long *dst, 
                                   int srcw, int srch, int dstw, int dsth,
                                   int dstx, int dsty)
{
  if (!block->copied)
  {
    int top = (block->srcY << 2);
    int bot = (block->srcY << 2) + h;
    int left = (block->srcX << 2);
    int right = (block->srcX << 2) + w;

    for (int ylp = 0; ylp < h; ylp++)
    {
      int srcpixy = top + ylp;
      int dstpixy = dsty + ylp;
      for (int xlp = 0; xlp < w; xlp++)
      {
        int srcpixx = left + xlp;
        int dstpixx = dstx + xlp;

        unsigned long pix = src[(srcpixx + (srcpixy * srcw))];
        pix += block->color;
        
        dst[(dstpixx + (dstpixy * dstw))] = pix;
      }
    }

    block->copied = true;
  }
}

void texture_buffer_md3::updateCoords(texture *tex)
{
  //cast the coords to their coord entry
  bakedCoord *baked = (bakedCoord*)tex->info;
  
  //recalculate the new coords based on the change from src to dst of the
  //associated baked block
  bakedBlock *block = baked->block;
  unsigned short srcX = (block->srcX << 2);
  unsigned short srcY = (block->srcY << 2);
  unsigned short dstX = (block->dstX << 2);
  unsigned short dstY = (block->dstY << 2);
  
  tex->coords.A.X = ((baked->coords.A.X - srcX) + dstX) / MD3_TEXIMAGE_W;
  tex->coords.A.Y = ((baked->coords.A.Y - srcY) + dstY) / MD3_TEXIMAGE_H;
  tex->coords.B.X = ((baked->coords.B.X - srcX) + dstX) / MD3_TEXIMAGE_W;
  tex->coords.B.Y = ((baked->coords.B.Y - srcY) + dstY) / MD3_TEXIMAGE_H;  
  tex->coords.C.X = ((baked->coords.C.X - srcX) + dstX) / MD3_TEXIMAGE_W;
  tex->coords.C.Y = ((baked->coords.C.Y - srcY) + dstY) / MD3_TEXIMAGE_H; 
  tex->coords.D.X = ((baked->coords.D.X - srcX) + dstX) / MD3_TEXIMAGE_W;
  tex->coords.D.Y = ((baked->coords.D.Y - srcY) + dstY) / MD3_TEXIMAGE_H;
}
  
fpoint2 texture_buffer_md3::getUpperLeft(texture *tex, int w, int h)
{
  fpoint2 upperLeft;
  
  //find upper left
  if (tex->coords.A.X < tex->coords.B.X)  //coords A is upper or lower left
  {
    if (tex->coords.A.Y <= tex->coords.C.Y)
    {
      if (tex->coords.C.Y > tex->coords.B.Y)
      {
        upperLeft.X = tex->coords.B.X - w;
        upperLeft.Y = tex->coords.B.Y;
      }
      else
        upperLeft = tex->coords.A;
    }
    else
    {
      if (tex->coords.C.X > tex->coords.A.X)
      {
        upperLeft.X = tex->coords.C.X - w;
        upperLeft.Y = tex->coords.C.Y;
      }
      else
        upperLeft = tex->coords.C;
    }
  }
  else if (tex->coords.A.X == tex->coords.B.X)
  {
    if (tex->coords.A.Y < tex->coords.B.Y)
    {
      if (tex->coords.C.X > tex->coords.A.X)
        upperLeft = tex->coords.A;
      else 
      {
        upperLeft.X = tex->coords.A.X - w;
        upperLeft.Y = tex->coords.A.Y;
      }
    }
    else if (tex->coords.A.Y > tex->coords.B.Y)
    {
      if (tex->coords.B.X < tex->coords.C.X)
        upperLeft = tex->coords.B;
      else
      {
        if (tex->coords.C.Y > tex->coords.B.Y)
        {
          upperLeft.X = tex->coords.B.X - w;
          upperLeft.Y = tex->coords.B.Y;
        }
        else
          upperLeft = tex->coords.C;
      }
    }
  }
  else
  {
    if (tex->coords.B.Y > tex->coords.C.Y)
    {
      if (tex->coords.C.X > tex->coords.B.X)
      {
        upperLeft.X = tex->coords.B.X;
        upperLeft.Y = tex->coords.B.Y - h;        
      }
      else
        upperLeft = tex->coords.C;
    }
    else if (tex->coords.B.Y < tex->coords.C.Y)
      upperLeft = tex->coords.B;
    else
    {
      if (tex->coords.A.Y > tex->coords.C.Y)
        upperLeft = tex->coords.B;
      else
      {
        upperLeft.X = tex->coords.A.X - w;
        upperLeft.Y = tex->coords.A.Y;
      }
    }
  } 

  return upperLeft;
}
*/

void texture_buffer_img::exportTGA(const char *filename)
{
  //create the TGA file header
  tga_header header;
  header.id_length = 0;     //no id
  header.colormap_type = 0; //no color map
  header.image_type = 2;    //RAW pixel data
  
  //no color map
  header.colormap_spec.first_entry_index = 0;
  header.colormap_spec.length = 0;
  header.colormap_spec.bpp = 0;
  
  //fill image specs
  header.image_spec.x_origin = 0;
  header.image_spec.y_origin = 0;
  header.image_spec.width = TEXIMAGE_W;  //for now
  header.image_spec.height = TEXIMAGE_H;
  header.image_spec.bpp = 32;
  header.image_spec.descriptor = 0; //no alpha channel yet
  
  int pixLen = TEXIMAGE_W * TEXIMAGE_H;
  
  FILE *tga = fopen(filename, "w+b");
  fwrite(&header, sizeof(tga_header), 1, tga);
  fwrite(&pixBuf, 4, pixLen, tga);
  fclose(tga);
} 
  