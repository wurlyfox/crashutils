#ifndef COMP_GEOM_H
#define COMP_GEOM_H

#include "graphics/context.h"

class comp_geom
{ 
  public:
  
  static void getVolumeVertices3d(fvolume *in, fpoint (&rect3d)[8]);
  static void getVolumeVertices3d(fvolume *in, dpoint (&rect3d)[8]);

  static void projectOutline2d(context *ctxt, dpoint *in, int inCount, dpoint *out, int &outCount);
  static void convexHull2d(dpoint *in, int inCount, dpoint *out, int &outCount);
  
  static bool pointInPolygon2d(int x, int y, dpoint *in, int inCount);
};

#endif