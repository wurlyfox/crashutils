#ifndef MODEL_WLD_GL_H
#define MODEL_WLD_GL_H

#include "graphics/model_wld.h"

class model_wld_gl : public model_wld
{
  public:
  
  virtual void drawAt(vector loc);
  virtual void draw();
};

#endif