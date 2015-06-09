#include "prim_alloc_gl.h"

void prim_alloc_gl::freePrimitives()
{
  prim_alloc::freePrimitives();

  pPolygonCount = 0;
  pVertexCount  = 0;
  pNormalCount  = 0;
  pTexcoordCount = 0;
}

void prim_alloc_gl::preparepPolygons(vertex *&vertices, fvector *&normals, fpoint2 *&texcoords)
{
  vertices = &pVertexBuffer[pVertexCount];
  normals = &pNormalBuffer[pNormalCount];
  texcoords = &pTexcoordBuffer[pTexcoordCount];
}

void prim_alloc_gl::preparepPolygons(vertex *&vertices, fpoint2 *&texcoords)
{
  vertices = &pVertexBuffer[pVertexCount];
  texcoords = &pTexcoordBuffer[pTexcoordCount];
}

void prim_alloc_gl::allocpPolygon(vertex *&vertices)
{
  vertices = &pVertexBuffer[pVertexCount];
  pVertexCount += 3;
  
  pPolygonCount++;
}

void prim_alloc_gl::allocpPolygon(vertex *&vertices, fvector *&normals)
{
  vertices = &pVertexBuffer[pVertexCount];
  pVertexCount += 3;

  normals = &pNormalBuffer[pNormalCount];
  pNormalCount += 3;

  pPolygonCount++;
}

void prim_alloc_gl::allocpPolygon(vertex *&vertices, fpoint2 *&texcoords)
{
  vertices = &pVertexBuffer[pVertexCount];
  pVertexCount += 3;

  texcoords = &pTexcoordBuffer[pTexcoordCount];
  pTexcoordCount += 3;

  pPolygonCount++;
}

void prim_alloc_gl::allocpPolygon(vertex *&vertices, fvector *&normals, fpoint2 *&texcoords)
{
  vertices = &pVertexBuffer[pVertexCount];
  pVertexCount += 3;

  normals = &pNormalBuffer[pNormalCount];
  pNormalCount += 3;

  texcoords = &pTexcoordBuffer[pTexcoordCount];
  pTexcoordCount += 3;

  pPolygonCount++;
}