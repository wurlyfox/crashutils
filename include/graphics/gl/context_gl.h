#ifndef CONTEXT_GL_H
#define CONTEXT_GL_H

#include "graphics/context.h"
#include "graphics/types.h"

class context_gl : public context
{
  protected:
  
  double model_view[16];
  double projection[16];
  int viewport[4];
  
  public:
  
  virtual void render() {};
  virtual void draw();
  virtual void update() {};
  virtual bool setActive() {};
  
  void setProjection(int mode);
  void setViewport(int width, int height);
  dpoint project2d(double X, double Y, double Z);
};

#endif