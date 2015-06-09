#include <gl\gl.h>
#include <gl\glu.h>

#include "graphics/gl/camera_gl.h"

void camera_gl::view()
{
  if (nested)
    nested->view();
    
  // model view mode
  if (mode == 0)
  {
    glTranslatef(-location.X, -location.Y, -location.Z);
    glRotatef(-rotation.Y, 1.0f, 0.0f, 0.0f);
    glRotatef(-rotation.X, 0.0f, 1.0f, 0.0f);
    glRotatef( rotation.Z, 0.0f, 0.0f, 1.0f); 
  }
  // inverse model view mode
  else if (mode == 1)
  {
    glRotatef( rotation.Y, 1.0f, 0.0f, 0.0f);
    glRotatef( rotation.X, 0.0f, 1.0f, 0.0f);
    glRotatef( rotation.Z, 0.0f, 0.0f, 1.0f); 
    glTranslatef(-location.X, -location.Y, -location.Z);
  }
  // crash mode
  else if (mode == 2)
  {
    update();
    
    glRotatef( -rotation.Y, 1.0f, 0.0f, 0.0f);
    glRotatef( -rotation.X, 0.0f, 1.0f, 0.0f);
    glRotatef( rotation.Z, 0.0f, 0.0f, 1.0f); 
    glTranslatef(-location.X, -location.Y, -location.Z);
  }
}