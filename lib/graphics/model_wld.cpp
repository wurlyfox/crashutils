#include "model_wld.h"

void model_wld::load(entry *wgeo, texture_buffer *texbuf, prim_alloc *prims)
{
  vertCount = 0;
  polyCount = 0;

  unsigned char *item1 = wgeo->itemData[0];
  
  positionMode = getWord(item1, 0x1C, true);
  if (positionMode == 0)
  {
    signed long modelX = getWord(item1, 0, true);
    signed long modelY = getWord(item1, 4, true);
    signed long modelZ = getWord(item1, 8, true);
    
    location.X = modelX;
    location.Y = modelY;
    location.Z = modelZ;
    
    unsigned long modelW = getWord(item1, 0xC, true);
    unsigned long modelH = getWord(item1, 0x10, true);
    unsigned long modelD = getWord(item1, 0x14, true);
    
    bound.P1.X = ((float)modelX * 8) / 0x1000;
    bound.P1.Y = ((float)modelY * 8) / 0x1000;
    bound.P1.Z = ((float)modelZ * 8) / 0x1000;
  
    bound.P2.X = bound.P1.X + (((float)modelW * 8) / 0x1000);
    bound.P2.Y = bound.P1.Y + (((float)modelH * 8) / 0x1000);
    bound.P2.Z = bound.P1.Z + (((float)modelD * 8) / 0x1000); 
  }
  
  loadVertices(wgeo, prims);
  loadPolygons(wgeo, texbuf, prims);
  
  selected = false;
}

void model_wld::loadVertices(entry *wgeo, prim_alloc *prims)
{
  if (wgeo->itemData[2] == 0 || wgeo->itemSize[2] == 0) { return; }
  
  int size              = wgeo->itemSize[2];
  unsigned char *buffer = wgeo->itemData[2];
  
  vertCount = size/(sizeof(unsigned long) * 2);

  vertices = prims->allocVertices(vertCount);
  
  for (int lp = 0; lp < vertCount; lp++)
  {
    int bufferOffset = lp * 8;
    unsigned long left  = getWord(buffer,   bufferOffset, true);
    unsigned long right = getWord(buffer, bufferOffset+4, true);
	
    signed short X = (right) & 0xFFF8;
    signed short Y = (right >> 16) & 0xFFF8;

    int Z = (left >> 24) & 0xFF; 
    int zcbits = (right >> 16) & 7;
    int zdbits = (right & 6);

    signed short zval = (Z<<3)+((zdbits<<10)+(zcbits<<13));

    Z = zval;

	  //psx GTE reads these values as fixed point, gl as floats
	  //do the appropriate conversions before storing so gl can interpret correctly
	  vertices[lp].X = ((float)X * 8) / 0x1000;
	  vertices[lp].Y = ((float)Y * 8) / 0x1000;
	  vertices[lp].Z = ((float)Z * 8) / 0x1000;

    unsigned char R = ((left & 0x000000FF));
    unsigned char G = ((left & 0x0000FF00) >>  8);
    unsigned char B = ((left & 0x00FF0000) >> 16);

    vertices[lp].Rf = ((float)R / 256);
    vertices[lp].Gf = ((float)G / 256);
    vertices[lp].Bf = ((float)B / 256);
  }
}

#ifdef GL_VERTEX_ARRAY_ENABLE
void model_wld::loadPolygons(entry *wgeo, texture_buffer *texbuf, prim_alloc *prims)
{
  if (wgeo->itemData[1] == 0 || wgeo->itemSize[1] == 0) {  return; }
  
  int size                    = wgeo->itemSize[1];
  unsigned char *buffer       = wgeo->itemData[1];

  unsigned char *texEIDArray  = wgeo->itemData[0] + 0x20;
  unsigned char *texInfoArray = wgeo->itemData[0] + 0x40;
  
  polyCount    = size/(sizeof(unsigned long) * 2);
  polyTexCount = 0;

  vertex *vertexList = vertices;
  prims->preparepPolygons(vertices, texcoords);
  
  vertex  *pVertex;
  fpoint2 *pTexcoord;
  for (int lp = 0; lp < polyCount; lp++)
  {
    int bufferOffset    = lp * 8;
    unsigned long left  = getWord(buffer,   bufferOffset, true);
    unsigned long right = getWord(buffer, bufferOffset+4, true);

    unsigned short texInfoOffset = (left >> 8) & 0xFFF;    
    unsigned long texInfoA       = getWord(texInfoArray, texInfoOffset*4, true);
    
    bool textured = (texInfoA >> 31) & 1;
    
    if (textured)
    {
      prims->allocpPolygon(pVertex, pTexcoord);  

      int aOffset = (right >> 20) & 0xFFF;
      int bOffset = (right >> 8) & 0xFFF;
      int cOffset = (left >> 20) & 0xFFF;
                    
      pVertex[0] = vertexList[aOffset];
      pVertex[1] = vertexList[bOffset];
      pVertex[2] = vertexList[cOffset];
        
      unsigned short texEIDOffset = (left >> 5)  & 0x7;
          
      unsigned char semiTrans = (texInfoA >> 29) & 3;
    
      unsigned long texInfoB = getWord(texInfoArray, (texInfoOffset*4)+4, true); 
      unsigned long texEID   = getWord(texEIDArray,   (texEIDOffset*4),   true);
	  
      if (texEID)
      {
        unsigned short texCoordsOffset = ((texInfoB >> 22) & 0x3FF);
        unsigned short colorMode       = ((texInfoB >> 20) & 3);
        unsigned short section128      = ((texInfoB >> 18) & 3);
      
        //unsigned short offsetX = ((texInfoB >> 10) & 0xF8) >> colorMode;  //multiple of 4 or else, varies for cmode
        //unsigned short offsetY = (texInfoB & 0x1F) << 2;   
        unsigned short offsetX = ((texInfoB >> 13) & 0x1F);  
        unsigned short offsetY =         (texInfoB & 0x1F);     
       
        //int texIndex = frameBuffer.getTexture(texEID, colorMode);
        unsigned char clutIndex = (texInfoB >> 6) & 0x7F;
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
       
        texture *tex = texbuf->getTexture(&texEntry);
        
        pTexcoord[0] = tex->coords.A;
        pTexcoord[1] = tex->coords.B;
        pTexcoord[2] = tex->coords.C;
      }
      else
      {
        pTexcoord[0] = {0, 0};
        pTexcoord[1] = {0, 0};
        pTexcoord[2] = {0, 0};
      }
      
      polyTexCount++;             
    }
  }

  polyNonCount = 0;
  
  for (int lp = 0; lp < polyCount; lp++)
  {
    int bufferOffset    = lp * 8;
    unsigned long left  = getWord(buffer,   bufferOffset, true);
    unsigned long right = getWord(buffer, bufferOffset+4, true);
    
    unsigned short texInfoOffset = (left >> 8) & 0xFFF;    
    unsigned long texInfoA       = getWord(texInfoArray, texInfoOffset*4, true);
    
    bool textured = (texInfoA >> 31) & 1;
    
    if (!textured)
    {
      prims->allocpPolygon(pVertex);
      
      int aOffset = (right >> 20) & 0xFFF;
      int bOffset = (right >> 8) & 0xFFF;
      int cOffset = (left >> 20) & 0xFFF;
                    
      pVertex[0] = vertexList[(aOffset/6)];
      pVertex[1] = vertexList[(bOffset/6)];
      pVertex[2] = vertexList[(cOffset/6)];
             
      polyNonCount++;             
    }
  }
}

#else

void model_wld::loadPolygons(entry *wgeo, texture_buffer *texbuf, prim_alloc *prims)
{
  if (wgeo->itemData[0] == 0 || wgeo->itemSize[0] == 0) { return; }
  if (wgeo->itemData[1] == 0 || wgeo->itemSize[1] == 0) { return; }
  
  int size                    = wgeo->itemSize[1];
  unsigned char *buffer       = wgeo->itemData[1];

  unsigned char *texEIDArray  = wgeo->itemData[0] + 0x20;
  unsigned char *texInfoArray = wgeo->itemData[0] + 0x40;
  
  polyCount = size/(sizeof(unsigned long) * 2);
  polygons  = prims->allocPolygons(polyCount);
  
  polyTexCount = 0;
  polyNonCount = 0;
  
  for (int lp = 0; lp < polyCount; lp++)
  {
    int bufferOffset    = lp * 8;
    unsigned long left  = getWord(buffer,   bufferOffset, true);
    unsigned long right = getWord(buffer, bufferOffset+4, true);
	
    int aOffset = (right >> 20) & 0xFFF;
    int bOffset = (right >> 8) & 0xFFF;
    int cOffset = (left >> 20) & 0xFFF;
    
    polygons[lp].vertexA = &vertices[aOffset];
    polygons[lp].vertexB = &vertices[bOffset];
    polygons[lp].vertexC = &vertices[cOffset];
    
    unsigned short texInfoOffset    = (left >> 8)  & 0xFFF;
    unsigned short texEIDOffset     = (left >> 5)  &   0x7;
        
    unsigned long texInfoA = getWord(texInfoArray, texInfoOffset*4, true);

    bool textured = (texInfoA >> 31) & 1;
    
    unsigned char semiTrans = (texInfoA >> 29) & 3;
    
    if (textured)
    {
      unsigned long texInfoB = getWord(texInfoArray, (texInfoOffset*4)+4, true); 
      unsigned long texEID   = getWord(texEIDArray,   (texEIDOffset*4),   true);
	  
      unsigned short texCoordsOffset = ((texInfoB >> 22) & 0x3FF);
      unsigned short colorMode       = ((texInfoB >> 20) & 3);
      unsigned short section128      = ((texInfoB >> 18) & 3);
    
      //unsigned short offsetX = ((texInfoB >> 10) & 0xF8) >> colorMode;  //multiple of 4 or else, varies for cmode
      //unsigned short offsetY = (texInfoB & 0x1F) << 2;   
      unsigned short offsetX = ((texInfoB >> 13) & 0x1F);  
      unsigned short offsetY =         (texInfoB & 0x1F);     
     
      //int texIndex = frameBuffer.getTexture(texEID, colorMode);
      unsigned char clutIndex = (texInfoB >> 6) & 0x7F;
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
     
      polygons[lp].tex = texbuf->getTexture(&texEntry);
       	  
      polygons[lp].textured = true;
      polyTexCount++;
    }
    else
    {
      polygons[lp].textured = false;
      polyNonCount++;
    }
  }
  
  bool sorted;
  for (int pass=0; pass<polyCount; pass++)
  {
    sorted = true; //assumed sorted
    
    for (int index=0; index<polyCount-1; index++)
    {
      polygon polygonA = polygons[index];
      polygon polygonB = polygons[index+1];
      if (polygonB.textured == true)
      {
        polygons[index]   = polygonB;
        polygons[index+1] = polygonA;
        sorted = false;            
      }      
    }

    if (sorted == true) { break; }
  }
}

#endif

//draw the model at its location (determined from entry data)
void model_wld::drawAtLocation()
{
  drawAt(location);
}

