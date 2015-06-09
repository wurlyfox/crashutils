#ifndef MODEL_FRG_GL_H
#define MODEL_FRG_GL_H

#include "graphics/model_frg.h"

class model_frg_gl : public model_frg
{
  public:
  
  virtual void drawAt(vector loc);
  virtual void draw();
};

#endif