#include <gl\gl.h>
#include <gl\glu.h>

#include <math.h>

#include "graphics/gl/immediate.h"

/*
void drawSelection()
{
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glBegin(GL_QUADS);
  glColor3f(1.0,1.0,1.0);
  drawRect3f(selection);
  glEnd();
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void selectRegion(region reg)
{
  float X1 = (((float)reg.P1.X)/0x1000) * 8;
  float Y1 = (((float)reg.P1.Y)/0x1000) * 8;
  float Z1 = (((float)reg.P1.Z)/0x1000) * 8;

  float X2 = (((float)reg.P2.X)/0x1000) * 8;
  float Y2 = (((float)reg.P2.Y)/0x1000) * 8;
  float Z2 = (((float)reg.P2.Z)/0x1000) * 8;
  
  selection.P1.X = X1;
  selection.P1.Y = Y1;
  selection.P1.Z = Z1;
  selection.P2.X = X2;
  selection.P2.Y = Y2;
  selection.P2.Z = Z2;
}
*/

void drawPath(vector loc, point P1, point P2, int scale)
{
  float X1 = ((loc.X+P1.X)/(float)0x1000)*scale;
  float Y1 = ((loc.Y+P1.Y)/(float)0x1000)*scale;
  float Z1 = ((loc.Z+P1.Z)/(float)0x1000)*scale;
  
  float X2 = ((loc.X+P2.X)/(float)0x1000)*scale;
  float Y2 = ((loc.Y+P2.Y)/(float)0x1000)*scale;
  float Z2 = ((loc.Z+P2.Z)/(float)0x1000)*scale;
  
  fvector diff = { X2-X1, Y2-Y1, Z2-Z1 };
  
  glBegin(GL_LINES);
  glColor3f(1.0,1.0,1.0);
  glVertex3f(X1, Y1, Z1);
  glVertex3f(X2, Y2, Z2);
  glEnd();
  
  float normal = sqrt(diff.X*diff.X + diff.Y*diff.Y + diff.Z*diff.Z);
  
  float mat44[16] = { 1, 0, 0, 0,
                       0, 1, 0, 0,
                       0, 0, 1, 0,
                       0, 0, 0, 1 };
  if (normal != 0)
  {
    fvector ndiff = { diff.X/normal, diff.Y/normal, diff.Z/normal };
    
    fvector rot = { ndiff.Y, -ndiff.X, 0 };
    
    float ac = acos( diff.Z/normal );
    float s = sin(ac);
    float c = cos(ac);
    float t = 1 - c;
    
    float x = rot.X;
    float y = rot.Y;
    float z = rot.Z;
 
    mat44[0] = t*x*x + c;
    mat44[1] = t*x*y - s*z;
    mat44[2] = t*x*z + s*y;
 
    mat44[4] = t*x*y + s*z;
    mat44[5] = t*y*y + c;
    mat44[6] = t*y*z - s*x;
 
    mat44[8] = t*x*z - s*y;
    mat44[9] = t*y*z + s*x;
    mat44[10] = t*z*z + c;
    
    mat44[15] = 1.0;
    
    glPushMatrix();
    glTranslatef(X1, Y1, Z1);
    glMultMatrixf(mat44);
    
    int arrowSize = 40;
    float arrowScale = ((float)arrowSize/0x1000)*scale;
    
    glBegin(GL_TRIANGLES);
    glColor3f(1.0,1.0,1.0);
    glVertex3f(0.0, 0.0, arrowScale);
    glVertex3f(arrowScale, 0.0, 0.0);
    glVertex3f(-arrowScale, 0.0, 0.0);
    glEnd();
    
    glPopMatrix();
  }
}

void drawRect3(region reg, point loc, int scale)
{
  region newRegion;
  newRegion.P1.X = reg.P1.X + loc.X;
  newRegion.P1.Y = reg.P1.Y + loc.Y;
  newRegion.P1.Z = reg.P1.Z + loc.Z;
  newRegion.P2.X = reg.P2.X + loc.X;
  newRegion.P2.Y = reg.P2.Y + loc.Y;
  newRegion.P2.Z = reg.P2.Z + loc.Z;
  
  drawRect3(newRegion, scale);
}

void drawRect3(region reg, int scale)
{  
  float X1 = (((float)reg.P1.X)/0x1000) * scale;
  float Y1 = (((float)reg.P1.Y)/0x1000) * scale;
  float Z1 = (((float)reg.P1.Z)/0x1000) * scale;
  
  float X2 = (((float)reg.P2.X)/0x1000) * scale;
  float Y2 = (((float)reg.P2.Y)/0x1000) * scale;
  float Z2 = (((float)reg.P2.Z)/0x1000) * scale;
  

  glBegin(GL_QUADS);
  
  glVertex3f(X1, Y1, Z1);
  glVertex3f(X2, Y1, Z1);
  glVertex3f(X2, Y2, Z1);
  glVertex3f(X1, Y2, Z1);
  
  glVertex3f(X1, Y1, Z1);
  glVertex3f(X1, Y1, Z2);
  glVertex3f(X1, Y2, Z2);
  glVertex3f(X1, Y2, Z1);
  
  glVertex3f(X2, Y1, Z1);
  glVertex3f(X2, Y1, Z2);
  glVertex3f(X2, Y2, Z2);
  glVertex3f(X2, Y2, Z1);
  
  glVertex3f(X1, Y1, Z1);
  glVertex3f(X1, Y1, Z2);
  glVertex3f(X2, Y1, Z2); 
  glVertex3f(X2, Y1, Z1);
 
  glVertex3f(X1, Y2, Z1);
  glVertex3f(X1, Y2, Z2);
  glVertex3f(X2, Y2, Z2); 
  glVertex3f(X2, Y2, Z1);
  
  glVertex3f(X1, Y1, Z2);
  glVertex3f(X2, Y1, Z2);
  glVertex3f(X2, Y2, Z2);
  glVertex3f(X1, Y2, Z2);
  
  glEnd();
}
  
  
void drawRect3f(fregion reg)
{    
  glVertex3f(reg.P1.X, reg.P1.Y, reg.P1.Z);
  glVertex3f(reg.P2.X, reg.P1.Y, reg.P1.Z);
  glVertex3f(reg.P2.X, reg.P2.Y, reg.P1.Z);
  glVertex3f(reg.P1.X, reg.P2.Y, reg.P1.Z);
  
  glVertex3f(reg.P1.X, reg.P1.Y, reg.P1.Z);
  glVertex3f(reg.P1.X, reg.P1.Y, reg.P2.Z);
  glVertex3f(reg.P1.X, reg.P2.Y, reg.P2.Z);
  glVertex3f(reg.P1.X, reg.P2.Y, reg.P1.Z);
  
  glVertex3f(reg.P2.X, reg.P1.Y, reg.P1.Z);
  glVertex3f(reg.P2.X, reg.P1.Y, reg.P2.Z);
  glVertex3f(reg.P2.X, reg.P2.Y, reg.P2.Z);
  glVertex3f(reg.P2.X, reg.P2.Y, reg.P1.Z);
  
  glVertex3f(reg.P1.X, reg.P1.Y, reg.P1.Z);
  glVertex3f(reg.P1.X, reg.P1.Y, reg.P2.Z);
  glVertex3f(reg.P2.X, reg.P1.Y, reg.P2.Z); 
  glVertex3f(reg.P2.X, reg.P1.Y, reg.P1.Z);
 
  glVertex3f(reg.P1.X, reg.P2.Y, reg.P1.Z);
  glVertex3f(reg.P1.X, reg.P2.Y, reg.P2.Z);
  glVertex3f(reg.P2.X, reg.P2.Y, reg.P2.Z); 
  glVertex3f(reg.P2.X, reg.P2.Y, reg.P1.Z);
  
  glVertex3f(reg.P1.X, reg.P1.Y, reg.P2.Z);
  glVertex3f(reg.P2.X, reg.P1.Y, reg.P2.Z);
  glVertex3f(reg.P2.X, reg.P2.Y, reg.P2.Z);
  glVertex3f(reg.P1.X, reg.P2.Y, reg.P2.Z);
}

/*
void drawCSpace(cspace *space)
{
  float X1 = ((float)(space->xyz1.X >> 8)/0x1000) * 8;
  float Y1 = ((float)(space->xyz1.Y >> 8)/0x1000) * 8;
  float Z1 = ((float)(space->xyz1.Z >> 8)/0x1000) * 8;
                                                    
  float X2 = ((float)(space->xyz2.X >> 8)/0x1000) * 8;
  float Y2 = ((float)(space->xyz2.Y >> 8)/0x1000) * 8;
  float Z2 = ((float)(space->xyz2.Z >> 8)/0x1000) * 8;
  
  glBegin(GL_QUADS);
  
  glVertex3f(X1, Y1, Z1);
  glVertex3f(X2, Y1, Z1);
  glVertex3f(X2, Y2, Z1);
  glVertex3f(X1, Y2, Z1);
  
  glVertex3f(X1, Y1, Z1);
  glVertex3f(X1, Y1, Z2);
  glVertex3f(X1, Y2, Z2);
  glVertex3f(X1, Y2, Z1);
  
  glVertex3f(X2, Y1, Z1);
  glVertex3f(X2, Y1, Z2);
  glVertex3f(X2, Y2, Z2);
  glVertex3f(X2, Y2, Z1);
  
  glVertex3f(X1, Y1, Z1);
  glVertex3f(X1, Y1, Z2);
  glVertex3f(X2, Y1, Z2); 
  glVertex3f(X2, Y1, Z1);
 
  glVertex3f(X1, Y2, Z1);
  glVertex3f(X1, Y2, Z2);
  glVertex3f(X2, Y2, Z2); 
  glVertex3f(X2, Y2, Z1);
  
  glVertex3f(X1, Y1, Z2);
  glVertex3f(X2, Y1, Z2);
  glVertex3f(X2, Y2, Z2);
  glVertex3f(X1, Y2, Z2);
  
  glEnd();
}

    
extern cspace nodeQuery[0x1000];
extern unsigned long nodeQueryType[0x1000];
extern unsigned long nodeQueryLen;

void drawQuery()
{
  glColor3f(1.0, 0.0, 0.0);
  for (int lp=0; lp < nodeQueryLen; lp++)
  {
    unsigned long nodeType = nodeQueryType[lp] & 0xE;
    unsigned long nodeSubtype = (nodeQueryType[lp] & 0x3F0) >> 4;
    
    if (nodeType == 0)
    {
      float bval = 1 - (0.5 + ((float)(nodeSubtype % 8)/16));
      float gval = 1 - (0.5 + ((float)(nodeSubtype / 8)/16));
      glColor3f(0.0, gval, bval);
    }
    else
      glColor3f(1.0, 0.0, 0.0);
      
    drawCSpace(&nodeQuery[lp]);
  }
  glColor3f(1.0, 1.0, 1.0);
}
*/