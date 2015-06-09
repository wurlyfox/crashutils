#ifndef SCENE_GL_H
#define SCENE_GL_H

#define GL_VERTEX_ARRAY_ENABLE

#include "graphics/scene.h"

#include "geometry_gl.h"
#include "camera_gl.h"
#include "zone_gl.h"
#include "model_gl.h"

class scene_gl : public scene
{
  private:
  
  bool inited;
  
  float LightAmbient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
  float LightDiffuse[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
  float LightPosition[4]= { 1.0f, 1.0f, 1.0f, 1.0f }; 

  void init();
  void draw();
  
  public:
  
  scene_gl(NSD *_nsd, NSF *_nsf);
  scene_gl(scene *shared, unsigned long flags); 
  ~scene_gl();
};

#endif