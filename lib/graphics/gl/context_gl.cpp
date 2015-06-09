#include <gl\gl.h>
#include <gl\glu.h>

#include "graphics/gl/context_gl.h"

void context_gl::draw()
{
  context::draw();
  
  glGetDoublev(GL_MODELVIEW_MATRIX, model_view);
}

void context_gl::setProjection(int mode)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();				

  if (mode == 0)
    gluPerspective(45.0f, (GLfloat)viewportWidth/(GLfloat)viewportHeight, 0.1f, 10000.0f);
  else if (mode == 1)
    glOrtho(-1.0f, 1.0f, 1.0f, -1.0f, 0.1f, 10000.0f);
    
  glMatrixMode(GL_MODELVIEW);	
  glLoadIdentity();			
}

void context_gl::setViewport(int width, int height)
{
  context::setViewport(width, height);
  
  if (height == 0)      
    height = 1;				                 

  glViewport(0, 0, width, height);
	
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();				

  gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 10000.0f);

  glMatrixMode(GL_MODELVIEW);	
  glLoadIdentity();			

  glGetDoublev(GL_PROJECTION_MATRIX, projection);
  glGetIntegerv(GL_VIEWPORT, viewport);  
}

dpoint context_gl::project2d(double X, double Y, double Z)
{
  dpoint win;
  gluProject(          X,          Y,         Z,
              model_view, projection,  viewport,
                  &win.X,     &win.Y,    &win.Z);
        
  win.Y = viewportHeight - win.Y;
  
  return win;
}