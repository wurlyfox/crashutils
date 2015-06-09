#include <gl\gl.h>
#include <gl\glu.h>

#include "graphics/gl/zone_gl.h"

void zone_gl::draw(unsigned long flags)
{
  /*
  if (flags & ZONE_VIEWMODE_VIEWWORLDS)
  {
    for (int lp = 0; lp < worldCount; lp++)
      worlds[lp]->drawFrom(location);
  }
  
  if (flags & ZONE_VIEWMODE_VIEWOBJECTS)
  {
    for (int lp = 0; lp < objectCount; lp++)
      objects[lp].drawFrom(&location);
  }
  */
  
  drawMisc(flags);   
}

void zone_gl::drawMisc(unsigned long flags)
{
  if (render)
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(2);
   
    if (flags & ZONE_VIEWMODE_VIEWSECTIONS)
    {
      for (int lp = 0; lp < sectionCount; lp++)
        sections[lp]->drawFrom(location, 8);
    }

    //draw a wireframe rectangle that shows zone boundaries
    if (flags & ZONE_VIEWMODE_VIEWBOUNDS)
      drawRect3(bounds, 8);
    
    if (flags & ZONE_VIEWMODE_VIEWCOLLISIONS) 
      drawCollisions();
    
    //drawQuery();
      
    glLineWidth(1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
}

void zone_gl::drawCollisions()
{
  float X = ((float)location.X * 8)/0x1000;
  float Y = ((float)location.Y * 8)/0x1000;
  float Z = ((float)location.Z * 8)/0x1000;
  
  glPushMatrix();
  
  glTranslatef(X, Y, Z);
  
  glBegin(GL_QUADS);
  glColor3f(1.0, 1.0, 1.0);
  for (int lp = 0; lp < colCount; lp++)
  {
    unsigned long nodeValue = (collisions[lp]->data);
    unsigned long nodeType = (nodeValue & 0xE) >> 1;
    unsigned long nodeSubtype = (nodeValue & 0x3F0) >> 4;
    
    if (nodeType == 0)           //solid scenery/wall = white
    {
      float g = 1.0 - ((float)nodeSubtype*0.015625f);
      glColor3f(1.0, g, 1.0);
    }
    else if (nodeType == 1)      //floor = light green
    {
      float a = 0.5 - ((float)nodeSubtype*0.0078125f);
      glColor3f(a, 1.0, a);
    }
    else if (nodeType == 2)      //level bound wall = light blue
      glColor3f(0.5, 0.5, 1.0);
    else if (nodeType == 3)      //pit death = red
      glColor3f(1.0, 0.0, 0.0); 
    else if (nodeType == 4)
      glColor3f(1.0, 0.5, 0.5);
      
    glVertex3f(collisions[lp]->bound.P1.X, collisions[lp]->bound.P1.Y, collisions[lp]->bound.P1.Z);
    glVertex3f(collisions[lp]->bound.P2.X, collisions[lp]->bound.P1.Y, collisions[lp]->bound.P1.Z);
    glVertex3f(collisions[lp]->bound.P2.X, collisions[lp]->bound.P2.Y, collisions[lp]->bound.P1.Z);
    glVertex3f(collisions[lp]->bound.P1.X, collisions[lp]->bound.P2.Y, collisions[lp]->bound.P1.Z);
  
    glVertex3f(collisions[lp]->bound.P1.X, collisions[lp]->bound.P1.Y, collisions[lp]->bound.P1.Z);
    glVertex3f(collisions[lp]->bound.P1.X, collisions[lp]->bound.P1.Y, collisions[lp]->bound.P2.Z);
    glVertex3f(collisions[lp]->bound.P1.X, collisions[lp]->bound.P2.Y, collisions[lp]->bound.P2.Z);
    glVertex3f(collisions[lp]->bound.P1.X, collisions[lp]->bound.P2.Y, collisions[lp]->bound.P1.Z);
  
    glVertex3f(collisions[lp]->bound.P2.X, collisions[lp]->bound.P1.Y, collisions[lp]->bound.P1.Z);
    glVertex3f(collisions[lp]->bound.P2.X, collisions[lp]->bound.P1.Y, collisions[lp]->bound.P2.Z);
    glVertex3f(collisions[lp]->bound.P2.X, collisions[lp]->bound.P2.Y, collisions[lp]->bound.P2.Z);
    glVertex3f(collisions[lp]->bound.P2.X, collisions[lp]->bound.P2.Y, collisions[lp]->bound.P1.Z);
    
    glVertex3f(collisions[lp]->bound.P1.X, collisions[lp]->bound.P1.Y, collisions[lp]->bound.P1.Z);
    glVertex3f(collisions[lp]->bound.P1.X, collisions[lp]->bound.P1.Y, collisions[lp]->bound.P2.Z);
    glVertex3f(collisions[lp]->bound.P2.X, collisions[lp]->bound.P1.Y, collisions[lp]->bound.P2.Z); 
    glVertex3f(collisions[lp]->bound.P2.X, collisions[lp]->bound.P1.Y, collisions[lp]->bound.P1.Z);
    
    glVertex3f(collisions[lp]->bound.P1.X, collisions[lp]->bound.P2.Y, collisions[lp]->bound.P1.Z);
    glVertex3f(collisions[lp]->bound.P1.X, collisions[lp]->bound.P2.Y, collisions[lp]->bound.P2.Z);
    glVertex3f(collisions[lp]->bound.P2.X, collisions[lp]->bound.P2.Y, collisions[lp]->bound.P2.Z); 
    glVertex3f(collisions[lp]->bound.P2.X, collisions[lp]->bound.P2.Y, collisions[lp]->bound.P1.Z);
    
    glVertex3f(collisions[lp]->bound.P1.X, collisions[lp]->bound.P1.Y, collisions[lp]->bound.P2.Z);
    glVertex3f(collisions[lp]->bound.P2.X, collisions[lp]->bound.P1.Y, collisions[lp]->bound.P2.Z);
    glVertex3f(collisions[lp]->bound.P2.X, collisions[lp]->bound.P2.Y, collisions[lp]->bound.P2.Z);
    glVertex3f(collisions[lp]->bound.P1.X, collisions[lp]->bound.P2.Y, collisions[lp]->bound.P2.Z);
  }
  glEnd();
  
  /*
  if (selCollision >= 0 && selCollision < colCount)
  {    
    glBegin(GL_QUADS);
    glColor3f(1.0, 0, 0);   
    collisions[selCollision].draw();
    glColor3f(1.0, 1.0, 1.0);
    glEnd();
  }
  */
  
  glPopMatrix();
}


/*
void section_gl::drawFrom(point loc, int scale)
{
  point totalLoc;
  totalLoc.X = location.X - loc.X;
  totalLoc.Y = location.Y - loc.Y;
  totalLoc.Z = location.Z - loc.Z;
  
  draw(totalLoc, scale);  
}
*/

void section_gl::drawFrom(point loc, int scale)
{
  if (!render) { return; }
   
  //TODO: implement variable path granularity
  if (pathLength > 1)
  {
    for (int lp=1; lp < pathLength; lp++)
      drawPath(loc, cameraPath[lp-1], cameraPath[lp], 8);
  }
}

void collisionNode_gl::draw()
{
  drawRect3f(bound);
}

//void collisionNode::drawFrom(point loc, int scale)
//{
//  drawRect3(bound, loc, scale);
//}