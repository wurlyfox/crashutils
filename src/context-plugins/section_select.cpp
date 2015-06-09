#include "section_select.h"

void context_plugin_section_select::onInit()
{
  parent = (context_plugin_select*)context_plugin::parent;
  
  /* SELECTIONS HERE */
}

void context_plugin_section_select::onRender()
{
  NSD *nsd; NSF *nsf; 
  contextScene->getNSDNSF(nsd, nsf);
  
  for (int zoneIndex=0; zoneIndex<nsf->entryCount[7]; zoneIndex++)
  {
    entry *zone = nsf->entries[7][zoneIndex];
    unsigned char *zoneHeader = zone->itemData[0];
    
    int headerColCount = getWord(zoneHeader, 0x204, true);
    int sectionCount = getWord(zoneHeader, 0x208, true);
    
    for (int sectIndex=0; sectIndex<sectionCount; sectIndex++)
    {
      int sectItemIndex = headerColCount + sectIndex;
      unsigned char *zoneCollisions = zone->itemData[1];
      
      signed long zoneX = getWord(zoneCollisions, 0, true);
      signed long zoneY = getWord(zoneCollisions, 4, true);
      signed long zoneZ = getWord(zoneCollisions, 8, true);
      
      unsigned char *zoneSection = zone->itemData[sectItemIndex];
      unsigned short pathCount = getHword(zoneSection, 0xA, true);
    
      for (int pathIndex=0; pathIndex<pathCount; pathIndex++)
      {
        signed short nodeX = getHword(zoneSection, 0x32+(pathIndex*12), true);
        signed short nodeY = getHword(zoneSection, 0x34+(pathIndex*12), true);
        signed short nodeZ = getHword(zoneSection, 0x36+(pathIndex*12), true);
        
        dpoint pathPoint = { zoneX + nodeX,
                             zoneY + nodeY,
                             zoneZ + nodeZ };
  
        int sectPointIndex = (sectIndex << 12) | (pathIndex);
        
        parent->recordSelectable(pathPoint, 4, zoneIndex, sectPointIndex);
      }
    }
  }
}

void context_plugin_section_select::onPoint(int x, int y)
{
  if (parent->closestSelectable)
  {
    if (parent->closestSelectable->type == 4)
    {
      NSD *nsd; NSF *nsf; 
      contextScene->getNSDNSF(nsd, nsf);
      
      selectable *selected = parent->closestSelectable;
       
      entry *selectedZone          = nsf->entries[7][selected->item_index];
      unsigned char *zoneHeader    = selectedZone->itemData[0];
      unsigned long headerColCount = getWord(zoneHeader, 0x204, true);
      
      int selSectionIndex            = (selected->vert_index >> 12) & 0xF;
      unsigned char *selectedSection = selectedZone->itemData[headerColCount + selSectionIndex];
      
      //if (selectedZone != currentZone)
      postMessage(CPM_SELECT_ZONE, (param)selectedZone);
        
      postMessage(CPM_SELECT_SECTION, (param)selectedSection);
    }
  }
} 