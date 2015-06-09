#include <gl\gl.h>
#include <gl\glu.h>

#include "graphics/gl/model_spr_gl.h"

//absolute draw, draw the model directly at its position
void model_spr_gl::drawAt(vector loc)
{
  glPushMatrix();
  
  float X = (loc.X * 8)/0x1000;
  float Y = (loc.Y * 8)/0x1000;
  float Z = (loc.Z * 8)/0x1000;
  
  glTranslatef(X, Y, Z);
  
  draw();
  
  glPopMatrix();
}

void model_spr_gl::draw()
{
  if (!render) { return; }
  
  glEnable(GL_TEXTURE_2D);
  
  glBegin(GL_TRIANGLES);
  glColor3f(1.0f,1.0f,1.0f);
  glTexCoord2fv(&quads[0].tex->coords.C.X); 
  //glColor3fv(&quads[0].vertexA->Rf);
  glVertex3fv(&quads[0].vertexC->X);              
  glTexCoord2fv(&quads[0].tex->coords.B.X); 
  //glColor3fv(&quads[0].vertexB->Rf);
  glVertex3fv(&quads[0].vertexB->X);
  glTexCoord2fv(&quads[0].tex->coords.D.X); 
  //glColor3fv(&quads[0].vertexC->Rf);
  glVertex3fv(&quads[0].vertexD->X);
  glTexCoord2fv(&quads[0].tex->coords.A.X); 
  //glColor3fv(&quads[0].vertexA->Rf);
  glVertex3fv(&quads[0].vertexA->X);              
  glTexCoord2fv(&quads[0].tex->coords.B.X); 
  //glColor3fv(&quads[0].vertexB->Rf);
  glVertex3fv(&quads[0].vertexB->X);
  glTexCoord2fv(&quads[0].tex->coords.C.X); 
  //glColor3fv(&quads[0].vertexC->Rf);
  glVertex3fv(&quads[0].vertexC->X);
  glEnd();
  
  glDisable(GL_TEXTURE_2D);
}