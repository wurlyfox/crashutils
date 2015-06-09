#include "zone.h"

zone::zone()
{
  for (int lp = 0; lp < MAX_SECTIONS; lp++)
    sections[lp] = 0;
    
  for (int lp = 0; lp < MAX_COLLISIONS; lp++)
    collisions[lp] = 0;
    
  unload();
}

void zone::load(entry *zone, geometry *geom)
{
  unload();
  loadModels(zone, geom);
  loadSections(zone);
  loadCollisions(zone);
}

void zone::unload()
{  
  polyCount = 0;
  vertCount = 0;

  worldCount   = 0;
  objectCount  = 0;
  sectionCount = 0;
  colCount     = 0;
  
  bounds.P1 = { 0, 0, 0 };
  bounds.P2 = { 0, 0, 0 };
  
  location  = { 0, 0, 0 };
  dimension = { 0, 0, 0 };
  
  render = false;
}

void zone::loadModels(entry *zdat, geometry *geom)
{  
  unsigned char *item1 = zdat->itemData[0];
  worldCount         = getWord(item1, 0, true);
  objectCount        = getWord(item1, 0x20C, true);
  
  unsigned long item2Count = getWord(item1, 0x204, true);
  unsigned long item3Count = getWord(item1, 0x208, true);
  unsigned long entityOffset = item2Count + item3Count;
  
  unsigned char *item2 = zdat->itemData[1];
  
  signed long zoneX = getWord(item2, 0, true);
  signed long zoneY = getWord(item2, 4, true);
  signed long zoneZ = getWord(item2, 8, true);
  signed long zoneW = getWord(item2, 0xC, true);
  signed long zoneH = getWord(item2, 0x10, true);
  signed long zoneD = getWord(item2, 0x14, true);
  
  location.X = zoneX;
  location.Y = zoneY;
  location.Z = zoneZ;
  
  dimension.X = zoneW;
  dimension.Y = zoneH;
  dimension.Z = zoneD;
  
  bounds.P1.X = location.X;
  bounds.P1.Y = location.Y;
  bounds.P1.Z = location.Z;
  bounds.P2.X = location.X+dimension.X;
  bounds.P2.Y = location.Y+dimension.Y;
  bounds.P2.Z = location.Z+dimension.Z;
  
  int remove = 0;
  for (int count = 0; count < worldCount; count++)
  {
    unsigned long wgeoEID   = getWord(item1, 4+((count+remove)*0x40), true);
    unsigned long parentCID = geom->nsd->lookupCID(wgeoEID);
    
    if (parentCID == -1)
    {
      worldCount--;
      count--;
      remove++;
    }
    else
    {
      chunk *parentChunk      = geom->nsf->lookupChunk(parentCID);
      entry *wgeo             = lookupEntry(wgeoEID, parentChunk);
            
      //worlds[count].load(wgeo, loc);
      //vertCount += worlds[count].vertCount;
      //polyCount += worlds[count].polyCount;
      worlds[count] = geom->getWorld(wgeo);
      vertCount += worlds[count]->vertCount;
      polyCount += worlds[count]->polyCount;
    }
  }  
  
  /*
  for (int count=0; count < objectCount; count++)
  {
    unsigned char *entityItem = zdat->itemData[entityOffset + count]; 
    unsigned char globalCode  = entityItem[0x12];
    
    signed short modelX = getHword(entityItem, 0x14, true);
    signed short modelY = getHword(entityItem, 0x16, true);
    signed short modelZ = getHword(entityItem, 0x18, true);
    
    unsigned long codeEID   = nsd->levelEIDs[globalCode];
    unsigned long parentCID = nsd->lookupCID(codeEID);
    chunk *parentChunk      = nsf->lookupChunk(parentCID);
    entry *code             = lookupEntry(codeEID, parentChunk);
    
    unsigned char *anims = code->itemData[5];
    
    if (anims && anims[0] == 1)
    { 
      unsigned long initSvtxEID = getWord(anims, 4, true);
      parentCID       = nsd->lookupCID(initSvtxEID);
      parentChunk     = nsf->lookupChunk(parentCID);
      entry *initSvtx = lookupEntry(initSvtxEID, parentChunk);
    
      point loc;
      loc.X = (modelX*4) + location.X;
      loc.Y = (modelY*4) + location.Y;
      loc.Z = (modelZ*4) + location.Z;
      
      objects[count].load(initSvtx, 0, loc);
      vertCount += objects[count].vertCount;
      polyCount += objects[count].polyCount;
      
      objects[count].render = true;
    }
    else
      objects[count].render = false;
  }*/
 
}

void zone::loadSections(entry *zone)
{
  unsigned char *item1 = zone->itemData[0];
  sectionCount = getWord(item1, 0x208, true);
  
  if (sectionCount > 0)
  {
    for (int lp = 0; lp < sectionCount; lp++)
    {
      sections[lp]->load(zone, lp);
      sections[lp]->render = true;
    }
  }
}

void zone::loadCollisions(entry *zdat)
{
  unsigned char *item2 = zdat->itemData[1];
  
  unsigned long zoneW = getWord(item2, 0xC, true);
  unsigned long zoneH = getWord(item2, 0x10, true);
  unsigned long zoneD = getWord(item2, 0x14, true);
  
  unsigned short rootNode = getHword(item2, 0x1C, true);

  volume initDivision = { { 0, 0, 0 } , { zoneW, zoneH, zoneD } };
  
  colCount = 0;
  loadCollisionNode(zdat, rootNode, initDivision, 0);
}

void zone::loadCollisionNode(entry *zdat, unsigned short node, volume division, int level)
{
  
  if (node & 1)
  {
    if (colCount < MAX_COLLISIONS)
    {
      collisionNode *newCollision = allocateCollision();
      newCollision->load(zdat, node, level, division);
    }
  }
  else if (node != 0)
  {
    unsigned char *treeItem = zdat->itemData[1];
    unsigned short maxNodeW = getHword(treeItem, 0x1E, true);
    unsigned short maxNodeH = getHword(treeItem, 0x20, true);
    unsigned short maxNodeD = getHword(treeItem, 0x22, true);
    
    unsigned char *nodeChildren = treeItem + node;
    
    volume subdivision = { { division.loc.X, division.loc.Y, division.loc.Z }, 
                           { division.dim.W, division.dim.H, division.dim.D } };  

    //has left, right, up, down, front, back
    bool hasLR, hasUD, hasFB;
    hasLR = level < maxNodeW;
    hasUD = level < maxNodeH;
    hasFB = level < maxNodeD;
    
    unsigned char flags = (hasLR << 0) | (hasUD << 1) | (hasFB << 2);
     
    //front, up, left
    //front, up, right
    //front, down, left
    //front, down, right
    //back, up, left,
    //back, up, right
    //back, down, left
    //back, down, right
    
    unsigned short children[8];
    
    switch (flags)
    {
      case 1:
      case 2:
      case 4:
      children[0] = getHword(nodeChildren, 0, true);
      children[1] = getHword(nodeChildren, 2, true);
      
      if (flags == 1)
      {
        subdivision.dim.W /= 2;
        loadCollisionNode(zdat, children[0], subdivision, level+1);
        subdivision.loc.X += subdivision.dim.W;
        loadCollisionNode(zdat, children[1], subdivision, level+1);
      }
      else if (flags == 2)
      {
        subdivision.dim.H /= 2;
        loadCollisionNode(zdat, children[0], subdivision, level+1);
        subdivision.loc.Y += subdivision.dim.H;
        loadCollisionNode(zdat, children[1], subdivision, level+1);
      }
      else
      {
        subdivision.dim.D /= 2;
        loadCollisionNode(zdat, children[0], subdivision, level+1);
        subdivision.loc.Z += subdivision.dim.D;
        loadCollisionNode(zdat, children[1], subdivision, level+1);
      }
      
      break;
      
      case 3:
      case 5:
      case 6:
      children[0] = getHword(nodeChildren, 0, true);
      children[1] = getHword(nodeChildren, 2, true);
      children[2] = getHword(nodeChildren, 4, true);
      children[3] = getHword(nodeChildren, 6, true);
      
      if (flags == 3)
      {
        subdivision.dim.W /= 2;
        subdivision.dim.H /= 2;
        
        loadCollisionNode(zdat, children[0], subdivision, level+1);
        
        subdivision.loc.Y += subdivision.dim.H;
        loadCollisionNode(zdat, children[1], subdivision, level+1);
        subdivision.loc.Y -= subdivision.dim.H;
        
        subdivision.loc.X += subdivision.dim.W;
        loadCollisionNode(zdat, children[2], subdivision, level+1);
        subdivision.loc.X -= subdivision.dim.W;
        
        subdivision.loc.X += subdivision.dim.W;
        subdivision.loc.Y += subdivision.dim.H;
        loadCollisionNode(zdat, children[3], subdivision, level+1);
        subdivision.loc.X -= subdivision.dim.W;
        subdivision.loc.Y -= subdivision.dim.H;
      }
      else if (flags == 5)
      {
        subdivision.dim.W /= 2;
        subdivision.dim.D /= 2;
        
        
        loadCollisionNode(zdat, children[0], subdivision, level+1);
        
        subdivision.loc.Z += subdivision.dim.D;
        loadCollisionNode(zdat, children[1], subdivision, level+1);
        subdivision.loc.Z -= subdivision.dim.D;
        
        subdivision.loc.X += subdivision.dim.W;
        loadCollisionNode(zdat, children[2], subdivision, level+1);
        subdivision.loc.X -= subdivision.dim.W;
        
        subdivision.loc.X += subdivision.dim.W;
        subdivision.loc.Z += subdivision.dim.D;
        loadCollisionNode(zdat, children[3], subdivision, level+1);
        subdivision.loc.X -= subdivision.dim.W;
        subdivision.loc.Z -= subdivision.dim.D;
        
      }
      else
      {
        subdivision.dim.H /= 2;
        subdivision.dim.D /= 2;
        
        loadCollisionNode(zdat, children[0], subdivision, level+1);
        
        subdivision.loc.Z += subdivision.dim.D;
        loadCollisionNode(zdat, children[1], subdivision, level+1);
        subdivision.loc.Z -= subdivision.dim.D;
        
        subdivision.loc.Y += subdivision.dim.H;
        loadCollisionNode(zdat, children[2], subdivision, level+1);
        subdivision.loc.Y -= subdivision.dim.H;
        
        subdivision.loc.Y += subdivision.dim.H;
        subdivision.loc.Z += subdivision.dim.D;
        loadCollisionNode(zdat, children[3], subdivision, level+1);
        subdivision.loc.Y -= subdivision.dim.H;
        subdivision.loc.Z -= subdivision.dim.D;
      }
      break;
      
      case 7:
      children[0] = getHword(nodeChildren, 0, true);
      children[1] = getHword(nodeChildren, 2, true);
      children[2] = getHword(nodeChildren, 4, true);
      children[3] = getHword(nodeChildren, 6, true);
      children[4] = getHword(nodeChildren, 8, true);
      children[5] = getHword(nodeChildren, 0xA, true);
      children[6] = getHword(nodeChildren, 0xC, true);
      children[7] = getHword(nodeChildren, 0xE, true);
      
      subdivision.dim.W /= 2;
      subdivision.dim.H /= 2;
      subdivision.dim.D /= 2;
      
      loadCollisionNode(zdat, children[0], subdivision, level+1);
      
      subdivision.loc.Z += subdivision.dim.D;
      loadCollisionNode(zdat, children[1], subdivision, level+1);
      subdivision.loc.Z -= subdivision.dim.D;
      
      subdivision.loc.Y += subdivision.dim.H;
      loadCollisionNode(zdat, children[2], subdivision, level+1);
      subdivision.loc.Y -= subdivision.dim.H;
      
      subdivision.loc.Y += subdivision.dim.H;
      subdivision.loc.Z += subdivision.dim.D;
      loadCollisionNode(zdat, children[3], subdivision, level+1);
      subdivision.loc.Y -= subdivision.dim.H;
      subdivision.loc.Z -= subdivision.dim.D;
      
      subdivision.loc.X += subdivision.dim.W;
      loadCollisionNode(zdat, children[4], subdivision, level+1);
      subdivision.loc.X -= subdivision.dim.W;
      
      subdivision.loc.X += subdivision.dim.W;
      subdivision.loc.Z += subdivision.dim.D;
      loadCollisionNode(zdat, children[5], subdivision, level+1);
      subdivision.loc.X -= subdivision.dim.W;
      subdivision.loc.Z -= subdivision.dim.D;
      
      subdivision.loc.X += subdivision.dim.W;
      subdivision.loc.Y += subdivision.dim.H;
      loadCollisionNode(zdat, children[6], subdivision, level+1);
      subdivision.loc.X -= subdivision.dim.W;
      subdivision.loc.Y -= subdivision.dim.H;
      
      subdivision.loc.X += subdivision.dim.W;
      subdivision.loc.Y += subdivision.dim.H;
      subdivision.loc.Z += subdivision.dim.D;
      loadCollisionNode(zdat, children[7], subdivision, level+1);
      subdivision.loc.X -= subdivision.dim.W;
      subdivision.loc.Y -= subdivision.dim.H;
      subdivision.loc.Z -= subdivision.dim.D;
      
      break;
    }
  }
}
            
void zone::addWorld(entry *wgeo, point loc, geometry *geom)
{
  worlds[worldCount] = geom->getWorld(wgeo);
  
  worlds[worldCount]->location.X = loc.X;
  worlds[worldCount]->location.Y = loc.Y;
  worlds[worldCount]->location.Z = loc.Z;
  
  vertCount += worlds[worldCount]->vertCount;
  polyCount += worlds[worldCount]->polyCount;
  worldCount++;
}

void zone::addObject(entry *svtx, int itemIndex, point loc, geometry *geom)
{
  objects[objectCount]->load(svtx, itemIndex, loc, geom->textureBuffer, geom->primAlloc);  
  vertCount += objects[objectCount]->vertCount;
  polyCount += objects[objectCount]->polyCount;
  objectCount++;
}

void zone::select()
{
  render = true;
}

void zone::deselect()
{
  render = false;
}

void zone::selectSection(int section)
{
  if (section < sectionCount)
  {
    for (int lp=0; lp < sectionCount; lp++)
      sections[lp]->render = false;
    
    sections[section]->render = true;
  }
}

void section::load(entry *zone, int sec)
{
  ID = sec;
  
  unsigned char *item1 = zone->itemData[0];
  int item2Count       = getWord(item1, 0x204, true);
  
  unsigned char *sectionItem = zone->itemData[item2Count+sec];
  
  //unsigned char pathMin = sectionItem[0x1C];
  //unsigned char pathMax = sectionItem[0x1D];
  
  pathLength = getHword(sectionItem, 0x1E, true);
  
  //unsigned short camMode = getHword(sectionItem, 0x20, true);
  //unsigned short pathScale = getHword(sectionItem, 0x22, true);
 
  //unsigned short v1 = getHword(sectionItem, 0x24, true);
  //unsigned short v2 = getHword(sectionItem, 0x26, true);
  //unsigned short v3 = getHword(sectionItem, 0x28, true);
  //unsigned short v4 = getHword(sectionItem, 0x2A, true);
  
  //unsigned short pathX = getHword(sectionItem, 0x2C, true);
  //unsigned short pathY = getHword(sectionItem, 0x2E, true);
  //unsigned short pathZ = getHword(sectionItem, 0x30, true);
  
  for (int lp = 0; lp < pathLength; lp++)
  {
    cameraPath[lp].X = (signed short)getHword(sectionItem, 0x32 + (lp*12), true);
    cameraPath[lp].Y = (signed short)getHword(sectionItem, 0x34 + (lp*12), true);
    cameraPath[lp].Z = (signed short)getHword(sectionItem, 0x36 + (lp*12), true);
  }
  
  if (pathLength > 0)
  {
    unsigned char *item2 = zone->itemData[1];
    start.X = getWord(item2, 0, true) + cameraPath[0].X;
    start.Y = getWord(item2, 4, true) + cameraPath[0].Y;
    start.Z = getWord(item2, 8, true) + cameraPath[0].Z;
  }
}

void collisionNode::load(entry *zdat, unsigned short node, unsigned char depth, volume division)
{
  data  =  node;
  level = depth;
                
  bound.P1.X = ((float)division.loc.X * 8) / 0x1000;
  bound.P1.Y = ((float)division.loc.Y * 8) / 0x1000;
  bound.P1.Z = ((float)division.loc.Z * 8) / 0x1000;
  
  bound.P2.X = (((float)division.loc.X + division.dim.W) * 8) / 0x1000;
  bound.P2.Y = (((float)division.loc.Y + division.dim.H) * 8) / 0x1000;
  bound.P2.Z = (((float)division.loc.Z + division.dim.D) * 8) / 0x1000;
 
  render = true;  
} 