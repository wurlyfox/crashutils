#include <gl\gl.h>
#include <gl\glu.h>

#include "graphics/gl/model_wld_gl.h"

//draw the model at the specified location
void model_wld_gl::drawAt(vector loc)
{
  float X = ((float)loc.X / 0x1000) * SCALE_WORLD;
  float Y = ((float)loc.Y / 0x1000) * SCALE_WORLD;
  float Z = ((float)loc.Z / 0x1000) * SCALE_WORLD;
  
  glPushMatrix();
  
  if (positionMode == -1)
    return;
   // glLoadIdentity();
    
  glTranslatef(X, Y, Z);
    
  draw();
  
  glPopMatrix();
}

//draw the model at the current location determined by the modelview matrix
void model_wld_gl::draw()
{ 
  if (selected)
  {  
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  }
  
  glEnable(GL_TEXTURE_2D);  
    
  #ifdef GL_VERTEX_ARRAY_ENABLE
  
  glEnableClientState(GL_VERTEX_ARRAY);       
  glEnableClientState(GL_COLOR_ARRAY); 
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  
  float *colors = (float*)(((unsigned char*)vertices) + 12);
    
  glColorPointer(3, GL_FLOAT, sizeof(vertex), colors);
  glVertexPointer(3, GL_FLOAT, sizeof(vertex), vertices);
  glTexCoordPointer(2, GL_FLOAT, 0, texcoords);

  glDrawArrays(GL_TRIANGLES, 0, polyTexCount*3);
  
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisable(GL_TEXTURE_2D);
 
  /*
  float *nonvert = (float*)(((unsigned char*)vertices) + (sizeof(vertex)*polyTexCount));
  float *noncolors = (float*)(((unsigned char*)colors) + (sizeof(vertex)*polyTexCount));
  
  glColorPointer(3, GL_FLOAT, sizeof(vertex), noncolors);
  glVertexPointer(3, GL_FLOAT, sizeof(vertex), nonvert);
    
  glDrawArrays(GL_TRIANGLES, 0, polyNonCount*3);
  */
   
  glDisableClientState(GL_VERTEX_ARRAY);               // Disable Vertex Arrays
  glDisableClientState(GL_COLOR_ARRAY); 
  
  if (selected)
  {
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
  }
  
  #else
 
  glBegin(GL_TRIANGLES);  
  for (int lp = 0; lp < polyTexCount; lp++)
  {
    glTexCoord2fv(&polygons[lp].tex->coords.A.X); 
    glColor3fv(&polygons[lp].vertexA->Rf);
    glVertex3fv(&polygons[lp].vertexA->X);              
    glTexCoord2fv(&polygons[lp].tex->coords.B.X);
    glColor3fv(&polygons[lp].vertexB->Rf);
    glVertex3fv(&polygons[lp].vertexB->X);
    glTexCoord2fv(&polygons[lp].tex->coords.C.X);
    glColor3fv(&polygons[lp].vertexC->Rf);
    glVertex3fv(&polygons[lp].vertexC->X);
  }
  glEnd();
  glDisable(GL_TEXTURE_2D);
   
  glBegin(GL_TRIANGLES);   
  for (int lp = polyTexCount; lp < polyTexCount+polyNonCount; lp++)
  {        
    glColor3fv(&polygons[lp].vertexA->Rf);
    glVertex3fv(&polygons[lp].vertexA->X);              
    glColor3fv(&polygons[lp].vertexB->Rf);
    glVertex3fv(&polygons[lp].vertexB->X);
    glColor3fv(&polygons[lp].vertexC->Rf);
    glVertex3fv(&polygons[lp].vertexC->X);       
  }
  glEnd();
  
  #endif
}