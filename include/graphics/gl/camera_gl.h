#ifndef CAMERA_GL_H
#define CAMERA_GL_H

#include "graphics/camera.h"

class camera_gl : public camera
{
  public:
  
  camera_gl() : camera() {}
  camera_gl(camera *cam) : camera(cam) {}
  
  void view(); 
  camera *newCamera() { return new camera_gl(this); }
};

#endif