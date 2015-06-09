#ifndef ZONE_GL_H
#define ZONE_GL_H

#include "graphics/zone.h"

#include "immediate.h"

class collisionNode_gl : public collisionNode
{ 
  public:
  
  void draw();
};

class section_gl : public section
{   
  public:
  
  void drawFrom(point loc, int scale);
};
  
class zone_gl : public zone
{  
  private:
  
  void drawMisc(unsigned long flags);
  void drawCollisions();
  
  public:
  
  zone_gl()
  {
    for (int lp = 0; lp < MAX_SECTIONS; lp++)
      sections[lp] = new section_gl;
    
    for (int lp = 0; lp < MAX_COLLISIONS; lp++)
      collisions[lp] = new collisionNode_gl;
  }
  
  ~zone_gl()
  {
    for (int lp = 0; lp < MAX_SECTIONS; lp++)
      delete sections[lp];
    
    for (int lp = 0; lp < MAX_COLLISIONS; lp++)
      delete collisions[lp];
  }
  
  void draw(unsigned long flags);
};

#endif