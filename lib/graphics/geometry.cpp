#include "geometry.h"

#include "../crash/eid.h"

geometry::geometry(NSD *_nsd, NSF *_nsf)
{
  // we need these so object models can resolve tgeo data from the 
  // entry with eid specified in a svtx; also needed for loading texture
  // chunk data with the texture buffer    
  nsd = _nsd;
  nsf = _nsf;
  
  // implement allocation of these fields in derived classes
  // by allocating objects of the appropriate subclass types
  // in their constructors
  primAlloc     = 0; 
  textureBuffer = 0;
  for (int lp = 0; lp < 0x100; lp++)
  {
    worldBuffer[lp] = 0;
    objectBuffer[lp] = 0;
    spriteBuffer[lp] = 0;
    fragmentBuffer[lp] = 0;
    miscBuffer[lp] = 0;
  }    
 
  // init the caches here
  resetWorldCache();
  resetObjectCache();
  resetSpriteCache();
  resetFragmentCache();
  
  resetMisc();
}

void geometry::reset()
{  
  resetWorldCache();
  resetObjectCache();
  resetSpriteCache();
  resetFragmentCache();
  
  //resetMisc();

  resetTextures();
  
  primAlloc->freePrimitives();
}

void geometry::resetWorldCache()
{
  worldCount = 0;
  
  for (int lp = 0; lp < 0x100; lp++)
    worldCache[lp].EID = EID_NONE;
}

void geometry::resetObjectCache()
{
  objectCount = 0;
  
  for (int lp = 0; lp < 0x100; lp++)
  {
    objectCache[lp].EID = EID_NONE;
   
    for (int lp2 = 0; lp2 < 32; lp2++)
      objectCache[lp].object[lp2] = 0;
  }
}

void geometry::resetSpriteCache()
{
  spriteCount = 0;
  
  for (int lp = 0; lp < 0x100; lp++)
  {
    spriteCache[lp].object = 0;
  }
}  

void geometry::resetFragmentCache()
{
  fragmentCount = 0;
  
  for (int lp = 0; lp < 0x100; lp++)
  {
    fragmentCache[lp].object = 0;
  }
}  

void geometry::resetMisc()
{
  miscCount = 0;
}

model_wld *geometry::getWorld(entry *wgeo)
{
  unsigned long EID        = wgeo->EID;
  unsigned long cacheIndex = (EID >> 15) & 0xFF; 
  
  int newIndex = 0;
  for (int lp = cacheIndex; lp < cacheIndex+0x100; lp++)
  {
    newIndex = lp % 0x100;
    if (worldCache[newIndex].EID == EID_NONE)
    {
      if (worldCount < 0x100)
      {
        worldBuffer[worldCount]->load(wgeo, textureBuffer, primAlloc);
        worldCache[newIndex].EID    = EID;
        worldCache[newIndex].object = worldBuffer[worldCount];
        worldCount++;
      }
 
      break;
    }
    else if (worldCache[newIndex].EID == EID)
    {
      break;
    }      
  }
  
  return worldCache[newIndex].object;
}

model_obj *geometry::getObject(entry *svtx, int frame)
{
  unsigned long EID        = svtx->EID;
  unsigned long cacheIndex = (EID >> 15) & 0xFF; 
  
  int newIndex = 0;
  for (int lp = cacheIndex; lp < cacheIndex+0x100; lp++)
  {
    newIndex = lp % 0x100;
    if (( objectCache[newIndex].EID == EID_NONE || 
          objectCache[newIndex].EID == EID ) &&
        objectCache[newIndex].object[frame] == 0 )
    {
      if (objectCount < 0x100)
      {
			  objectBuffer[objectCount]->init(nsf, nsd);
        objectBuffer[objectCount]->load(svtx, frame, textureBuffer, primAlloc);
        objectBuffer[objectCount]->render = true;
        objectCache[newIndex].EID           = EID;
        objectCache[newIndex].object[frame] = objectBuffer[objectCount];
        objectCount++;
      }
 
      break;
    }
    else if (objectCache[newIndex].EID == EID)
    {
      break;
    }      
  }
  
  return objectCache[newIndex].object[frame];
}

model_spr *geometry::getSprite(unsigned long texEID, unsigned char *texInfoArray, unsigned long scale)
{
  //we need to do a bit of trickery here
  textureEntry texEntry;

  unsigned long texInfoA = getWord(texInfoArray, 0, true);
  unsigned long texInfoB = getWord(texInfoArray, 4, true);
	  
  unsigned short texCoordsOffset = ((texInfoB >> 22) & 0x3FF);
  unsigned short colorMode       = ((texInfoB >> 20) & 3);
  unsigned short section128      = ((texInfoB >> 18) & 3);
    
  unsigned short offsetX = ((texInfoB >> 13) & 0x1F);  
  unsigned short offsetY =         (texInfoB & 0x1F); 
        
  unsigned char clutIndex   = (texInfoB >> 6) & 0x7F;
  unsigned char clutPalette = (texInfoA >> 24) & 0xF;  
      
  unsigned char chunkIndex = textureBuffer->getTextureChunk(texEID);
  
  texEntry.coords      = texCoordsOffset;
  texEntry.colorMode   = colorMode;
  texEntry.sect128     = section128;
  texEntry.offsetX     = offsetX;
  texEntry.offsetY     = offsetY;
  texEntry.clutIndex   = clutIndex;
  texEntry.clutPalette = clutPalette;
  texEntry.chunkIndex  = chunkIndex;
  
  unsigned long cacheIndex = texEntry.coords & 0xFF;
  
  int newIndex = 0;
  for (int lp = cacheIndex; lp < cacheIndex+0x100; lp++)
  {
    newIndex = lp % 0x100;
        
    if (spriteCache[newIndex].object == 0)
    {
      if (spriteCount < 0x100)
      {
        spriteBuffer[spriteCount]->load(texEID, texInfoArray, scale, textureBuffer, primAlloc);
        spriteBuffer[spriteCount]->render = true;
        spriteCache[newIndex].object = spriteBuffer[spriteCount];
        
        spriteCache[newIndex].data.coords      = texCoordsOffset;
        spriteCache[newIndex].data.colorMode   = colorMode;
        spriteCache[newIndex].data.sect128     = section128;
        spriteCache[newIndex].data.offsetX     = offsetX;
        spriteCache[newIndex].data.offsetY     = offsetY;
        spriteCache[newIndex].data.clutIndex   = clutIndex;
        spriteCache[newIndex].data.clutPalette = clutPalette;
        spriteCache[newIndex].data.chunkIndex  = chunkIndex;
        
        spriteCount++; 
      }
      
      break;
    }
    else 
    {
      textureEntry *spriteTex = &spriteCache[newIndex].data;
      
      if (spriteTex->coords      == texCoordsOffset &&
          spriteTex->colorMode   == colorMode       &&
          spriteTex->sect128     == section128      &&
          spriteTex->offsetX     == offsetX         &&
          spriteTex->offsetY     == offsetY         &&
          spriteTex->clutIndex   == clutIndex       &&
          spriteTex->clutPalette == clutPalette     &&
          spriteTex->chunkIndex  == chunkIndex)
      {
        break;
      }
    }
  }

  return spriteCache[newIndex].object;  
}

model_frg *geometry::getFragment(unsigned long texEID, int quadCount, squad2 *quadInfo, unsigned char *texInfoArray, unsigned long scale)
{
  // similarly with the fragment cache; now with multiple texs per frag, we can index with the first tex
  textureEntry texEntry;

  unsigned long texInfoA = getWord(texInfoArray, 0, true);
  unsigned long texInfoB = getWord(texInfoArray, 4, true);
	  
  unsigned short texCoordsOffset = ((texInfoB >> 22) & 0x3FF);
  unsigned short colorMode       = ((texInfoB >> 20) & 3);
  unsigned short section128      = ((texInfoB >> 18) & 3);
    
  unsigned short offsetX = ((texInfoB >> 13) & 0x1F);  
  unsigned short offsetY =         (texInfoB & 0x1F); 
        
  unsigned char clutIndex   = (texInfoB >> 6) & 0x7F;
  unsigned char clutPalette = (texInfoA >> 24) & 0xF;  
      
  unsigned char chunkIndex = textureBuffer->getTextureChunk(texEID);
  
  texEntry.coords      = texCoordsOffset;
  texEntry.colorMode   = colorMode;
  texEntry.sect128     = section128;
  texEntry.offsetX     = offsetX;
  texEntry.offsetY     = offsetY;
  texEntry.clutIndex   = clutIndex;
  texEntry.clutPalette = clutPalette;
  texEntry.chunkIndex  = chunkIndex;
  
  unsigned long cacheIndex = texEntry.coords & 0xFF;
  
  int newIndex = 0;
  for (int lp = cacheIndex; lp < cacheIndex+0x100; lp++)
  {
    newIndex = lp % 0x100;
        
    if (fragmentCache[newIndex].object == 0)
    {
      if (fragmentCount < 0x100)
      {
        fragmentBuffer[fragmentCount]->load(texEID, quadCount, quadInfo, texInfoArray, scale, textureBuffer, primAlloc);
        fragmentBuffer[fragmentCount]->render = true;
        fragmentCache[newIndex].object = fragmentBuffer[fragmentCount];
        
        fragmentCache[newIndex].data.coords      = texCoordsOffset;
        fragmentCache[newIndex].data.colorMode   = colorMode;
        fragmentCache[newIndex].data.sect128     = section128;
        fragmentCache[newIndex].data.offsetX     = offsetX;
        fragmentCache[newIndex].data.offsetY     = offsetY;
        fragmentCache[newIndex].data.clutIndex   = clutIndex;
        fragmentCache[newIndex].data.clutPalette = clutPalette;
        fragmentCache[newIndex].data.chunkIndex  = chunkIndex;
        
        fragmentCount++; 
      }
      
      break;
    }
    else 
    {
      textureEntry *fragmentTex = &fragmentCache[newIndex].data;
      
      if (fragmentTex->coords      == texCoordsOffset &&
          fragmentTex->colorMode   == colorMode       &&
          fragmentTex->sect128     == section128      &&
          fragmentTex->offsetX     == offsetX         &&
          fragmentTex->offsetY     == offsetY         &&
          fragmentTex->clutIndex   == clutIndex       &&
          fragmentTex->clutPalette == clutPalette     &&
          fragmentTex->chunkIndex  == chunkIndex)
      {
        break;
      }
    }
  }

  return fragmentCache[newIndex].object;  
}

model_msc *geometry::getMisc()
{
  return miscBuffer[miscCount++];
}

void geometry::resetTextures()
{
  textureBuffer->resetTextures();
}