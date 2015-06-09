#include <gl\gl.h>
#include <gl\glu.h>

#include "graphics/gl/model_gl.h"

void model_gl::orient()
{  
  glTranslatef(trans.X, trans.Y, trans.Z);
    
  //euler YXY format
  glRotatef((rot.X-rot.Z), 0.0f, 1.0f, 0.0f);
  glRotatef(       -rot.Y, 1.0f, 0.0f, 0.0f);
  glRotatef(        rot.Z, 0.0f, 1.0f, 0.0f);
  
  glScalef(scale.X, scale.Y, scale.Z);
}  

void model_gl::draw()
{
  glPushMatrix();
  orient();
  
  if (type == 1)
  {
    object_model->draw(colors);
  }
  else if (type == 2)
    sprite_model->draw();
  else if (type == 3)
    fragment_model->draw();
    
  glPopMatrix();
}