#include "graphics/md3/texture_buffer_md3.h"

#include <stdlib.h>

void texture_buffer_md3::init()
{
  for (int lp = 0; lp < 25; lp++)
    bakedBlockCount[lp] = 0;
    
  for (int type = 0; type < 25; type++)
  {
    for (int blk = 0; blk < 256; blk++)
      bakedBlockCache[type][blk].valid = false;
  }
  
  bakedCoordCount = 0;
}

void texture_buffer_md3::storeBlock(unsigned long *pixBuf, int x, int y, int w, int h)
{
  //here we want to store the block directly in a memory pixel buffer, rather than 
  //in video hw framebuffer
  
  int dstw = MD3_TEXIMAGE_WIDTH;
  
  for (int ylp = 0; ylp < h; ylp++)
  {
    for (int xlp = 0; xlp < w; xlp++)
    {
      unsigned long pix = pixBuf[xlp + (ylp * w)];
      
      this->pixBuf[(x + xlp) + ((y + ylp) * dstw)] = pix;
    }
  }
}

fquad2 texture_buffer_md3::getBlockCoords(point A, point B, point C, point D)
{
  //here we will return the coords directly
  fquad2 blockCoords = { { (float)A.X, (float)A.Y },
                         { (float)B.X, (float)B.Y },
                         { (float)C.X, (float)C.Y },
                         { (float)D.X, (float)D.Y } };
                         
  return blockCoords;
}

texture *texture_buffer_md3::bakeBlock(texture *tex, float Rf, float Gf, float Bf)
{
  //convert back the colors to 24bpp
  unsigned char R = Rf * 256;
  unsigned char G = Gf * 256;
  unsigned char B = Bf * 256;
  unsigned long color = RGBA(R,G,B,0);
  
  //get block width and height
  int w_sqr = (int)(tex->coords.C.X - tex->coords.A.X);
  int h_sqr = (int)(tex->coords.C.Y - tex->coords.A.Y);
  if (w_sqr == 0) { w_sqr = (tex->coords.B.X - tex->coords.A.X); }
  if (h_sqr == 0) { h_sqr = (tex->coords.B.Y - tex->coords.A.Y); }
  if (w_sqr < 0) { w_sqr = -w_sqr; }
  if (h_sqr < 0) { h_sqr = -h_sqr; }
  
  //round to nearest power of 2 (implied by adding 1)
  //map powers of 2 starting at 4 to those starting at 1
  int w_sqr2 = (w_sqr + 1) >> 2;
  int h_sqr2 = (h_sqr + 1) >> 2;
  
  //compute log 2 of these to get respective indices of w/h
  //in the power of 2s sequence
  int w = 0;
  int h = 0;
  while (w_sqr2 = w_sqr2 >> 1) { w++; }
  while (h_sqr2 = h_sqr2 >> 1) { h++; }
 
  //get the block group index from both combined indices
  int groupIndex = 24 - ((h * 5) + w);
   
  //get block X and Y
  fpoint2 upperLeft = getUpperLeft(tex, w_sqr, h_sqr);
  int X = (int)upperLeft.X >> 2;
  int Y = (int)upperLeft.Y >> 2;
  
  //compute hash
  int hash = (Y & 0xF0) + (X >> 4);
  
  //search for matching baked block  
  bakedBlock *foundEntry = 0;
  for (int lp = hash; lp < (hash+256); lp++)
  {
    int entryIndex = lp % 256;
    foundEntry = &bakedBlockCache[groupIndex][entryIndex];
    if (foundEntry->valid)
    {
      if (foundEntry->textured == true &&
          foundEntry->srcX     == X    &&
          foundEntry->srcY     == Y    &&
          foundEntry->color    == color)
      {
        //found the matching entry with our block
        break;
      }
    }
    else
    {
      //create a new entry
      foundEntry->valid    = true;    
      foundEntry->copied   = false;
      foundEntry->textured = true;
      foundEntry->srcX     = X;
      foundEntry->srcY     = Y;
      foundEntry->color    = color;
      foundEntry->dstX     = 0;
      foundEntry->dstY     = 0;
      int index = bakedBlockCount[groupIndex]++;
          
      bakedBlocks[groupIndex][index] = foundEntry;
          
      break;
    }
  }
   
  if (foundEntry == 0)
  {
    printf("none found\n");
  }
  
  //make a new coords entry
  bakedCoord *coord = &bakedCoords[bakedCoordCount++];
  coord->coords = tex->coords;
  coord->block  = foundEntry;
  
  //allocate a new tex and replace the tex info with a pointer to the entry
  texture *newTex = allocTexture();
  newTex->info = (void*)coord;
  
  return newTex;
}

texture *texture_buffer_md3::colorBlock(float Rf, float Gf, float Bf)
{
  //convert back the colors to 24bpp
  unsigned char R = Rf * 256;
  unsigned char G = Gf * 256;
  unsigned char B = Bf * 256;
  unsigned long color = RGBA(R,G,B,0);
 
  //get the block group index
  int groupIndex = 24;
  
  //search for matching baked block  
  bakedBlock *foundEntry = 0;
  for (int lp = 0; lp < 256; lp++)
  {
    int entryIndex = lp;
    foundEntry = &bakedBlockCache[groupIndex][entryIndex];
    if (foundEntry->valid)
    {
      if (foundEntry->textured == false &&
          foundEntry->color    == color)
      {
        //found the matching entry with our color
        break;
      }
    }
    else
    {
      //create a new entry
      foundEntry->valid    = true;    
      foundEntry->copied   = false;
      foundEntry->textured = false;
      foundEntry->srcX     = 0;
      foundEntry->srcY     = 0;
      foundEntry->color    = color;
      foundEntry->dstX     = 0;
      foundEntry->dstY     = 0;
      int index = bakedBlockCount[groupIndex]++;
          
      bakedBlocks[groupIndex][index] = foundEntry;
      
      break;
    }
  }
      
  if (foundEntry == 0)
  {
    printf("none found\n");
  }
  
  //make a new coords entry
  bakedCoord *coord = &bakedCoords[bakedCoordCount++];
  coord->coords = { { 0, 0 }, { 3, 0 }, { 3, 3 }, { 0, 3 } };
  coord->block  = foundEntry;
  
  //allocate a new tex and replace the tex info with a pointer to the entry
  texture *newTex = allocTexture();
  newTex->info = (void*)coord;
  
  return newTex;
}

void texture_buffer_md3::rebuildBlocks()
{
  //figure out dimensions of the new pixel buffer
  int bufw = MD3_TEXIMAGE_WIDTH;  //for now
  int bufh = MD3_TEXIMAGE_HEIGHT; //for now
  
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
                    MD3_TEXIMAGE_WIDTH, MD3_TEXIMAGE_HEIGHT, 
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

void texture_buffer_md3::copyColorBlock(bakedBlock *block, int w, int h, 
                                        unsigned long *dst, 
                                        int dstw, int dsth,
                                        int dstx, int dsty)
{
  if (!block->copied)
  {  
    unsigned long pix = block->color;
    
    for (int ylp = 0; ylp < h; ylp++)
    {
      int dstpixy = dsty + ylp;
      for (int xlp = 0; xlp < w; xlp++)
      {
        int dstpixx = dstx + xlp;
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
  
  tex->coords.A.X = ((baked->coords.A.X - srcX) + dstX) / MD3_TEXIMAGE_WIDTH;
  tex->coords.A.Y = ((baked->coords.A.Y - srcY) + dstY) / MD3_TEXIMAGE_HEIGHT;
  tex->coords.B.X = ((baked->coords.B.X - srcX) + dstX) / MD3_TEXIMAGE_WIDTH;
  tex->coords.B.Y = ((baked->coords.B.Y - srcY) + dstY) / MD3_TEXIMAGE_HEIGHT;  
  tex->coords.C.X = ((baked->coords.C.X - srcX) + dstX) / MD3_TEXIMAGE_WIDTH;
  tex->coords.C.Y = ((baked->coords.C.Y - srcY) + dstY) / MD3_TEXIMAGE_HEIGHT; 
  tex->coords.D.X = ((baked->coords.D.X - srcX) + dstX) / MD3_TEXIMAGE_WIDTH;
  tex->coords.D.Y = ((baked->coords.D.Y - srcY) + dstY) / MD3_TEXIMAGE_HEIGHT;
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

void texture_buffer_md3::exportTGA(const char *filename)
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
  header.image_spec.width = MD3_TEXIMAGE_WIDTH;  //for now
  header.image_spec.height = MD3_TEXIMAGE_HEIGHT;
  header.image_spec.bpp = 32;
  header.image_spec.descriptor = 0; //no alpha channel yet
  
  int pixLen = MD3_TEXIMAGE_WIDTH * MD3_TEXIMAGE_HEIGHT;
  
  FILE *tga = fopen(filename, "w+b");
  fwrite(&header, sizeof(tga_header), 1, tga);
  fwrite(&pixBuf, 4, pixLen, tga);
  fclose(tga);
} 
  