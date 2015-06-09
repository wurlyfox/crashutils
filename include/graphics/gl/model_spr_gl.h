#ifndef MODEL_SPR_GL_H
#define MODEL_SPR_GL_H

#include "graphics/model_spr.h"

class model_spr_gl : public model_spr
{
  public:
  
  virtual void drawAt(vector loc);
  virtual void draw();
};

#endif


