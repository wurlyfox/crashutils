#include "model_obj.h"

void model_obj::load(entry *svtx, int frame, point loc, texture_buffer *texbuf, prim_alloc *prims)
{
  load(svtx, frame, texbuf, prims);
  
  positionMode = 0;
  location.X = loc.X;
  location.Y = loc.Y;
  location.Z = loc.Z;
}

void model_obj::load(entry *svtx, int frame, texture_buffer *texbuf, prim_alloc *prims)
{
  positionMode = 1;
  location.X = 0;
  location.Y = 0;
  location.Z = 0;
  
  vertCount = 0;
  polyCount = 0;
  
  if (svtx->type != 1) { return; }
  
  if (svtx->itemData[frame] == 0 || svtx->itemSize[frame] == 0) { return; }
  
  unsigned char *buffer = svtx->itemData[frame];
  
  vertCount               =        getWord(buffer, 0, true);
  unsigned long tgeoEID   =        getWord(buffer, 4, true);
  unsigned long parentCID =         nsd->lookupCID(tgeoEID);
  chunk *parentChunk      =     nsf->lookupChunk(parentCID);
  entry *tgeoEntry        = lookupEntry(tgeoEID, parentChunk);
				  
  loadVertices(svtx, frame, prims);
  loadPolygons(tgeoEntry, texbuf, prims);
  render = true;
}
  
void model_obj::loadVertices(entry *svtx, int frame, prim_alloc *prims)
{
  unsigned char *buffer = svtx->itemData[frame];
  
  //load the vertices/normals for this frame
  vertCount = getWord(buffer, 0, true);

  signed long offsetX = getWord(buffer, 0x8, true);
  signed long offsetY = getWord(buffer, 0xC, true);
  signed long offsetZ = getWord(buffer, 0x10, true);
  
  //**the game subtracts a default of 128 from the model coordinate offset  
  //(scaling here is also done to shift the sign to the appropriate position)
  signed short offsetXP = (offsetX - 128) * SCALE_OBJECT;
  signed short offsetYP = (offsetY - 128) * SCALE_OBJECT;
  signed short offsetZP = (offsetZ - 128) * SCALE_OBJECT;
 
  vertices  = prims->allocVertices(vertCount);
  normals   = prims->allocNormals(vertCount);
 
  #define SVTX_VERTICES 0x38
  
  for (int lp = 0; lp < vertCount; lp++)
  {
    unsigned long bufferOffset = lp * 6;
	
    //first grab AAZZYYXXCCBB values from binary model data
    unsigned long  CNXZYX  =  getWord(buffer, SVTX_VERTICES + bufferOffset, true);
    unsigned short CNZCNY  = getHword(buffer, SVTX_VERTICES + bufferOffset+4, true);
	
    //then separate appropriately, performing any additional scaling
    //like the game does before sending to the GTE
    unsigned short vertX = ((CNXZYX & 0x000000FF) >>  0) * SCALE_OBJECT;
    unsigned short vertY = ((CNXZYX & 0x0000FF00) >>  8) * SCALE_OBJECT;
    unsigned short vertZ = ((CNXZYX & 0x00FF0000) >> 16) * SCALE_OBJECT;
    
    signed char CNX = (CNXZYX & 0xFF000000) >> 24;
    signed char CNY = (CNZCNY & 0x00FF);
    signed char CNZ = (CNZCNY & 0xFF00) >> 8;

    //now calculate total vertex position
    signed short X = offsetXP + vertX;
    signed short Y = offsetYP + vertY;
    signed short Z = offsetZP + vertZ;
	
    //psx GTE reads these values as fixed point, gl as floats
    //do the appropriate conversions before storing so gl can interpret correctly
    vertices[lp].X = ((float)X * 8) / 0x1000;
    vertices[lp].Y = ((float)Y * 8) / 0x1000;
    vertices[lp].Z = ((float)Z * 8) / 0x1000;
   
    //pre-emptively sets the object to be drawn white if texture information is not found
    vertices[lp].Rf = 1.0;
    vertices[lp].Gf = 1.0;
    vertices[lp].Bf = 1.0;
	
    normals[lp].X = (float)CNX / (16);
    normals[lp].Y = (float)CNY / (16);
    normals[lp].Z = (float)CNZ / (16);
  }  
  
  //load the bound box for this frame
  signed long boundX1 = getWord(buffer, 0x14, true);
  signed long boundY1 = getWord(buffer, 0x18, true);
  signed long boundZ1 = getWord(buffer, 0x1C, true);
  
  signed long boundX2 = getWord(buffer, 0x20, true);
  signed long boundY2 = getWord(buffer, 0x24, true);
  signed long boundZ2 = getWord(buffer, 0x28, true);
  
  bound.P1.X = ((float)boundX1 * 8) / 0x1000;
  bound.P1.Y = ((float)boundY1 * 8) / 0x1000;
  bound.P1.Z = ((float)boundZ1 * 8) / 0x1000;
  
  bound.P2.X = ((float)boundX2 * 8) / 0x1000;
  bound.P2.Y = ((float)boundY2 * 8) / 0x1000;
  bound.P2.Z = ((float)boundZ2 * 8) / 0x1000;
}

#ifdef GL_VERTEX_ARRAY_ENABLE
void model_obj::loadPolygons(entry *tgeo, texture_buffer *texbuf, prim_alloc *prims)
{
  if (tgeo->itemData[0] == 0 || tgeo->itemSize[0] == 0) { return; }
  if (tgeo->itemData[1] == 0 || tgeo->itemSize[1] == 0) { return; }
  
  unsigned char *buffer       = tgeo->itemData[0];
  unsigned char *texInfoArray = tgeo->itemData[0] + 0x14;
  
  //or -are- these actually scale values?
  //where are these used in the assembly?
  unsigned long scaleX = getWord(buffer, 0x4, true);
  unsigned long scaleY = getWord(buffer, 0x8, true);
  unsigned long scaleZ = getWord(buffer, 0xC, true);
    
  polyCount    = getWord(buffer, 0, true);
  polyTexCount = 0;

  vertex *vertex_list = vertices;
  fvector *normal_list = normals;
  prims->preparepPolygons(vertices, normals, texcoords);
  
  vertex  *pVertex;
  fvector *pNormal;
  fpoint2 *pTexcoord;
    
  buffer = tgeo->itemData[1];
  for (int lp = 0; lp < polyCount; lp++)
  {
    unsigned long bufferOffset = lp * 8;

    unsigned short flagTex       = getHword(buffer, bufferOffset+6, true);
    unsigned short texInfoOffset = (flagTex & 0x7FFF);

    unsigned long texInfoA  = getWord(texInfoArray, texInfoOffset*4, true);
    bool textured           = (texInfoA >> 31) & 1;
   
    if (textured)
    {          
      prims->allocpPolygon(pVertex, pNormal, pTexcoord);  

      unsigned short aOffset = getHword(buffer, bufferOffset+0, true);
      unsigned short bOffset = getHword(buffer, bufferOffset+2, true);
      unsigned short cOffset = getHword(buffer, bufferOffset+4, true);

      pVertex[0] = vertex_list[(aOffset/6)];
      pVertex[1] = vertex_list[(bOffset/6)];
      pVertex[2] = vertex_list[(cOffset/6)];
      pNormal[0] = normal_list[(aOffset/6)];
      pNormal[1] = normal_list[(bOffset/6)];
      pNormal[2] = normal_list[(cOffset/6)];
      
      unsigned char modelFlag      = (flagTex & 0x8000) >> 15;
      unsigned short texInfoOffset = (flagTex & 0x7FFF);
    
      unsigned char semiTrans = (texInfoA >> 29) & 3;
           
      unsigned long texEID   = getWord(texInfoArray, (texInfoOffset*4)+4, true); 
      unsigned long texInfoB = getWord(texInfoArray, (texInfoOffset*4)+8, true);
	  
      unsigned short texCoordsOffset = ((texInfoB >> 22) & 0x3FF);
      unsigned short colorMode       = ((texInfoB >> 20) & 3);
      unsigned short section128      = ((texInfoB >> 18) & 3);
    
      //unsigned short offsetX = ((texInfoB >> 10) & 0xF8) >> colorMode;  //multiple of 4 or else, varies for cmode
      //unsigned short offsetY = (texInfoB & 0x1F) << 2; 
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
     
      texture *tex = texbuf->getTexture(&texEntry);
      
      pTexcoord[0] = tex->coords.A;
      pTexcoord[1] = tex->coords.B;
      pTexcoord[2] = tex->coords.C;

      polyTexCount++;
    }
  }  
    
  polyNonCount = 0;
  
  for (int lp = 0; lp < polyCount; lp++)
  {
    unsigned long bufferOffset = lp * 8;
    
    unsigned short flagTex       = getHword(buffer, bufferOffset+6, true);
    unsigned short texInfoOffset = (flagTex & 0x7FFF);
    
    unsigned long texInfoA = getWord(texInfoArray, texInfoOffset*4, true);
    bool textured          = (texInfoA >> 31) & 1;
   
    if (!textured)
    {          
      prims->allocpPolygon(pVertex, pNormal); 

      unsigned short aOffset = getHword(buffer, bufferOffset+0, true);
      unsigned short bOffset = getHword(buffer, bufferOffset+2, true);
      unsigned short cOffset = getHword(buffer, bufferOffset+4, true);
 
      pVertex[0] = vertex_list[(aOffset/6)];
      pVertex[1] = vertex_list[(bOffset/6)];
      pVertex[2] = vertex_list[(cOffset/6)];
      pNormal[0] = normal_list[(aOffset/6)];
      pNormal[1] = normal_list[(bOffset/6)];
      pNormal[2] = normal_list[(cOffset/6)];

      unsigned char R = (texInfoA >>  0) & 0xFF;
      unsigned char G = (texInfoA >>  8) & 0xFF;
      unsigned char B = (texInfoA >> 16) & 0xFF;
      
      pVertex[0].Rf = (float)R/256;
      pVertex[0].Gf = (float)G/256;
      pVertex[0].Bf = (float)B/256;
      pVertex[1].Rf = (float)R/256;
      pVertex[1].Gf = (float)G/256;
      pVertex[1].Bf = (float)B/256;
      pVertex[2].Rf = (float)R/256;
      pVertex[2].Gf = (float)G/256;
      pVertex[2].Bf = (float)B/256;      

      polyNonCount++;
    }
  }  
}

#else
    
void model_obj::loadPolygons(entry *tgeo, texture_buffer *texbuf, prim_alloc *prims)
{
  if (tgeo->itemData[0] == 0 || tgeo->itemSize[0] == 0) { return; }
  if (tgeo->itemData[1] == 0 || tgeo->itemSize[1] == 0) { return; }
  
  unsigned char *buffer       = tgeo->itemData[0];
  unsigned char *texInfoArray = tgeo->itemData[0] + 0x14;
  
  //or -are- these actually scale values?
  //where are these used in the assembly?
  unsigned long scaleX = getWord(buffer, 0x4, true);
  unsigned long scaleY = getWord(buffer, 0x8, true);
  unsigned long scaleZ = getWord(buffer, 0xC, true);
    
  polyCount = getWord(buffer, 0, true);
  polygons  = prims->allocPolygons(polyCount);  
   
  polyTexCount = 0;
  polyNonCount = 0;
    
  buffer = tgeo->itemData[1];
  for (int lp = 0; lp < polyCount; lp++)
  {
    unsigned long bufferOffset    = lp * 8;
    
    unsigned short aOffset = getHword(buffer, bufferOffset+0, true);
    unsigned short bOffset = getHword(buffer, bufferOffset+2, true);
    unsigned short cOffset = getHword(buffer, bufferOffset+4, true);
    unsigned short flagTex = getHword(buffer, bufferOffset+6, true);
	  
    polygons[lp].vertexA = &vertices[(aOffset/6)];
    polygons[lp].vertexB = &vertices[(bOffset/6)];
    polygons[lp].vertexC = &vertices[(cOffset/6)];
    polygons[lp].normalA = &normals[(aOffset/6)];
    polygons[lp].normalB = &normals[(bOffset/6)];
    polygons[lp].normalC = &normals[(cOffset/6)];
    
    unsigned char modelFlag      = (flagTex & 0x8000) >> 15;
    unsigned short texInfoOffset = (flagTex & 0x7FFF);
	
    unsigned long texInfoA  = getWord(texInfoArray, texInfoOffset*4, true);

    bool textured           = (texInfoA >> 31) & 1;
    unsigned char semiTrans = (texInfoA >> 29) & 3;
    
    unsigned char R         = (texInfoA >>  0) & 0xFF;
    unsigned char G         = (texInfoA >>  8) & 0xFF;
    unsigned char B         = (texInfoA >> 16) & 0xFF;
   
    polygons[lp].color.R = (float)R/256;
    polygons[lp].color.G = (float)G/256;
    polygons[lp].color.B = (float)B/256;
    
    if (textured)
    {
      unsigned long texEID   = getWord(texInfoArray, (texInfoOffset*4)+4, true); 
      unsigned long texInfoB = getWord(texInfoArray, (texInfoOffset*4)+8, true);
	  
      unsigned short texCoordsOffset = ((texInfoB >> 22) & 0x3FF);
      unsigned short colorMode       = ((texInfoB >> 20) & 3);
      unsigned short section128      = ((texInfoB >> 18) & 3);
    
      //unsigned short offsetX = ((texInfoB >> 10) & 0xF8) >> colorMode;  //multiple of 4 or else, varies for cmode
      //unsigned short offsetY = (texInfoB & 0x1F) << 2; 
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
      fvector  normalA = normals[index];
      fvector  normalB = normals[index+1];
      if (polygonB.textured == true)
      {
        polygons[index]   = polygonB;
        polygons[index+1] = polygonA;
        normals[index]    = normalA;
        normals[index+1]  = normalB;
        sorted = false;            
      }      
    }

    if (sorted == true) { break; }
  }
}

#endif

//draw the model at its location (specified when loading)
void model_obj::drawAtLocation()
{  
  drawAt(location);
}