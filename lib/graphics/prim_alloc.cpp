#include "prim_alloc.h"

void prim_alloc::freePrimitives()
{  
  polygonCount = 0;
  quadCount    = 0;
  vertexCount  = 0;
  normalCount  = 0;
  
  #ifdef GL_VERTEX_ARRAY_ENABLE
  pPolygonCount = 0;
  pVertexCount  = 0;
  pNormalCount  = 0;
  pTexcoordCount = 0;

  mLineCount = 0;
  mTriangleCount = 0;
  mQuadCount = 0;
  #endif
  
  //colorCount   = 0;  ..colors local to models
}

vertex *prim_alloc::allocVertices(int count)
{
  vertex *vertices = &vertexBuffer[vertexCount];
  vertexCount += count;

  return vertices;
}

polygon *prim_alloc::allocPolygons(int count)
{
  polygon *polygons = &polygonBuffer[polygonCount];
  polygonCount += count;
 
  return polygons;
}

quad *prim_alloc::allocQuads(int count)
{
  quad *quads = &quadBuffer[quadCount];
  quadCount += count;

  return quads;
}

fvector *prim_alloc::allocNormals(int count)
{
  fvector *normals = &normalBuffer[normalCount];
  normalCount += count;

  return normals;
}
 
#ifdef GL_VERTEX_ARRAY_ENABLE
void prim_alloc::preparepPolygons(vertex *&vertices, fvector *&normals, fpoint2 *&texcoords)
{
  vertices = &pVertexBuffer[pVertexCount];
  normals = &pNormalBuffer[pNormalCount];
  texcoords = &pTexcoordBuffer[pTexcoordCount];
}

void prim_alloc::preparepPolygons(vertex *&vertices, fpoint2 *&texcoords)
{
  vertices = &pVertexBuffer[pVertexCount];
  texcoords = &pTexcoordBuffer[pTexcoordCount];
}

void prim_alloc::allocpPolygon(vertex *&vertices)
{
  vertices = &pVertexBuffer[pVertexCount];
  pVertexCount += 3;
  
  pPolygonCount++;
}

void prim_alloc::allocpPolygon(vertex *&vertices, fvector *&normals)
{
  vertices = &pVertexBuffer[pVertexCount];
  pVertexCount += 3;

  normals = &pNormalBuffer[pNormalCount];
  pNormalCount += 3;

  pPolygonCount++;
}

void prim_alloc::allocpPolygon(vertex *&vertices, fpoint2 *&texcoords)
{
  vertices = &pVertexBuffer[pVertexCount];
  pVertexCount += 3;

  texcoords = &pTexcoordBuffer[pTexcoordCount];
  pTexcoordCount += 3;

  pPolygonCount++;
}

void prim_alloc::allocpPolygon(vertex *&vertices, fvector *&normals, fpoint2 *&texcoords)
{
  vertices = &pVertexBuffer[pVertexCount];
  pVertexCount += 3;

  normals = &pNormalBuffer[pNormalCount];
  pNormalCount += 3;

  texcoords = &pTexcoordBuffer[pTexcoordCount];
  pTexcoordCount += 3;

  pPolygonCount++;
}


fline *prim_alloc::allocmLines(int count)
{
  fline *lines = &mLineBuffer[mLineCount];
  mLineCount += count;
  
  return lines;
}

ftriangle *prim_alloc::allocmTriangles(int count)
{
  ftriangle *triangles = &mTriangleBuffer[mTriangleCount];
  mTriangleCount += count;
  
  return triangles;
}

fquad *prim_alloc::allocmQuads(int count)
{
  fquad *quads = &mQuadBuffer[mQuadCount];
  mQuadCount += count;
  
  return quads;
}

#endif