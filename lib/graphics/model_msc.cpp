#include "model_msc.h"

fline *model_msc::newLines(prim_alloc *prims, int count)
{
  lines = prims->allocmLines(count);
  lineCount += count;
  
  return lines;
}

ftriangle *model_msc::newTriangles(prim_alloc *prims, int count)
{
  triangles = prims->allocmTriangles(count);
  triangleCount += count;
  
  return triangles;
}

fquad *model_msc::newQuads(prim_alloc *prims, int count)
{
  quads = prims->allocmQuads(count);
  quadCount += count;
  
  return quads;
}





