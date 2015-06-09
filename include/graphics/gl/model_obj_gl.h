#ifndef MODEL_OBJ_GL_H
#define MODEL_OBJ_GL_H

#include "graphics/model_obj.h"

class model_obj_gl : public model_obj
{
  public:
  
  virtual void drawAt(vector loc);
  virtual void draw(fcolor *colors = 0);
};

#endif