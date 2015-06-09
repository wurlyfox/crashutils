#ifndef MODEL_GL_H
#define MODEL_GL_H

#include "graphics/model.h"

//#include "model_wld_gl.h"
//#include "model_obj_gl.h"
//#include "model_spr_gl.h"

class model_gl : public model
{
  public:
  
  void orient();
  void draw();
};
  
#endif