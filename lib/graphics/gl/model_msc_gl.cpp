#include <gl/gl.h>

#include "graphics/gl/model_msc_gl.h"

void model_msc_gl::draw()
{
  glColor3f(color.R, color.G, color.B);
  
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glEnableClientState(GL_VERTEX_ARRAY);       

  if (lines)
  {
    glVertexPointer(3, GL_FLOAT, 0, lines);
    glDrawArrays(GL_LINES, 0, lineCount*2);
  }
  if (triangles)
  {
    glVertexPointer(3, GL_FLOAT, 0, triangles);
    glDrawArrays(GL_TRIANGLES, 0, triangleCount*3);
  }
  if (quads)
  {
    //for (int lp=0; lp<quadCount; lp++)
    //{
      //glVertexPointer(3, GL_FLOAT, 0, &quads[lp]);
      //glDrawArrays(GL_LINE_LOOP, 0, 4);
    //}
    glVertexPointer(3, GL_FLOAT, 0, quads);
    glDrawArrays(GL_QUADS, 0, quadCount*4);
  }
  
  glDisableClientState(GL_VERTEX_ARRAY);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}