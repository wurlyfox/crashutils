#include "model_spr.h"

void model_spr::load(unsigned long texEID, unsigned char *texInfoArray, unsigned long scale, texture_buffer *texbuf, prim_alloc *prims)
{
  positionMode = 1;   //no drawing internal translation
  
  vertices = prims->allocVertices(4);
  
  vertices[0].X = -(((float)scale * 8) / 0x1000);
  vertices[0].Y =   ((float)scale * 8) / 0x1000;
  vertices[0].Z =                             0;
  
  vertices[1].X = ((float)scale * 8) / 0x1000;
  vertices[1].Y = ((float)scale * 8) / 0x1000;
  vertices[1].Z =                           0;
  
  vertices[2].X = -(((float)scale * 8) / 0x1000);
  vertices[2].Y = -(((float)scale * 8) / 0x1000);
  vertices[2].Z =                              0;
  
  vertices[3].X =   ((float)scale * 8) / 0x1000;
  vertices[3].Y = -(((float)scale * 8) / 0x1000);
  vertices[3].Z =                             0;
  
  quads = prims->allocQuads(1);
  
  quads[0].vertexA = &vertices[0];
  quads[0].vertexB = &vertices[1];
  quads[0].vertexC = &vertices[2];
  quads[0].vertexD = &vertices[3];
  
  unsigned long texInfoA = getWord(texInfoArray, 0, true);
  unsigned long texInfoB = getWord(texInfoArray, 4, true);
	  
  unsigned short texCoordsOffset = ((texInfoB >> 22) & 0x3FF);
  unsigned short colorMode       = ((texInfoB >> 20) & 3);
  unsigned short section128      = ((texInfoB >> 18) & 3);
    
  unsigned short offsetX = ((texInfoB >> 13) & 0x1F);  
  unsigned short offsetY =         (texInfoB & 0x1F); 
        
  unsigned char clutIndex   = (texInfoB >> 6) & 0x7F;
  unsigned char clutPalette = (texInfoA >> 24) & 0xF;  
      
  textureEntry texEntry;
  texEntry.coords      = texCoordsOffset;
  texEntry.colorMode   = colorMode;
  texEntry.sect128     = section128;
  texEntry.offsetX     = offsetX;
  texEntry.offsetY     = offsetY;
  texEntry.clutIndex   = clutIndex;
  texEntry.clutPalette = clutPalette;
  texEntry.chunkIndex  = texbuf->getTextureChunk(texEID);
     
  quads[0].tex      = texbuf->getTexture(&texEntry);
  quads[0].textured = true;    
 
  render = true;
}

//draw the model at its location
void model_spr::drawAtLocation()
{
  drawAt(location);
}