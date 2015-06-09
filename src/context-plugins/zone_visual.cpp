#include "zone_visual.h"

class context_plugin_zone_visual : public context_plugin
{ 
  protected:
  
  context_plugin_editor *parent;
  
  static model_msc *neighborSelections;
  static int selectionCount;
  static fquad *selectionFaces;
  static fline *selectionLines;
  
  public:
  
  void init();
  void onRender();
}

model_msc *context_plugin_zone_visual::neighborSelections = 0;
int        context_plugin_zone_visual::selectionCount     = 0;
fquad     *context_plugin_zone_visual::selectionFaces     = 0; 
fline     *context_plugin_zone_visual::selectionLines     = 0;

void context_plugin_zone_visual::onInit()
{
  parent = (context_plugin_editor*)context_plugin::parent;
  
  if (!neighborSelections)
  {
    neighborSelections = geom->getMisc();
    neighborSelections->color = { 0.0, 0.0, 1.0 };
  }
 
  if (!selectionFaces)
    selectionFaces = neighborSelections->newQuads(4*8);
    
  if (!selectionLines)
    selectionLines = neighborSelections->newLines(2*8);
    
  // handle all children's init
  context_plugin::init();
}

void context_plugin_zone_visual::onRender()
{
  // outline all of the selected zone's neighbors blue
  if (parent->currentZone)
  {
    selectionCount = 0;
    
    NSD *nsd; NSF *nsf; 
    contextScene->getNSDNSF(nsd, nsf);
    
    unsigned char *zoneHeader = parent->currentZone->itemData[0];
    int neighborZoneCount = getWord(zoneHeader, 0x210, true);
    
    for (int lp=0; lp < neighborZoneCount; lp++)
    { 
      unsigned long neighborEID = getWord(zoneHeader, 0x214+(4*lp), true);
      unsigned long parentCID   = nsd->lookupCID(neighborEID);
      
      if (parentCID == -1)
      {
        entry *neighborZone = lookupEntry(neigborEID, parentCID);
        unsigned char *zoneCollision = neigborZone->itemData[1];
                                
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
        
        int faceIndex = lp*2;
        int lineIndex = lp*4;
        
        selectionFaces[faceIndex+0] = { vert[0], vert[1], vert[3], vert[2] };
        selectionFaces[faceIndex+1] = { vert[4], vert[5], vert[7], vert[6] };
        selectionLines[lineIndex+0] = { vert[0], vert[4] };
        selectionLines[lineIndex+1] = { vert[1], vert[5] };
        selectionLines[lineIndex+2] = { vert[2], vert[6] };
        selectionLines[lineIndex+3] = { vert[3], vert[7] };
      
        selectionCount++;
      }
    }
    
    for (int lp=neighborZoneCount; lp<8; lp++)
    {
      int faceIndex = lp*2;
      int lineIndex = lp*4;
      
      selectionFaces[faceIndex+0] = { 0, 0, 0, 0 };
      selectionFaces[faceIndex+1] = { 0, 0, 0, 0 };
      selectionLines[lineIndex+0] = { 0, 0 };
      selectionLines[lineIndex+1] = { 0, 0 };      
      selectionLines[lineIndex+2] = { 0, 0 };
      selectionLines[lineIndex+3] = { 0, 0 }; 
    }      
  }
}