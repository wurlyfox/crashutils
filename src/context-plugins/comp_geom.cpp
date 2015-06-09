#include "comp_geom.h"

void comp_geom::getVolumeVertices3d(fvolume *in, fpoint (&rect3d)[8])
{
  rect3d[0] = {             in->loc.X,             in->loc.Y,             in->loc.Z };
  rect3d[1] = { in->loc.X + in->dim.W,             in->loc.Y,             in->loc.Z };
  rect3d[2] = {             in->loc.X, in->loc.Y + in->dim.H,             in->loc.Z };
  rect3d[3] = { in->loc.X + in->dim.W, in->loc.Y + in->dim.H,             in->loc.Z };
  rect3d[4] = {             in->loc.X,             in->loc.Y, in->loc.Z + in->dim.D };
  rect3d[5] = { in->loc.X + in->dim.W,             in->loc.Y, in->loc.Z + in->dim.D };
  rect3d[6] = {             in->loc.X, in->loc.Y + in->dim.H, in->loc.Z + in->dim.D };
  rect3d[7] = { in->loc.X + in->dim.W, in->loc.Y + in->dim.H, in->loc.Z + in->dim.D };
}

void comp_geom::getVolumeVertices3d(fvolume *in, dpoint (&rect3d)[8])
{
  rect3d[0] = {             in->loc.X,             in->loc.Y,             in->loc.Z };
  rect3d[1] = { in->loc.X + in->dim.W,             in->loc.Y,             in->loc.Z };
  rect3d[2] = {             in->loc.X, in->loc.Y + in->dim.H,             in->loc.Z };
  rect3d[3] = { in->loc.X + in->dim.W, in->loc.Y + in->dim.H,             in->loc.Z };
  rect3d[4] = {             in->loc.X,             in->loc.Y, in->loc.Z + in->dim.D };
  rect3d[5] = { in->loc.X + in->dim.W,             in->loc.Y, in->loc.Z + in->dim.D };
  rect3d[6] = {             in->loc.X, in->loc.Y + in->dim.H, in->loc.Z + in->dim.D };
  rect3d[7] = { in->loc.X + in->dim.W, in->loc.Y + in->dim.H, in->loc.Z + in->dim.D };
}

void comp_geom::projectOutline2d(context *ctxt, dpoint *in, int inCount, dpoint *out, int &outCount)
{
  dpoint poly2d[inCount];
  
  // project all 3d points on the poly to 2d
  for (int lp=0; lp<inCount; lp++)
    poly2d[lp] = ctxt->project2d(in[lp].X, in[lp].Y, in[lp].Z);
    

  // get context viewport width and height
  int viewportWidth, viewportHeight;
  ctxt->getViewport(viewportWidth, viewportHeight);
  
  // check for trivial reject
  bool reject = true;
  for (int lp=0; lp<inCount; lp++)
  {
    if (poly2d[lp].X >= 0 && poly2d[lp].X < viewportWidth &&
        poly2d[lp].Y >= 0 && poly2d[lp].Y < viewportHeight)
    {
      reject = false;
      break;
    }
  }
  
  // get the convex hull
  if (!reject)
    convexHull2d(poly2d, inCount, out, outCount); 
  else
    outCount = 0;
}

void comp_geom::convexHull2d(dpoint *in, int inCount, dpoint *out, int &outCount)
{
  // sort points  
  for (int lp2=inCount; lp2>=0; lp2--)
  {
    for (int lp=0; lp<lp2; lp++)
    {
      double X1 = in[lp].X;
      double Y1 = in[lp].Y;
      double X2 = in[lp+1].X;
      double Y2 = in[lp+1].Y;
     
      if (X1 > X2 ||
          (X1 == X2 && Y1 > Y2))
      {
        in[lp].X = X2;
        in[lp].Y = Y2;
        in[lp+1].X = X1;
        in[lp+1].Y = Y1;
      }        
    }
  }

      
  int k=0;

  // Build lower hull
  for (int i = 0; i < inCount; ++i) 
  {
    while (k >= 2 && 
           ((out[k-1].X - in[k-2].X) 
          * (   in[i].Y - in[k-2].Y) 
          - (out[k-1].Y - in[k-2].Y) 
          * (   in[i].X - in[k-2].X))
           <= 0) 
      k--;
    out[k++] = in[i];
  }

  // Build upper hull
  for (int i = inCount-2, t = k+1; i >= 0; i--) 
  {
    while (k >= t && 
           ((out[k-1].X - out[k-2].X) 
          * (   in[i].Y - out[k-2].Y) 
          - (out[k-1].Y - out[k-2].Y) 
          * (   in[i].X - out[k-2].X))
           <= 0) 
      k--;
    out[k++] = in[i];
  }
  
  outCount = k;
}

bool comp_geom::pointInPolygon2d(int x, int y, dpoint *in, int inCount)
{
  bool isIn = false;
  
  int i, j = inCount - 1;
  for (i = 0; i < inCount; i++) 
  {
    double xi = in[i].X; 
    double yi = in[i].Y;
    double xj = in[j].X;
    double yj = in[j].Y;
    
    if ((yi < y && yj >=y
    ||   yj < y && yi >=y)
    &&  (xi <=x || xj <=x)) 
      isIn ^= (xi+(y-yi)/(yj-yi)*(xj-xi)<x);  
    
    j = i;
  }  
  
  return isIn;
}
