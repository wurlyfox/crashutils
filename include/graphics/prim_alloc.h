#ifndef PRIM_ALLOC
#define PRIM_ALLOC

#include "types.h"

class prim_alloc
{
  protected:

  long vertexCount;
  long polygonCount;
  long quadCount;
  long normalCount;

  vertex vertexBuffer[MAX_VERTICES*MAX_MODELS];
  polygon polygonBuffer[MAX_POLYGONS*MAX_MODELS];
  quad quadBuffer[MAX_QUADS*MAX_MODELS];
  fvector normalBuffer[MAX_VERTICES*MAX_MODELS];

  #ifdef GL_VERTEX_ARRAY_ENABLE
  long pPolygonCount;
  long pVertexCount;
  long pNormalCount;
  long pTexcoordCount;

  fpoint2 pTexcoordBuffer[MAX_VERTICES*MAX_MODELS];
  vertex pVertexBuffer[MAX_VERTICES*MAX_MODELS];
  fvector pNormalBuffer[MAX_VERTICES*MAX_MODELS];
  
  long mLineCount;
  long mTriangleCount;
  long mQuadCount;
  
  fline mLineBuffer[0x100];
  ftriangle mTriangleBuffer[0x100];
  fquad mQuadBuffer[0x100];
  #endif
  
  public:

  prim_alloc() { freePrimitives(); }
  ~prim_alloc() {};
  
  vertex *allocVertices(int count);
  polygon *allocPolygons(int count);
  quad *allocQuads(int count);
  fvector *allocNormals(int count);

  #ifdef GL_VERTEX_ARRAY_ENABLE
  void preparepPolygons(vertex *&vertices, fvector *&normals, fpoint2 *&texcoords);
  void preparepPolygons(vertex *&vertices, fpoint2 *&texcoords);
  void allocpPolygon(vertex *&vertices);
  void allocpPolygon(vertex *&vertices, fvector *&normals);
  void allocpPolygon(vertex *&vertices, fpoint2 *&texcoords);
  void allocpPolygon(vertex *&vertices, fvector *&normals, fpoint2 *&texcoords);
  
  fline *allocmLines(int count);
  ftriangle *allocmTriangles(int count);
  fquad *allocmQuads(int count);
  #endif
   
  void freePrimitives();

  #ifdef GL_VERTEX_ARRAY_ENABLE
  long getPolygonCount() { return polygonCount + pPolygonCount; }
  long getVertexCount()  { return vertexCount + pVertexCount; }
  long getNormalCount()  { return normalCount + pNormalCount; }
  long getTexcoordCount() { return pTexcoordCount; }
  #else
  long getVertexCount() { return vertexCount; }
  long getPolygonCount() { return polygonCount; }
  long getNormalCount() { return normalCount; }
  #endif
  
  long getQuadCount() { return quadCount; }

  void resetVertices() { vertexCount = 0; }
  void resetPolygons() { polygonCount = 0; }
  void resetQuads()    { quadCount = 0; }
  void resetNormals()  { normalCount = 0; }
};

#endif