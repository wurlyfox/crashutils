//provides the interface for geometric models (i.e. graphics system-friendly [converted] equivalent of model entries from the crash NSF data)
//(including memory and allocation for their polygons, vertices, normals, texture coords, and etc)

//any requests for geometry (non-instance model) via EID shall be directed here; time shall not be wasted re-converting the same 
//entry data for each subsequent request of geometry corresponding to the same EID. thus, a cache is kept for each request, or 3
//= 1 per 'type' of geometry (WGEO, TGEO, or sprite which is actually non-attributable to a specific EID-so sprite cache is indexed differently)

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "prim_alloc.h"
#include "model_wld.h"
#include "model_obj.h"
#include "model_spr.h"
#include "model_frg.h"
#include "model_msc.h"
#include "texture_buffer.h"

template <typename T, typename U>
struct g_dataCacheEntry
{
  U data;
  T *object;
};

template <typename T>
struct g_entryCacheEntry
{
  unsigned long EID;
  T *object;
};
  
template <typename T>
struct g_itemCacheEntry
{
  unsigned long EID;
  T *object[32];
};

class geometry
{
  friend class scene;
  friend class  zone;
  friend class scene_gl;
  friend class context_wgl;
  
  protected:
  NSF *nsf;
	NSD *nsd;

  long worldCount;
  long objectCount;
  long spriteCount;
  long fragmentCount;

  model_wld *worldBuffer[0x100];    //[0x100];
  model_obj *objectBuffer[0x100];   //[0x100];
  model_spr *spriteBuffer[0x100];   //[0x100];
  model_frg *fragmentBuffer[0x100];
  
  long miscCount;
  model_msc *miscBuffer[0x100];
  
  g_entryCacheEntry<model_wld> worldCache[0x100];
  g_itemCacheEntry<model_obj> objectCache[0x100];
  g_dataCacheEntry<model_spr, textureEntry> spriteCache[0x100];
  g_dataCacheEntry<model_frg, textureEntry> fragmentCache[0x100];
  
  texture_buffer *textureBuffer; 
  
  public:

  prim_alloc *primAlloc;  

  geometry(NSD *_nsd, NSF *_nsf);
  virtual ~geometry() {};
  
  void reset();
  
  void resetWorldCache();
  void resetObjectCache();
  void resetSpriteCache();
  void resetFragmentCache();
  
  int getWorldCount() { return worldCount; }
  int getObjectCount() { return objectCount; }
  int getSpriteCount() { return spriteCount; }
  int getFragmentCount() { return fragmentCount; }
  
  model_wld *getWorld(entry *wgeo);
  model_wld *getWorld(int index) { return worldBuffer[index]; }
  model_obj *getObject(entry *svtx, int frame);
  model_obj *getObject(int index) { return objectBuffer[index]; }
  model_spr *getSprite(unsigned long texEID, unsigned char *texInfoArray, unsigned long scale);  
  model_spr *getSprite(int index) { return spriteBuffer[index]; }
  model_frg *getFragment(unsigned long texEID, int quadCount, squad2 *quadInfo, unsigned char *texInfoArray, unsigned long scale);  
  model_frg *getFragment(int index) { return fragmentBuffer[index]; }
  
  void resetMisc();
  
  int getMiscCount() { return miscCount; }
  
  model_msc *getMisc();
  model_msc *getMisc(int index) { return miscBuffer[index]; }
  
  void resetTextures();
};

#endif