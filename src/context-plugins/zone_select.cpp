#include "zone_select.h"

#include "comp_geom.h"

int context_plugin_zone_select::selectionCount = 1;
fquad *context_plugin_zone_select::selectionFaces = 0; 
fline *context_plugin_zone_select::selectionLines = 0;
  
void context_plugin_zone_select::onInit()
{
  parent = (context_plugin_select*)context_plugin::parent;

  zoneOutlineCount = 0;
  
  if (!selectionFaces)
    selectionFaces = parent->selections->newQuads(geom->primAlloc, 2);
  
  if (!selectionLines)
    selectionLines = parent->selections->newLines(geom->primAlloc, 4);
  //selectionCount = 1;
}

void context_plugin_zone_select::onRender()
{ 
  if (contextScene->getViewMode() & (SCENE_VIEWMODE_WGEOSINGLE
                                    | SCENE_VIEWMODE_SVTXSINGLE
                                    | SCENE_VIEWMODE_SPRITESINGLE))
    return;
    
  zoneOutlineCount = 0;
  
  NSD *nsd; NSF *nsf; 
  contextScene->getNSDNSF(nsd, nsf);
  
  for (int lp=0; lp<nsf->entryCount[7]; lp++)
  {
    entry *zone = nsf->entries[7][lp];
    unsigned char *zoneCollision = zone->itemData[1];
                            
    signed long zoneX   = getWord(zoneCollision, 0, true);
    signed long zoneY   = getWord(zoneCollision, 4, true);
    signed long zoneZ   = getWord(zoneCollision, 8, true);
    unsigned long zoneW = getWord(zoneCollision, 0xC, true);
    unsigned long zoneH = getWord(zoneCollision, 0x10, true);
    unsigned long zoneD = getWord(zoneCollision, 0x14, true);
  
    fvolume zoneRect;

    zoneRect.loc.X = ((float)zoneX / 0x1000) * 8;
    zoneRect.loc.Y = ((float)zoneY / 0x1000) * 8;
    zoneRect.loc.Z = ((float)zoneZ / 0x1000) * 8;
    zoneRect.dim.W = ((float)zoneW / 0x1000) * 8;
    zoneRect.dim.H = ((float)zoneH / 0x1000) * 8;
    zoneRect.dim.D = ((float)zoneD / 0x1000) * 8;
  
    dpoint zoneRect3d[8];
    comp_geom::getVolumeVertices3d(&zoneRect, zoneRect3d);
    
    dpoint zoneOutline2d[8];
    int outlinePointCount;
    comp_geom::projectOutline2d(parentContext, 
                                   zoneRect3d, 8, 
                                zoneOutline2d, outlinePointCount);
                                                    
    // create an entry for this zone's outline
    zone_outline *newOutline = &zoneOutline[zoneOutlineCount++];
      
    newOutline->pointCount = outlinePointCount;
    for (int lp=0; lp<outlinePointCount; lp++)
      newOutline->outline[lp] = zoneOutline2d[lp];
    
    newOutline->zone = zone;
  } 
}

void context_plugin_zone_select::onHover(int x, int y)
{    
  if (contextScene->getViewMode() & (SCENE_VIEWMODE_WGEOSINGLE
                                    | SCENE_VIEWMODE_SVTXSINGLE
                                    | SCENE_VIEWMODE_SPRITESINGLE))
    return;
    
  bool zoneSel = false;
  
  zone_outline *curOutline;
  for (int lp=0; lp<zoneOutlineCount; lp++)
  {
    curOutline = &zoneOutline[lp];
        
    zoneSel = comp_geom::pointInPolygon2d(x, y, curOutline->outline, curOutline->pointCount);
    if (zoneSel)       
      break;
  }
  
  if (zoneSel)
  {
    entry *zone = curOutline->zone;
    unsigned char *zoneCollision = zone->itemData[1];
                            
    signed long zoneX   = getWord(zoneCollision, 0, true);
    signed long zoneY   = getWord(zoneCollision, 4, true);
    signed long zoneZ   = getWord(zoneCollision, 8, true);
    unsigned long zoneW = getWord(zoneCollision, 0xC, true);
    unsigned long zoneH = getWord(zoneCollision, 0x10, true);
    unsigned long zoneD = getWord(zoneCollision, 0x14, true);
  
    fvolume zoneRect3d;
    zoneRect3d.loc.X = ((float)zoneX / 0x1000) * 8;
    zoneRect3d.loc.Y = ((float)zoneY / 0x1000) * 8;
    zoneRect3d.loc.Z = ((float)zoneZ / 0x1000) * 8;
    zoneRect3d.dim.W = ((float)zoneW / 0x1000) * 8;
    zoneRect3d.dim.H = ((float)zoneH / 0x1000) * 8;
    zoneRect3d.dim.D = ((float)zoneD / 0x1000) * 8;
    
    fpoint vert[8];
    comp_geom::getVolumeVertices3d(&zoneRect3d, vert);
    
    selectionFaces[0] = { vert[0], vert[1], vert[3], vert[2] };
    selectionFaces[1] = { vert[4], vert[5], vert[7], vert[6] };
    selectionLines[0] = { vert[0], vert[4] };
    selectionLines[1] = { vert[1], vert[5] };
    selectionLines[2] = { vert[2], vert[6] };
    selectionLines[3] = { vert[3], vert[7] };
  }  
}


//HANDLED BY context_plugin_section_select
void context_plugin_zone_select::onPoint(int x, int y)
{
  if (contextScene->getViewMode() & (SCENE_VIEWMODE_WGEOSINGLE
                                    | SCENE_VIEWMODE_SVTXSINGLE
                                    | SCENE_VIEWMODE_SPRITESINGLE))
    return;
    
  bool zoneSel = false;
  
  zone_outline *curOutline;
  for (int lp=0; lp<zoneOutlineCount; lp++)
  {
    curOutline = &zoneOutline[lp];
        
    zoneSel = comp_geom::pointInPolygon2d(x, y, curOutline->outline, curOutline->pointCount);
    if (zoneSel)       
      break;
  }
  
  if (zoneSel)
  {
    entry *zone = curOutline->zone;
    postMessage(MSG_SELECT_ZONE, (param)zone);
  }
}