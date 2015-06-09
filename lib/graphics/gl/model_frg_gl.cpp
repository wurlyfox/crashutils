#include <gl\gl.h>
#include <gl\glu.h>

#include "graphics/gl/model_frg_gl.h"

//absolute draw, draw the model directly at its position
void model_frg_gl::drawAt(vector loc)
{
  glPushMatrix();
  
  float X = (loc.X * 8)/0x1000;
  float Y = (loc.Y * 8)/0x1000;
  float Z = (loc.Z * 8)/0x1000;
  
  glTranslatef(X, Y, Z);
  
  draw();
  
  glPopMatrix();
}

void model_frg_gl::draw()
{
  if (!render) { return; }
  
  glEnable(GL_TEXTURE_2D);
  
  glBegin(GL_TRIANGLES);
  for (int lp=0; lp<quadCount; lp++)
  {
    glColor3f(1.0f,1.0f,1.0f);
    glTexCoord2fv(&quads[lp].tex->coords.C.X); 
    //glColor3fv(&quads[0].vertexA->Rf);
    glVertex3fv(&quads[lp].vertexC->X);              
    glTexCoord2fv(&quads[lp].tex->coords.B.X); 
    //glColor3fv(&quads[0].vertexB->Rf);
    glVertex3fv(&quads[lp].vertexB->X);
    glTexCoord2fv(&quads[lp].tex->coords.D.X); 
    //glColor3fv(&quads[0].vertexC->Rf);
    glVertex3fv(&quads[lp].vertexD->X);
    glTexCoord2fv(&quads[lp].tex->coords.A.X); 
    //glColor3fv(&quads[0].vertexA->Rf);
    glVertex3fv(&quads[lp].vertexA->X);              
    glTexCoord2fv(&quads[lp].tex->coords.B.X); 
    //glColor3fv(&quads[0].vertexB->Rf);
    glVertex3fv(&quads[lp].vertexB->X);
    glTexCoord2fv(&quads[lp].tex->coords.C.X); 
    //glColor3fv(&quads[0].vertexC->Rf);
    glVertex3fv(&quads[lp].vertexC->X);
  }
  glEnd();
  
  glDisable(GL_TEXTURE_2D);
}

