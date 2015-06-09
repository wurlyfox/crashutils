#ifndef MODEL_OBJ
#define MODEL_OBJ

#include "../crash/entry.h"
#include "../crash/nsf.h"
#include "../crash/nsd.h"

#include "prim_alloc.h"
#include "texture_buffer.h"
#include "types.h"

#define SCALE_OBJECT 4

class model_obj
{
  protected:
	NSF *nsf;  //we need to load these for each object so that they can resolve
	NSD *nsd;  //the appropriate tgeo entry given just a svtx
	
  public:
  bool render;
  int polyCount;        //total polygons
  int polyTexCount;     //total textured polygons
  int polyNonCount:15;  //total non-textured polygons
  int positionMode:1;
  int vertCount;
 
  polygon *polygons;
  vertex  *vertices;
  fvector *normals;
  #ifdef GL_VERTEX_ARRAY_ENABLE
  fpoint2 *texcoords;
  #endif
  
  fregion bound;
  vector location;
  
	void init(NSF *_nsf, NSD *_nsd)
  {  
    nsf = _nsf;
	  nsd = _nsd;
  }
  
  void load(entry *svtx, int frame, point loc, texture_buffer *texbuf, prim_alloc *prims);
  void load(entry *svtx, int frame, texture_buffer *texbuf, prim_alloc *prims);
  virtual void loadVertices(entry *svtx, int frame, prim_alloc *prims);
  virtual void loadPolygons(entry *tgeo, texture_buffer *texbuf, prim_alloc *prims);
  
  virtual void draw(fcolor *colors = 0) {}; //draw with no translation
  virtual void drawAt(vector loc) {};       //draw with translation by loc 
  void drawAtLocation();                    //draw with translation by location (draw at location)
};

#endif