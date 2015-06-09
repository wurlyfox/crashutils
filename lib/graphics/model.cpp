#include "model.h"

#include <math.h>   //for shaders

void model::calcColors()
{
  const float radian = 3.14159265f/180;

  if (type == 1)
  {
    fmatrix LLM, LROT, LOBJ;
    fvector LLV;
    fcolormatrix LCM;
    fcolor LC, OC;
    
    //[LRot]  = [ cos(x)cos(z) - sin(x)cos(y)sin(z), sin(x)sin(y), cos(x)sin(z) + sin(x)cos(y)cos(z)]
    //          [                      sin(y)sin(z),       cos(y),                     -sin(y)cos(z)]
    //          [-sin(x)cos(z) - cos(x)cos(y)sin(z), cos(x)sin(y), cos(x)cos(y)cos(z) - sin(x)sin(z)]

    fangle euler;
    if (scale.X < 0)
      euler.X = rot.Z + rot.X;
    else
      euler.X = rot.Z - rot.X;
      
    euler.Y = -rot.Y;
    euler.Z = -rot.Z; 
    
    if (euler.X > 0) {}
    else             { euler.X = 360 + ((int)euler.X % 360); }
    if (euler.Y > 0) {}
    else             { euler.Y = 360 + ((int)euler.Y % 360); }   
    if (euler.Z > 0) {}
    else             { euler.Z = 360 + ((int)euler.Z % 360); }
    
    float sinx = sin(euler.X*radian);
    float siny = sin(euler.Y*radian);
    float sinz = sin(euler.Z*radian);
    float cosx = cos(euler.X*radian);
    float cosy = cos(euler.Y*radian);
    float cosz = cos(euler.Z*radian);
    
    LROT.V1.X = (cosx*cosz) - (sinx*cosy*sinz);
    LROT.V1.Y = (siny*sinz);
    LROT.V1.Z = (-sinx*cosz) - (cosx*cosy*sinz);
    LROT.V2.X = (sinx*siny);
    LROT.V2.Y = cosy;
    LROT.V2.Z = (cosx*siny);
    LROT.V3.X = (cosx*sinz) + (sinx*cosy*cosz);
    LROT.V3.Y = -(siny*cosz);
    LROT.V3.Z = (cosx*cosy*cosz) - (sinx*sinz);
    
    if (scale.X < 0)
    {
      lightMatrix.V1.X = -lightMatrix.V1.X;
      lightMatrix.V1.Y = -lightMatrix.V1.Y;
      lightMatrix.V1.Z = -lightMatrix.V1.Z;
    }

    LLM.V1.X = (LROT.V1.X * lightMatrix.V1.X) + (LROT.V2.X * lightMatrix.V1.Y) + (LROT.V3.X * lightMatrix.V1.Z);
    LLM.V2.X = (LROT.V1.X * lightMatrix.V2.X) + (LROT.V2.X * lightMatrix.V2.Y) + (LROT.V3.X * lightMatrix.V2.Z);
    LLM.V3.X = (LROT.V1.X * lightMatrix.V3.X) + (LROT.V2.X * lightMatrix.V3.Y) + (LROT.V3.X * lightMatrix.V3.Z);
    LLM.V1.Y = (LROT.V1.Y * lightMatrix.V1.X) + (LROT.V2.Y * lightMatrix.V1.Y) + (LROT.V3.Y * lightMatrix.V1.Z);
    LLM.V2.Y = (LROT.V1.Y * lightMatrix.V2.X) + (LROT.V2.Y * lightMatrix.V2.Y) + (LROT.V3.Y * lightMatrix.V2.Z);
    LLM.V3.Y = (LROT.V1.Y * lightMatrix.V3.X) + (LROT.V2.Y * lightMatrix.V3.Y) + (LROT.V3.Y * lightMatrix.V3.Z);
    LLM.V1.Z = (LROT.V1.Z * lightMatrix.V1.X) + (LROT.V2.Z * lightMatrix.V1.Y) + (LROT.V3.Z * lightMatrix.V1.Z);
    LLM.V2.Z = (LROT.V1.Z * lightMatrix.V2.X) + (LROT.V2.Z * lightMatrix.V2.Y) + (LROT.V3.Z * lightMatrix.V2.Z);
    LLM.V3.Z = (LROT.V1.Z * lightMatrix.V3.X) + (LROT.V2.Z * lightMatrix.V3.Y) + (LROT.V3.Z * lightMatrix.V3.Z);
    
    unsigned long vertCount = (object_model->polyTexCount+object_model->polyNonCount) * 3;
    
    vertex *vertices = object_model->vertices;
    fvector *normals = object_model->normals;
    for (int lp = 0; lp < vertCount; lp++)
    {
      LLV.X = (LLM.V1.X * normals[lp].X) + (LLM.V2.X * normals[lp].Y) + (LLM.V3.X * normals[lp].Z);
      LLV.Y = (LLM.V1.Y * normals[lp].X) + (LLM.V2.Y * normals[lp].Y) + (LLM.V3.Y * normals[lp].Z);
      LLV.Z = (LLM.V1.Z * normals[lp].X) + (LLM.V2.Z * normals[lp].Y) + (LLM.V3.Z * normals[lp].Z);
      
      LC.R = backColor.R + (colorMatrix.C1.R * LLV.X) + (colorMatrix.C2.R * LLV.Y) + (colorMatrix.C3.R * LLV.Z);
      LC.G = backColor.G + (colorMatrix.C1.G * LLV.X) + (colorMatrix.C2.G * LLV.Y) + (colorMatrix.C3.G * LLV.Z);
      LC.B = backColor.B + (colorMatrix.C1.B * LLV.X) + (colorMatrix.C2.B * LLV.Y) + (colorMatrix.C3.B * LLV.Z);
      
      //0   : 0.25 ->   0 : 0.50
      //0.25:  1.0 -> 0.50 : 1.0
      
      //if (RP < 8) { R = RP * 16; } else { R = 128 + ((RP-8) * (16/3)); }
      //if (GP < 8) { G = GP * 16; } else { G = 128 + ((GP-8) * (16/3)); }
      //if (BP < 8) { B = BP * 16; } else { B = 128 + ((BP-8) * (16/3)); }
        
      OC.R = LC.R * vertices[lp].Rf;
      OC.G = LC.G * vertices[lp].Gf;
      OC.B = LC.B * vertices[lp].Bf;
      
      if (OC.R < 0) { OC.R = -OC.R; }
      if (OC.G < 0) { OC.G = -OC.G; }
      if (OC.B < 0) { OC.B = -OC.B; }

      //adjust output color according to psx gpu color curve
      if (OC.R <= 0.25f) { OC.R *= 2; }
      else               { OC.R = 0.5f + ((OC.R - 0.25f) * (0.5f/3)); }
      if (OC.G <= 0.25f) { OC.G *= 2; }
      else               { OC.G = 0.5f + ((OC.G - 0.25f) * (0.5f/3)); }
      if (OC.B <= 0.25f) { OC.B *= 2; }
      else               { OC.B = 0.5f + ((OC.B - 0.25f) * (0.5f/3)); } 
      
      colors[lp].R = OC.R;
      colors[lp].G = OC.G;
      colors[lp].B = OC.B;
      
    }
  }     
}
