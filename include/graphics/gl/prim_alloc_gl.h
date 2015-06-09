//support for primitive allocation in form of gl vertex arrays

#ifndef PRIM_ALLOC_GL_H
#define PRIM_ALLOC_GL_H

#include "prim_alloc.h"

class prim_alloc_gl : public prim_alloc
{
  private:

  long pPolygonCount;
  long pVertexCount;
  long pNormalCount;
  long pTexcoordCount;

  fpoint2 pTexcoordBuffer[MAX_VERTICES*MAX_MODELS];
  vertex pVertexBuffer[MAX_VERTICES*MAX_MODELS];
  fvector pNormalBuffer[MAX_VERTICES*MAX_MODELS];

  public:

  prim_alloc_gl() { freePrimitives(); }
  ~prim_alloc_gl() {};
  
  void freePrimitives();

  void preparepPolygons(vertex *&vertices, fvector *&normals, fpoint2 *&texcoords);
  void preparepPolygons(vertex *&vertices, fpoint2 *&texcoords);
  void allocpPolygon(vertex *&vertices);
  void allocpPolygon(vertex *&vertices, fvector *&normals);
  void allocpPolygon(vertex *&vertices, fpoint2 *&texcoords);
  void allocpPolygon(vertex *&vertices, fvector *&normals, fpoint2 *&texcoords);

  long getPolygonCount() { return polygonCount + pPolygonCount; }
  long getVertexCount()  { return vertexCount + pVertexCount; }
  long getNormalCount()  { return normalCount + pNormalCount; }
  long getTexcoordCount() { return pTexcoordCount; }
};

#endif