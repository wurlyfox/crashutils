#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glext.h>

#include "graphics/gl/texture_buffer_gl.h"

void texture_buffer_gl::init()
{
  if (inited)
  {
	  glEnable(GL_TEXTURE_2D);  
    glBindTexture(GL_TEXTURE_2D, globalTex);
	}
  else
  {
    inited = true;

	  glEnable(GL_TEXTURE_2D);  

    glGenTextures(1, (GLuint*)&globalTex);
    glBindTexture(GL_TEXTURE_2D, globalTex);
    
    unsigned long *pixBuf = (unsigned long*)calloc(GL_TEXIMAGE_W*GL_TEXIMAGE_H,sizeof(unsigned long));
    
    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, GL_TEXIMAGE_W, GL_TEXIMAGE_H, GL_RGBA, GL_UNSIGNED_BYTE, pixBuf);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE ); 
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); 
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);   //ALTERNATIVE would be GL_REPEAT
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
    
    free(pixBuf);
  }
}

void texture_buffer_gl::subTextureCreate(textureCacheEntry *tex)
{
  //glEnable(GL_TEXTURE_2D);  
  //glBindTexture(GL_TEXTURE_2D, globalTex);

  texture_buffer::subTextureCreate(tex);
}

void texture_buffer_gl::storeBlock(unsigned long *pixBuf, int x, int y, int w, int h)
{
  glTexSubImage2D(GL_TEXTURE_2D, 0, x, GL_TEXIMAGE_H - (y + h), w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixBuf);    
}

fquad2 texture_buffer_gl::getBlockCoords(point A, point B, point C, point D)
{
  fquad2 blockCoords = { { GL_COORDX(A.X), GL_COORDY(A.Y) },
                         { GL_COORDX(B.X), GL_COORDY(B.Y) },
                         { GL_COORDX(C.X), GL_COORDY(C.Y) },
                         { GL_COORDX(D.X), GL_COORDY(D.Y) } };
                         
  return blockCoords;
}