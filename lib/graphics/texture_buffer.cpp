#include "texture_buffer.h"

//occupy a texEntry with correct values from rom to use getTexture
void texture_buffer::initAll()
{  
	subTextureCount = 0;
	
  initCluts();
  initTextureCache();
  initSubtextureGroups(false);
}

void texture_buffer::resetTextures()
{
  subTextureCount = 0;
  
  initTextureCache();
  initSubtextureGroups(true);
}

void texture_buffer::initTextureCache()
{
  for (int lp = 0; lp < CACHE_SIZE_TEXTURE; lp++)
  {
    textureCache[lp].subTexture = 0;
    textureCacheSorted[lp] = 0;
  }
}

void texture_buffer::initSubtextureGroups(bool reallocate)
{
  for (int lp=0; lp < 25; lp++)
  {
    //calculate width and height of the block
    int w = (1 << (lp % 5)) * 4;
    int h = (1 << ((lp / 5) % 5)) * 4;
    
    if ((w == 4) && (h == 32))
    {
      //allocate space for 64 blocks
      subTexGroups[lp].usedBlocks  =   0;
      subTexGroups[lp].allocBlocks =  64;
      if (reallocate)
        free(subTexGroups[lp].blocks);
   
      subTexGroups[lp].blocks = (subTextureBlock*)malloc(sizeof(subTextureBlock)*256);

      unsigned short xLoc = 0;
      unsigned short yLoc = 256;

      int counter = 0;
      for (int ylp = 0; ylp < 1; ylp++)
      {
        for (int xlp = 0; xlp < 64; xlp++)
        {
          subTexGroups[lp].blocks[counter].X        = xLoc + (w*xlp);
          subTexGroups[lp].blocks[counter].Y        = yLoc + (h*ylp);
          subTexGroups[lp].blocks[counter].texEntry = 0;
          counter++;
        }
      }        
    }
    else if (((h == 64) || (h == 32)) && 
       ((w == 4) || (w == 8) || (w == 16)))
    {
      subTexGroups[lp].usedBlocks = 0;
      subTexGroups[lp].allocBlocks = 0;
      
      if (reallocate)
        free(subTexGroups[lp].blocks);
        
      subTexGroups[lp].blocks = 0;
      
    }
    else if (w == 32 && h == 32)
    {
      //allocate space for 256 blocks
      subTexGroups[lp].usedBlocks  =   0;
      subTexGroups[lp].allocBlocks = 256;
      if (reallocate)
        free(subTexGroups[lp].blocks);
   
      subTexGroups[lp].blocks      = (subTextureBlock*)malloc(sizeof(subTextureBlock)*256);

      unsigned short xLoc = 256;
      unsigned short yLoc = 0;

      int counter = 0;
      for (int ylp = 0; ylp < 16; ylp++)
      {
        for (int xlp = 0; xlp < 16; xlp++)
        {
          subTexGroups[lp].blocks[counter].X        = xLoc + (w*xlp);
          subTexGroups[lp].blocks[counter].Y        = yLoc + (h*ylp);
          subTexGroups[lp].blocks[counter].texEntry = 0;
          counter++;
        }
      }        
    }
    else
    {
    
      //allocate space for 64 blocks
      subTexGroups[lp].usedBlocks  =  0;
      subTexGroups[lp].allocBlocks = 64;
      if (reallocate)
        free(subTexGroups[lp].blocks);
   
      subTexGroups[lp].blocks      = (subTextureBlock*)malloc(sizeof(subTextureBlock)*64);

      unsigned short xLoc = groupLocations[lp].X;
      unsigned short yLoc = groupLocations[lp].Y;

      int counter = 0;
      for (int ylp = 0; ylp < 8; ylp++)
      {
        for (int xlp = 0; xlp < 8; xlp++)
        {
          subTexGroups[lp].blocks[counter].X        = xLoc + (w*xlp);
          subTexGroups[lp].blocks[counter].Y        = yLoc + (h*ylp);
          subTexGroups[lp].blocks[counter].texEntry = 0;
          counter++;
        }
      }        
    }      
  }
}

unsigned char texture_buffer::getTextureChunk(unsigned long EID)
{
  unsigned char index = (unsigned char)nsf->lookupTextureChunkIndex(EID);
  
  return index;
}

texture *texture_buffer::getTexture(textureEntry *texEntry)
{
  //ccccssyyyyyxxxxx 

  int hash = (((texEntry->chunkIndex << 2) | (texEntry->sect128)) << 8) % CACHE_SIZE_TEXTURE;
  
  int searchStart = hash;
  int searchEnd   = hash + CACHE_SIZE_TEXTURE;  //decrease value to improve performance
                                                             //                  loss of accuracy
  textureCacheEntry *freeEntry;
  
  bool cacheFull = true;
  for (int lp=searchStart; lp<searchEnd; lp++)
  {
    int cacheIndex = lp % CACHE_SIZE_TEXTURE;
    if (textureCache[cacheIndex].subTexture == 0)
    {
      freeEntry = &textureCache[cacheIndex];
      cacheFull = false;
      break;
    }    
    else if (texEntry->coords == textureCache[cacheIndex].coords)
    {
      if (
          texEntry->offsetX     == textureCache[cacheIndex].offsetX     &&
          texEntry->offsetY     == textureCache[cacheIndex].offsetY     &&
          texEntry->clutIndex   == textureCache[cacheIndex].clutIndex   &&
          texEntry->clutPalette == textureCache[cacheIndex].clutPalette &&
          texEntry->chunkIndex  == textureCache[cacheIndex].chunkIndex  &&
          texEntry->sect128     == textureCache[cacheIndex].sect128     &&
          texEntry->colorMode   == textureCache[cacheIndex].colorMode            
         )
      {
        return &subTextures[textureCache[cacheIndex].subTexture];
      }
    }
  }

  if (!cacheFull)
  {
    //record cache entry
    freeEntry->coords      = texEntry->coords;
    freeEntry->offsetX     = texEntry->offsetX;    
    freeEntry->offsetY     = texEntry->offsetY;  
    freeEntry->clutIndex   = texEntry->clutIndex;    
    freeEntry->clutPalette = texEntry->clutPalette;
    freeEntry->chunkIndex  = texEntry->chunkIndex;
    freeEntry->sect128     = texEntry->sect128;   
    freeEntry->colorMode   = texEntry->colorMode;              

    //subTexture of index >= 1 means a valid entry
    //create the new subtexture and store at subtexture index - 1, involves gl subtexture and finding next unused
    //free region which the subtexture can be placed

    //.........................
    freeEntry->subTexture = ++subTextureCount;
    
    //buildTextures();
    subTextureCreate(freeEntry);
    
    return &subTextures[freeEntry->subTexture];
  }
  else
  {
    printf("ERROR: requested texture cannot be created; texture cache is full");
    return &subTextures[0];
  }
}      
      
void texture_buffer::subTextureCreate(textureCacheEntry *tex)
{
  unsigned char groupIndex = tex->coords % 25;
  subTextureGroup *group = &subTexGroups[groupIndex];
  
  int w = (1 << (groupIndex % 5)) * 4;
  int h = (1 << ((groupIndex / 5) % 5)) * 4;
  subTextureBlock *block;
  
  //determine whether we need to do a vram copy 
  //if not the texture entry already has a block created in vram for a 
  //different region variant but at the same block coordinates, thus 
  //that block need not be copied to vram multiple times for each region
  //variant;
  bool vramCopy = true;
  
  for (int lp = 0; lp < group->usedBlocks; lp++)
  {
    textureEntry *texEntry = group->blocks[lp].texEntry;
    
    if (texEntry)
    {
      if (
          texEntry->offsetX     == tex->offsetX     &&
          texEntry->offsetY     == tex->offsetY     &&
          texEntry->clutIndex   == tex->clutIndex   &&
          texEntry->clutPalette == tex->clutPalette &&
          texEntry->chunkIndex  == tex->chunkIndex  &&
          texEntry->sect128     == tex->sect128     &&
          texEntry->colorMode   == tex->colorMode            
         )
      {
        vramCopy = false;
        block = &group->blocks[lp];
        break;
      }
    }
  }
  
  if (vramCopy)
  {  
    while (true)
    {
      if (group->usedBlocks < group->allocBlocks)
      {
        int blockIndex = group->usedBlocks;
        group->usedBlocks = group->usedBlocks + 1;
        block = &group->blocks[blockIndex];
      
        block->texEntry = tex; 
        break;
      }
      else
      {
        //.. split allocate etc
        //return;
        //determine which group to steal from based on
        //A) how many blocks the group already has allocated
        //B) which would be faster to split/combine

        //freeBlocks = alloc - used

        //freeBlocks/alloc = compare ratio   weigh 75%
        //dist from other group = 5 - ((curGroup % 5) - (oGroup % 5)) / 10
        //                      + 5 - (((curGroup / 5) % 5) - ((oGroup / 5) % 5)) / 10
        signed short groupPriority[25];

        for (int lp = 0; lp < 25; lp++)
        {
          subTextureGroup *lpGroup = &subTexGroups[lp];

          unsigned short freeBlocks = (lpGroup->allocBlocks - lpGroup->usedBlocks);
          unsigned short allocRatio = (unsigned short)(((float)freeBlocks / lpGroup->allocBlocks) * 75);

          unsigned short grouplogW = (lp % 5);
          unsigned short grouplogH = (lp / 5) % 5;
          
          unsigned short curGrouplogW = (groupIndex % 5);
          unsigned short curGrouplogH = (groupIndex / 5) % 5;

          signed char grouplogDistX = (curGrouplogW - grouplogW);
          signed char grouplogDistY = (curGrouplogH - grouplogH);
          float distX, distY;
          
          if (grouplogDistX >= 0)
            distX = ((float)(10 - grouplogDistX) / 20);
          else
            distX = ((float)(6 + grouplogDistX) / 20);
          
          if (grouplogDistY >= 0)
            distY = ((float)(10 - grouplogDistY) / 20);
          else
            distY = ((float)(6 + grouplogDistY) / 20);
          
          signed short distRatio = (distX + distY) * 25;

          groupPriority[lp] = allocRatio + distRatio;
        }

        bool success = false;
        while (!success)
        {
          signed char hiPri = -1;
          for (int lp = 0; lp < 25; lp++)
          {
            if (hiPri == -1 || groupPriority[hiPri] < groupPriority[lp])
            {
              if (lp != groupIndex && subTexGroups[lp].allocBlocks)
                hiPri = lp;
            }
          }

          unsigned short grouplogW = (hiPri % 5);
          unsigned short grouplogH = (hiPri / 5) % 5;
            
          unsigned short curGrouplogW = (groupIndex % 5);
          unsigned short curGrouplogH = (groupIndex / 5) % 5;

          if (grouplogW > curGrouplogW && grouplogH >= curGrouplogH ||
              grouplogW >= curGrouplogW && grouplogH > curGrouplogH)
          {
            success = splitBlock(hiPri, groupIndex);
          }
          else if (grouplogW < curGrouplogW && grouplogH <= curGrouplogH ||
                   grouplogW <= curGrouplogW && grouplogH < curGrouplogH)
          {
            point2 freePoint;
            if (success = getFreeBlocks(hiPri, groupIndex, freePoint))
              combineBlocks(hiPri, groupIndex, freePoint);
          }
          
          groupPriority[hiPri] = 0;
        }
      }
    } 

    unsigned long pixBuf[w*h*2]; 

    subTexturePixels(tex, pixBuf);
    storeBlock(pixBuf, block->X, block->Y, w, h);
  }
  
  point texA, texB, texC, texD;
  getCoords(tex->coords, texA, texB, texC, texD);
  
  texA.X += block->X; texB.X += block->X; texC.X += block->X; texD.X += block->X;
  texA.Y += block->Y; texB.Y += block->Y; texC.Y += block->Y; texD.Y += block->Y;
  
  subTextures[tex->subTexture].coords = getBlockCoords(texA, texB, texC, texD);
}

bool texture_buffer::splitBlock(int srcGroup, int dstGroup)
{
  unsigned char srcW = (1 << (srcGroup % 5)) * 4;
  unsigned char srcH = (1 << ((srcGroup / 5) % 5)) * 4;  
  unsigned char dstW = (1 << (dstGroup % 5)) * 4;
  unsigned char dstH = (1 << ((dstGroup / 5) % 5)) * 4;  

  subTextureGroup *dst       = &subTexGroups[dstGroup];
  subTextureBlock *dstBlocks = dst->blocks;
  subTextureGroup *src       = &subTexGroups[srcGroup];
  subTextureBlock *srcBlocks = src->blocks;

  unsigned short dstAlloc    = dst->allocBlocks;
  unsigned short srcAlloc    = src->allocBlocks;

  signed short freeBlock = srcAlloc - 1;
  while (srcBlocks[freeBlock].texEntry)
  {
    freeBlock--;
    if (freeBlock == -1) { return false; }
  }
    
  unsigned short blockX = srcBlocks[freeBlock].X;
  unsigned short blockY = srcBlocks[freeBlock].Y;

  int splitX = srcW / dstW;
  int splitY = srcH / dstH;

  if (splitX == 2 && splitY == 1)
  {
    src->allocBlocks--;
    
    addFreeBlock(dstGroup, {       blockX, blockY});
    addFreeBlock(dstGroup, {blockX + dstW, blockY});
  }
  else if (splitX == 1 && splitY == 2)
  {
    src->allocBlocks--;
    
    addFreeBlock(dstGroup, {blockX, blockY});
    addFreeBlock(dstGroup, {blockX, blockY + dstH});
  }
  else
  {
    if (splitX != 1)
    {
      splitBlock(srcGroup, srcGroup - 1);
      splitBlock(srcGroup - 1, dstGroup);
    }
    if (splitY != 1)
    {
      splitBlock(srcGroup, srcGroup - 5);
      splitBlock(srcGroup - 5, dstGroup);
    }
  }
  
  return true;
}

bool texture_buffer::combineBlocks(int srcGroup, int dstGroup, point2 loc)
{
  unsigned char srcW = (1 << (srcGroup % 5)) * 4;
  unsigned char srcH = (1 << ((srcGroup / 5) % 5)) * 4;  
  unsigned char dstW = (1 << (dstGroup % 5)) * 4;
  unsigned char dstH = (1 << ((dstGroup / 5) % 5)) * 4;  

  subTextureGroup *src    = &subTexGroups[srcGroup];
  subTextureBlock *srcBlocks = src->blocks;
  
  unsigned short srcAlloc = src->allocBlocks;

  //if we're trying to combine smaller blocks into larger ones
  if ((srcW < dstW && srcH < dstH)  ||
      (srcW < dstW && srcH == dstH) ||
      (srcW == dstW && srcH < dstH))
  {
    unsigned char consecX = dstW / srcW;
    unsigned char consecY = dstH / srcH;

    unsigned short delX1 = loc.X;
    unsigned short delY1 = loc.Y;
    unsigned short delX2 = loc.X + (srcW*consecX);
    unsigned short delY2 = loc.Y + (srcH*consecY);
 
    int deleteCount = 0;
    int newSrcCount = 0;
    for (int lp=0; lp < srcAlloc; lp++)
    {
      
      if (srcBlocks[lp].X >= delX1 && srcBlocks[lp].X < delX2 &&
          srcBlocks[lp].Y >= delY1 && srcBlocks[lp].Y < delY2)
      {
        if (srcBlocks[lp].texEntry) { return false; }
        deleteCount++;
      }
    }

    if (deleteCount != (consecX*consecY))
      return false;

    for (int lp=0; lp < srcAlloc; lp++)
    {
      if (srcBlocks[lp].X >= delX1 && srcBlocks[lp].X < delX2 &&
          srcBlocks[lp].Y >= delY1 && srcBlocks[lp].Y < delY2)
      {
        
      }
      else
      { 
        srcBlocks[newSrcCount].X        = srcBlocks[lp].X;
        srcBlocks[newSrcCount].Y        = srcBlocks[lp].Y;
        srcBlocks[newSrcCount].texEntry = srcBlocks[lp].texEntry;

        newSrcCount++;
      }
    }

    src->allocBlocks = newSrcCount;

    addFreeBlock(dstGroup, loc);

    return true;
  }
}

unsigned short texture_buffer::addFreeBlock(int dstGroup, point2 loc)
{
  subTextureGroup *dst = &subTexGroups[dstGroup];

  int shamt = 0;
  unsigned short size  = dst->allocBlocks;
  unsigned short index = 0;
  
  if (size == 0)
  {
    dst->blocks = (subTextureBlock*)malloc(16*(sizeof(subTextureBlock)));
    dst->allocBlocks++;
  }
  else if (size < 16) 
    index = dst->allocBlocks++;
  else
  {
    while (size >>= 1) { shamt++; }
    unsigned short allocPoint = (1 << (shamt));

    index = dst->allocBlocks++;
      
    if (index == allocPoint)
    {
      subTextureBlock *temp = (subTextureBlock*)malloc(allocPoint*sizeof(subTextureBlock));
      
      for (int lp = 0; lp < allocPoint; lp++)
      {
        temp[lp].X = dst->blocks[lp].X;
        temp[lp].Y = dst->blocks[lp].Y;
        temp[lp].texEntry = dst->blocks[lp].texEntry;
      }
      
      free(dst->blocks);   
      dst->blocks = (subTextureBlock*)malloc(allocPoint*2*(sizeof(subTextureBlock)));

      for (int lp = 0; lp < allocPoint; lp++)
      {
        dst->blocks[lp].X = temp[lp].X;
        dst->blocks[lp].Y = temp[lp].Y;
        dst->blocks[lp].texEntry = temp[lp].texEntry;
      }
        
      free(temp);
    }
  }
  
  dst->blocks[index].X        = loc.X;
  dst->blocks[index].Y        = loc.Y;
  dst->blocks[index].texEntry = 0;

  return index;
}
      
bool texture_buffer::getFreeBlocks(int srcGroup, int dstGroup, point2 &loc)
{
  unsigned char srcW = (1 << (srcGroup % 5)) * 4;
  unsigned char srcH = (1 << ((srcGroup / 5) % 5)) * 4;  
  unsigned char dstW = (1 << (dstGroup % 5)) * 4;
  unsigned char dstH = (1 << ((dstGroup / 5) % 5)) * 4;  

  subTextureGroup *src    = &subTexGroups[srcGroup];
  subTextureBlock *srcBlocks = src->blocks;
  
  unsigned short srcAlloc = src->allocBlocks;
  
  //if we're trying to combine smaller blocks into larger ones
  if (srcW < dstW && srcH < dstH  ||
      srcW < dstW && srcH == dstH ||
      srcW == dstW && srcH < dstH)
  {
    unsigned char consecX = dstW / srcW;
    unsigned char consecY = dstH / srcH;

    //first build the block map
    unsigned long blockMap[0x800];
    for (int lp = srcAlloc - 1; lp >= 0; lp--)
    {
      unsigned short X = srcBlocks[lp].X;
      unsigned short Y = srcBlocks[lp].Y;

      unsigned char bit    = (X/4) % 32;
      unsigned short index = ((X/4) / 32) + ((Y/4)*8);
      blockMap[index] |= (0x80000000 >> bit);
    }
 
    int foundIndex   =    -1;
    bool tryNext     = false; 
    for (int lp = srcAlloc - 1; lp >= 0; lp--)
    {
      unsigned short X = srcBlocks[lp].X;
      unsigned short Y = srcBlocks[lp].Y;

      if ((X - srcW) < 0 || (Y - srcH) < 0)   //if too far left or up
        break;

      tryNext = false;
      for (int tstY = 0; tstY < consecY; tstY++)
      {
        unsigned short Yoff = (tstY*srcH);
        unsigned short newY = Y - Yoff;
         
        for (int tstX = 0; tstX < consecX; tstX++)
        {
          unsigned short Xoff = (tstX*srcW);
          unsigned short newX = X - Xoff;
                      
          unsigned char bit    = (newX/4) % 32;
          unsigned short index = ((newX/4) / 32) + ((newY/4)*8);
          
          tryNext = !(blockMap[index] & (0x80000000 >> bit));
        
          if (tryNext)
            break;
        }

        if (tryNext)
          break;
      }
              
      if (!tryNext)
      {
        foundIndex = lp;
        break;
      }
    }

    if (foundIndex != -1)
    {
      loc.X = srcBlocks[foundIndex].X - ((consecX-1)*srcW);
      loc.Y = srcBlocks[foundIndex].Y - ((consecY-1)*srcH);
    
      return true;
    }
  }

  return false;
}

void texture_buffer::getCoords(int index, point &A, point &B, point &C, point &D)
{
  unsigned char AX = (texCoords[index][0] & 0x00FF);
  unsigned char BX = (texCoords[index][1] & 0x00FF);
  unsigned char CX = (texCoords[index][2] & 0x00FF);
  unsigned char DX = (texCoords[index][3] & 0x00FF);

  unsigned char AY = ((texCoords[index][0] & 0xFF00) >> 8);
  unsigned char BY = ((texCoords[index][1] & 0xFF00) >> 8);
  unsigned char CY = ((texCoords[index][2] & 0xFF00) >> 8);
  unsigned char DY = ((texCoords[index][3] & 0xFF00) >> 8);
  
  A.X = AX;
  B.X = BX;
  C.X = CX;
  D.X = DX;
  
  A.Y = AY;
  B.Y = BY;
  C.Y = CY;
  D.Y = DY;
}

bool texture_buffer::subTexturePixels(textureEntry *tex, unsigned long *pixBuf)
{
  chunk *chunk            = nsf->textureChunks[tex->chunkIndex];
  unsigned char *pixels   = chunk->data;
  unsigned long *clut     = &cluts[tex->chunkIndex][tex->clutIndex][tex->clutPalette*16];
  unsigned char colorMode = tex->colorMode;
  
  unsigned char width     = (1 << (tex->coords % 5))       * 4;
  unsigned char height    = (1 << ((tex->coords / 5) % 5)) * 4;
  
  //max(32)*4=max(128)
  unsigned short chunkX = ((tex->offsetX * 4) << (1-colorMode)) + (tex->sect128 * (128 << (1 - colorMode)));
  unsigned short chunkY = tex->offsetY * 4;  
  //unsigned long *pixBuf = (unsigned long*)malloc(width*height*sizeof(unsigned long));
  //unsigned long pixBuf[width*height*2];
   
  //8 bit mode = 512x128 pixels @ 2 pixels/hword = 256 hwords per line
  if (colorMode == 1)
  {
    for (int ylp = 0; ylp < height; ylp++)
    {
      unsigned short yoffset = (chunkY + ylp);
      for (int xlp = 0; xlp < width; xlp+=2)
      {  
        unsigned short xoffset = (chunkX + xlp);
        unsigned short offset = (yoffset*256) + (xoffset/2); //hwords

        unsigned short pixelAB = getHword(pixels, offset*2, true);
          
        unsigned char pixelA = (pixelAB & 0x00FF);
        unsigned char pixelB = ((pixelAB & 0xFF00) >> 8);
        
        pixBuf[xlp + 0 + ((height-ylp-1)*width)] = clut[pixelA];
        pixBuf[xlp + 1 + ((height-ylp-1)*width)] = clut[pixelB];   
      }
    }
  }
  else if (colorMode == 0)
  {
    for (int ylp = 0; ylp < height; ylp++)
    {
      unsigned short yoffset = (chunkY + ylp);
      for (int xlp = 0; xlp < width; xlp+=4)
      {  
        unsigned short xoffset = (chunkX + xlp);
        unsigned short offset = (yoffset*256) + (xoffset/4); //hwords

        unsigned short pixelABCD = getHword(pixels, offset*2, true);
          
        unsigned char pixelA = (pixelABCD & 0x000F);
        unsigned char pixelB = ((pixelABCD & 0x00F0) >> 4);
        unsigned char pixelC = ((pixelABCD & 0x0F00) >> 8);
        unsigned char pixelD = ((pixelABCD & 0xF000) >> 12);
        
        pixBuf[xlp + 0 + ((height-ylp-1)*width)] = clut[pixelA];
        pixBuf[xlp + 1 + ((height-ylp-1)*width)] = clut[pixelB]; 
        pixBuf[xlp + 2 + ((height-ylp-1)*width)] = clut[pixelC];
        pixBuf[xlp + 3 + ((height-ylp-1)*width)] = clut[pixelD];         
      }
    }
  }
  else
  {
    return false;
  }	

  return true;    
}

void texture_buffer::initCluts()
{
  for (int chunk = 0; chunk < 8/*nsf->textureCount*/; chunk++)
  {
    for (int clut = 0; clut < 16; clut++)
	    addClut(chunk, clut);
  }
}

void texture_buffer::addClut(unsigned char chunkIndex, unsigned char clutIndex)
{
  unsigned char *colors = nsf->textureChunks[chunkIndex]->data;
  
  int numColors = 256;
  
  for (int count = 0; count < numColors; count++)
  {
    unsigned int indexOffset = clutIndex * 256;
	  unsigned int totalOffset = (indexOffset + count) * 2;
    unsigned long color = getHword(colors, totalOffset, true);

    unsigned char RP = (color & 0x1F);
    unsigned char GP = ((color & 0x3E0) >> 5);
    unsigned char BP = ((color & 0x7C00) >> 10);
	  unsigned char AP = ((color & 0x8000) >> 15);
	
    /*
     Intensity        PC      PSX
       Minimum          0       0
       Medium (circa)   16      8
       Maximum          31      31
      */
    
    //0 - 16  - 32  
    //0 - 128 - 256  (*8)
    
    //0 - 8 on PSX (*2) maps to 0 - 16 PC (for < 8) ( * 8 for total pc val)
    //8 - 31 on PSX (+8 * 4/3)
	
    
    unsigned char R, G, B, A;
    if (RP < 8) { R = RP * 16; } else { R = 128 + ((RP-8) * (16/3)); }
    if (GP < 8) { G = GP * 16; } else { G = 128 + ((GP-8) * (16/3)); }
    if (BP < 8) { B = BP * 16; } else { B = 128 + ((BP-8) * (16/3)); }
    
    if (AP == 0 && RP == 0 && GP == 0 && BP == 0) { A = 0; }
    else { A = 255; }
    
    cluts[chunkIndex][clutIndex][count] = RGBA(R, G, B, A);
  }
  
  clutCount++;
}
