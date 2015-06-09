#include "model_frg.h"

void model_frg::load(unsigned long texEID, int count, squad2 *quadInfo, unsigned char *texInfoArray, unsigned long scale, texture_buffer *texbuf, prim_alloc *prims)
{
  positionMode = 1;   //no drawing internal translation
  
  // determine texture used by each quad in the fragment
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
       
  texture *quadTex = texbuf->getTexture(&texEntry);
  
  // allocate quads and vertices; occupy accordingly
  quadCount = count;
  vertCount = quadCount * 4;
  
  vertices = prims->allocVertices(vertCount);
  quads    = prims->allocQuads(quadCount);
    
  for (int lp = 0; lp < quadCount; lp++)
  {
    vertex *vert = &vertices[lp*4];
    squad2 *quad = &quadInfo[lp];
    
    vert[0].X = ((float)(quad->A.X << scale) * 8) / 0x1000;
    vert[0].Y = ((float)(quad->A.Y << scale) * 8) / 0x1000;
    vert[0].Z = 0;
    
    vert[1].X = ((float)(quad->B.X << scale) * 8) / 0x1000;
    vert[1].Y = ((float)(quad->B.Y << scale) * 8) / 0x1000;
    vert[1].Z = 0;
    
    vert[2].X = ((float)(quad->C.X << scale) * 8) / 0x1000;
    vert[2].Y = ((float)(quad->C.Y << scale) * 8) / 0x1000;
    vert[2].Z = 0;
    
    vert[3].X = ((float)(quad->D.X << scale) * 8) / 0x1000;
    vert[3].Y = ((float)(quad->D.Y << scale) * 8) / 0x1000;
    vert[3].Z = 0;
    
    quads[lp].vertexA = &vert[0];
    quads[lp].vertexB = &vert[1];
    quads[lp].vertexC = &vert[2];
    quads[lp].vertexD = &vert[3];  

    quads[lp].tex      = quadTex;
    quads[lp].textured = true;    
  }

  render = true;
}

//draw the model at its location
void model_frg::drawAtLocation()
{
  drawAt(location);
}