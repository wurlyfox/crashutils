#include <gl\gl.h>
#include <gl\glu.h>

#include "graphics/gl/model_obj_gl.h"

//draw the model at the specified location
void model_obj_gl::drawAt(vector loc)
{
  float X = ((float)loc.X / 0x1000) * SCALE_OBJECT;
  float Y = ((float)loc.Y / 0x1000) * SCALE_OBJECT;
  float Z = ((float)loc.Z / 0x1000) * SCALE_OBJECT;
     
  glPushMatrix();
  
  glTranslatef(X, Y, Z);

  draw();

  glPopMatrix();
}

//draw the model at the current location determined by modelview matrix
void model_obj_gl::draw(fcolor *colors)
{
  if (!render) { return; }
     
  glPushMatrix();
    
  glEnable(GL_TEXTURE_2D);
	
  #ifdef GL_VERTEX_ARRAY_ENABLE
  
  if (!colors)
  {
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  
    glEnableClientState(GL_VERTEX_ARRAY);       
    glEnableClientState(GL_COLOR_ARRAY); 
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    float *colorPtr = (float*)(((unsigned char*)vertices) + 12);
      
    glColorPointer(3, GL_FLOAT, sizeof(vertex), colorPtr);
    glNormalPointer(GL_FLOAT, 0, normals);
    glVertexPointer(3, GL_FLOAT, sizeof(vertex), vertices);
    glTexCoordPointer(2, GL_FLOAT, 0, texcoords);

    glDrawArrays(GL_TRIANGLES, 0, polyTexCount*3);
    
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
   
    float *nonvert = (float*)(vertices + (polyTexCount*3));
    float *nonnorm = (float*)(normals + (polyTexCount*3));
    float *noncolorPtr = (float*)(nonvert + 3);
    
    glColorPointer(3, GL_FLOAT, sizeof(vertex), noncolorPtr);
    glNormalPointer(GL_FLOAT, 0, nonnorm);
    glVertexPointer(3, GL_FLOAT, sizeof(vertex), nonvert);
      
    glDrawArrays(GL_TRIANGLES, 0, polyNonCount*3);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY); 
    glDisableClientState(GL_NORMAL_ARRAY);

    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
  }
  else 
  {
    glEnableClientState(GL_VERTEX_ARRAY);       
    glEnableClientState(GL_COLOR_ARRAY); 
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
          
    glColorPointer(3, GL_FLOAT, 0, colors);
    glVertexPointer(3, GL_FLOAT, sizeof(vertex), vertices);
    glTexCoordPointer(2, GL_FLOAT, 0, texcoords);

    glDrawArrays(GL_TRIANGLES, 0, polyTexCount*3);
    
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
   
    float *nonvert = (float*)(vertices + (polyTexCount*3));
    float *nonnorm = (float*)(normals + (polyTexCount*3));
    float *noncolors = (float*)(colors + (polyTexCount*3));
    
    glColorPointer(3, GL_FLOAT, 0, noncolors);
    glVertexPointer(3, GL_FLOAT, sizeof(vertex), nonvert);
      
    glDrawArrays(GL_TRIANGLES, 0, polyNonCount*3);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY); 
  }
  #else
  
  glBegin(GL_TRIANGLES);
  for (int lp = 0; lp < polyTexCount; lp++)
  {
    glTexCoord2fv(&polygons[lp].tex->coords.A.X); 
    glColor3fv(&polygons[lp].color.R);
    glNormal3fv(&polygons[lp].normalA->X);
    glVertex3fv(&polygons[lp].vertexA->X);              
    glTexCoord2fv(&polygons[lp].tex->coords.B.X); 
    glColor3fv(&polygons[lp].color.R);
    glNormal3fv(&polygons[lp].normalB->X);
    glVertex3fv(&polygons[lp].vertexB->X);
    glTexCoord2fv(&polygons[lp].tex->coords.C.X); 
    glColor3fv(&polygons[lp].color.R);
    glNormal3fv(&polygons[lp].normalC->X);
    glVertex3fv(&polygons[lp].vertexC->X);
  }
  glEnd();
  
  glDisable(GL_TEXTURE_2D);

  glBegin(GL_TRIANGLES);
  for (int lp = polyTexCount; lp < polyTexCount+polyNonCount; lp++)
  {      
    glColor3fv(&polygons[lp].color.R);
    glNormal3fv(&polygons[lp].normalA->X);
    glVertex3fv(&polygons[lp].vertexA->X);              
    glColor3fv(&polygons[lp].color.R);
    glNormal3fv(&polygons[lp].normalB->X);
    glVertex3fv(&polygons[lp].vertexB->X);
    glColor3fv(&polygons[lp].color.R);
    glNormal3fv(&polygons[lp].normalC->X);
    glVertex3fv(&polygons[lp].vertexC->X);    	  
  }
  glEnd();
  
  #endif
  
  glPopMatrix();
}