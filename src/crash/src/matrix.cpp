#include "matrix.h"

void MulMatrix(csmatrix *left, csmatrix *right)
{
  csmatrix temp;
  copyVector(&temp.V1, &left->V1);
  copyVector(&temp.V2, &left->V2);
  copyVector(&temp.V3, &left->V3);

  left->V1.X = ((temp.V1.X * right->V1.X) + (temp.V2.X * right->V1.Y) + (temp.V3.X * right->V1.Z)) >> 12;
  left->V2.X = ((temp.V1.X * right->V2.X) + (temp.V2.X * right->V2.Y) + (temp.V3.X * right->V2.Z)) >> 12;
  left->V3.X = ((temp.V1.X * right->V3.X) + (temp.V2.X * right->V3.Y) + (temp.V3.X * right->V3.Z)) >> 12;
  left->V1.Y = ((temp.V1.Y * right->V1.X) + (temp.V2.Y * right->V1.Y) + (temp.V3.Y * right->V1.Z)) >> 12;
  left->V2.Y = ((temp.V1.Y * right->V2.X) + (temp.V2.Y * right->V2.Y) + (temp.V3.Y * right->V2.Z)) >> 12;
  left->V3.Y = ((temp.V1.Y * right->V3.X) + (temp.V2.Y * right->V3.Y) + (temp.V3.Y * right->V3.Z)) >> 12;
  left->V1.Z = ((temp.V1.Z * right->V1.X) + (temp.V2.Z * right->V1.Y) + (temp.V3.Z * right->V1.Z)) >> 12;
  left->V2.Z = ((temp.V1.Z * right->V2.X) + (temp.V2.Z * right->V2.Y) + (temp.V3.Z * right->V2.Z)) >> 12;
  left->V3.Z = ((temp.V1.Z * right->V3.X) + (temp.V2.Z * right->V3.Y) + (temp.V3.Z * right->V3.Z)) >> 12;
}

void ScaleMatrix(csmatrix *left, cvector *right)
{
  left->V1.X = (left->V1.X * right->X) >> 12;
  left->V2.X = (left->V2.X * right->X) >> 12;
  left->V3.X = (left->V3.X * right->X) >> 12;
  left->V1.Y = (left->V1.Y * right->Y) >> 12;
  left->V2.Y = (left->V2.Y * right->Y) >> 12;
  left->V3.Y = (left->V3.Y * right->Y) >> 12;
  left->V1.Z = (left->V1.Z * right->Z) >> 12;
  left->V2.Z = (left->V2.Z * right->Z) >> 12;
  left->V3.Z = (left->V3.Z * right->Z) >> 12;
}

void transform(cvector *input, cvector *trans, cangle *rot, cvector *scale, cvector *output)
{
  //s4 = svtxVector
  //s5 = translate
  //s1 = rotate
  //s6 = scale
  //s7 = arg10
  
  //a 'rotation vector' passed to the sub consists of 
  //some tait-bryan angles (roll/pitch/yaw)
  //in YXZ form
  //this is used to a generate the corresponding
  //linear transformation in 3 dimensional space
  //for a rotation about each axis by the specified 
  //angles
  //accomplished by matrix multiplication
   //of the rotation matrices for each axis/specified angle
  
  //*all rotation vector entries anded with 0xFFF
 
  //s0 = -sin(rotate.Z)
  //v0 = cos(rotate.Z)
  //v1 = sin(rotate.Z)
  
  //[v0,     0,  s0]
  //[ 0,0x1000,   0]
  //[v1,     0,  v0]

  //(z rot matrix)
  
  //s0 = sin(rotate.Y);
  //v1 = -sin(rotate.Y);
  //v0 = cos(rotate.Y);
  
  //[v0,   s0,     0]
  //[v1,   v0,     0]
  //[ 0,    0,0x1000]
 
  //(y rot matrix)
  //  
  //xrot = rotate.X - rotate.Z; (tait-bryan angles)
  //
  //s0 = -sin(xrot);
  //v1 = sin(xrot);
  //v0 = cos(xrot);
  //  
  //[v0,      0,  s0]
  //[ 0, 0x1000,   0]
  //[v1,      0,  v0]
  
  //(x rot matrix)
  //0x800 = pi
  //0x1000 = 2pi
  
  //first convert from crash angle to normal angle representation, then back to fixed pt
  //we could have also used sine/cos tables like the game
  float rotX = CTGA(rot->X);
  float rotY = CTGA(rot->Y);
  float rotZ = CTGA(rot->Z);
  
  signed short sinrotX = FLOAT2FIXED(sin(rotX));
  signed short cosrotX = FLOAT2FIXED(cos(rotX));
  signed short sinrotY = FLOAT2FIXED(sin(rotY));
  signed short cosrotY = FLOAT2FIXED(cos(rotY));
  signed short sinrotZ = FLOAT2FIXED(sin(rotZ));
  signed short cosrotZ = FLOAT2FIXED(cos(rotZ));
  
  csmatrix matA;   //appears here as the transpose of the actual matrix
  matA.V1 = { cosrotZ,      0,  sinrotZ };
  matA.V2 = {       0, 0x1000,        0 };
  matA.V3 = {-sinrotZ,      0,  cosrotZ };     
  
  csmatrix matB;
  matB.V1 = { cosrotY, -sinrotY,      0 };
  matB.V2 = { sinrotY,  cosrotY,      0 };
  matB.V3 = {       0,        0, 0x1000 };
  
  csmatrix matC;
  matC.V1 = {  cosrotX,      0, sinrotX };
  matC.V2 = {        0, 0x1000,       0 };
  matC.V3 = { -sinrotX,      0, cosrotX };
  
  MulMatrix(&matA, &matB);
  MulMatrix(&matA, &matC);
 
  //the linear transformation is further scaled by the
  //scale vector if one exists  
  if (scale)
    ScaleMatrix(&matA, scale); //guess: scales the rotation
    
  //then the input vector from anim frame (initial direction?) is scaled by 16..
  cvector in, out;
  in.X = (input->X >= 0 ? input->X: input->X+0xF) >> 4;
  in.Y = (input->Y >= 0 ? input->Y: input->Y+0xF) >> 4;
  in.Z = (input->Z >= 0 ? input->Z: input->Z+0xF) >> 4;
  
  //linear transformation sub 
  //finally the input vector from anim frame (default
  //facing direction) is transformed by the scaled rotation matrix
  //to yield the vector that the object would be translated
  //by if it were to move in the direction specfied by its orientation
  transform(&in, &matA, &out);
  
  //unscale due to scaling of the initial direction before its transformation
  out.X <<= 4;
  out.Y <<= 4;
  out.Z <<= 4;

  //finally translate out to get the output vector  
  addVector(&out, trans, output);
}

void transform(cvector *input, csmatrix *matrix, cvector *output)
{
  output->X = ((matrix->V1.X * input->X) + (matrix->V2.X * input->Y) + (matrix->V3.X * input->Z)) >> 12;
  output->Y = ((matrix->V1.Y * input->X) + (matrix->V2.Y * input->Y) + (matrix->V3.Y * input->Z)) >> 12;
  output->Z = ((matrix->V1.Z * input->X) + (matrix->V2.Z * input->Y) + (matrix->V3.Z * input->Z)) >> 12;
}

unsigned long apxDist(cvector *vectA, cvector *vectB)
{
  long distX = vectA->X - vectB->X;
  if (distX < 0) { distX = vectB->X - vectA->X; }
  long distY = vectA->Y - vectB->Y;
  if (distY < 0) { distX = vectB->Y - vectA->Y; }
  long distZ = vectA->Z - vectB->Z;
  if (distZ < 0) { distX = vectB->Z - vectA->Z; }
    
  unsigned long dist;
  if (distX >= distY && distX >= distZ)
  {
    unsigned long distYZ = (distY + distZ) / 4;
    dist                 = (distX + distYZ);
  }
  else if (distY >= distX && distY >= distZ)
  {
    unsigned long distXZ = (distX + distZ) / 4;
    dist                 = (distY + distXZ);
  }
  else
  {
    unsigned long distXY = (distX  + distY) / 4;
    dist                 = (distXY + distZ);
  }
  
  return dist;
}

unsigned long eucDist(cvector *vectA, cvector *vectB)
{
  long diffX = CTGVC(vectA->X - vectB->X);
  long diffY = CTGVC(vectA->Y - vectB->Y);
  long diffZ = CTGVC(vectA->Z - vectB->Z);
  
  float diffXf = FIXED2FLOAT(diffX);
  float diffYf = FIXED2FLOAT(diffY);
  float diffZf = FIXED2FLOAT(diffZ);
  
  float squared = (diffXf * diffXf) + 
                  (diffYf * diffYf) + 
                  (diffZf * diffZf);
                        
  float magnitude = sqrt(squared);
  
  return CTCVC(FLOAT2FIXED(magnitude));
}

unsigned long eucDistXZ(cvector *vectA, cvector *vectB)
{
  long diffX = CTGVC(vectA->X - vectB->X);
  long diffZ = CTGVC(vectA->Z - vectB->Z);
  
  float diffXf = FIXED2FLOAT(diffX);
  float diffZf = FIXED2FLOAT(diffZ);
  
  float squared = (diffXf * diffXf) + 
                  (diffZf * diffZf);
                        
  float magnitude = sqrt(squared);
  
  return CTCVC(FLOAT2FIXED(magnitude));
}

signed long angleXZ(cvector *vectA, cvector *vectB)
{
  //we dont have to scale down for atan2
  signed long distX = vectB->X - vectA->X;
  signed long distZ = vectB->Z - vectA->Z;
  
  signed long arctan = atan2(distX, distZ);
  
  //we do have to convert back to crash angles if we are using math's atan2
  signed long ang = CTCA(arctan) & 0xFFF;
  
  return ang;
}

signed long angleXY(cvector *vectA, cvector *vectB)
{
  //we dont have to scale down for atan2
  signed long distX = vectB->X - vectA->X;
  signed long distY = vectB->Y - vectA->Y;
  
  signed long arctan = atan2(distX, distY);
    
  //we do have to convert back to crash angles if we are using math's atan2
  signed long ang = CTCA(arctan) & 0xFFF;
  
  return ang;
}
  
  
  
  
  
    
