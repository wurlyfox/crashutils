#include "section_visual.h"

void context_plugin_section_visual::onInit()
{
  parent = (context_plugin_editor*)context_plugin::parent;
}

void context_plugin_section_visual::messageRouter(int msg, param lparam, param rparam)
{
  switch (msg)
  {
    case CPM_ADD_ZONE:
    {
      entry *zone = (entry*)lparam;
      unsigned char *zoneHeader = zone->itemData[0];
      
      int headerColCount = getWord(zoneHeader, 0x204, true);
      int sectionCount   = getWord(zoneHeader, 0x208, true);
      
      for (int sectionIndex = 0; sectionIndex < sectionCount; sectionIndex++)
      { 
        // initialize and add the new path model
        model_path sectionPath;
        sectionPath.path   = geom->getMisc();
        sectionPath.trail  = sectionPath.path->newLines(200);
        sectionPath.arrows = sectionPath.path->newTriangles(200);
        for (int lp = 0; lp < 200; lp++)
        {
          sectionPath.trail[lp]  = { 0, 0 };
          sectionPath.arrows[lp] = { 0, 0, 0 };
        }
        pathCache->get(zone, sectionIndex, sectionPath); 
        
        // grab the corresponding section to build the model
        unsigned char *section = zone->itemData[headerColCount+sectionIndex];  
        unsigned short pathNodeCount = getHword(section, 0xA, true);
        
        buildPath(zone, section, 0, pathNodeCount);
      }
    } 
    break;
    case CPM_SELECT_SECTION:
    {
      //...resolve section index by checking referenced zone 
      //edit: we just use currentZone for now
      entry *currentZone             = parent->parent->currentZone;
      entry *previousZone            = parent->parent->previousZone;
      unsigned char *section         = (unsigned char*)lparam;
      unsigned char *previousSection = parent->parent->previousSection;
        
      //
      int sectionIndex = 2;
      int prevSectionIndex = 2;
      for (; prevSectionIndex<previousZone->itemCount; prevSectionIndex++)
        if (previousSection == previousZone->itemData[prevSectionIndex]) { break; }
      for (; sectionIndex<currentZone->itemCount; sectionIndex++)
        if (currentSection == currentZone->itemData[sectionIndex]) { break; }
        
      //...use cache to look up corresponding geometric data    
      model_msc *prevPath = pathCache->get(previousZone, prevSectionIndex);
      model_msc *curPath  = pathCache->get( currentZone, sectionIndex);    
      prevPath->color = { 0.0, 0.0, 0.0 };  // deselect prev path back to white
      curPath->color  = { 1.0, 0.0, 0.0 };  // select new path with red
    }
    break;
    case CPM_UPDATE_SECTION_PATH_POINT:
    {
      entry *currentZone          = parent->parent->currentZone;
      signed char *currentSection = parent->parent->currentSection;
    
      unsigned short node = (unsigned short)rparam;
      
      buildPath(currentZone, currentSection, node, node);
    }
    break;
    case CPM_UPDATE_SECTION_PATH:
    {
      entry *currentZone          = parent->parent->currentZone;
      signed char *currentSection = parent->parent->currentSection;
    
      unsigned short pathNodeCount = getHword(currentSection, 0xA, true);
      
      buildPath(currentZone, currentSection, 0, pathNodeCount);
    }
    break;
  }
  
  context_plugin::messageRouter(msg, lparam, rparam);
}

void context_plugin_section_visual::buildPath(entry *zone, unsigned char *section, unsigned short min, unsigned short max)
{
  int sectionIndex = 2;
  for (; sectionIndex<zone->itemCount; sectionIndex++)
    if (section == zone->itemData[sectionIndex]) { break; }
  
  model_path sectionPath = pathCache->get(zone, sectionIndex);
  
  signed char *zoneCollisions = zone->itemData[1];
    
  signed long zoneX = getWord(zoneCollisions, 0, true);
  signed long zoneY = getWord(zoneCollisions, 4, true);
  signed long zoneZ = getWord(zoneCollisions, 8, true);
  
  int pathNodeCount = getHword(currentSection, 0xA, true);
  
  point P1 = { getHword(section, 0x32, true),
               getHword(section, 0x34, true),
               getHword(section, 0x36, true) };
               
  float X1 = ((zoneX+P1.X)/(float)0x1000)*8;
  float Y1 = ((zoneY+P1.Y)/(float)0x1000)*8;
  float Z1 = ((zoneZ+P1.Z)/(float)0x1000)*8;
    
  int lp;
  for (lp = 0; lp < pathNodeCount-1; lp++)
  {
    point P2 = { getHword(section, 0x32+((lp+1)*12),
                 getHword(section, 0x34+((lp+1)*12),
                 getHword(section, 0x36+((lp+1)*12) };
                               
    float X2 = ((zoneX+P2.X)/(float)0x1000)*8;
    float Y2 = ((zoneY+P2.Y)/(float)0x1000)*8;
    float Z2 = ((zoneZ+P2.Z)/(float)0x1000)*8;
  
    sectionPath.trail[lp] = { { X1, Y1, Z1 } , { X2, Y2, Z2 } };

    fvector diff = { X2-X1, Y2-Y1, Z2-Z1 };
      
    float normal = sqrt(diff.X*diff.X + diff.Y*diff.Y + diff.Z*diff.Z);
    
    /*float mat44[16] = { 1, 0, 0, 0,
                          0, 1, 0, 0,
                          0, 0, 1, 0,
                          0, 0, 0, 1 };*/
                                             
    if (normal != 0)
    {
      fvector ndiff = { diff.X/normal, diff.Y/normal, diff.Z/normal };
      
      fvector rot = { ndiff.Y, -ndiff.X, 0 };
      
      float ac = acos( diff.Z/normal );
      float s = sin(ac);
      float c = cos(ac);
      float t = 1 - c;
      
      float x = rot.X;
      float y = rot.Y;
      float z = rot.Z;
   
      /*
      mat[0] = t*x*x + c;
      mat[1] = t*x*y - s*z;
      mat[2] = t*x*z + s*y;
   
      mat[4] = t*x*y + s*z;
      mat[5] = t*y*y + c;
      mat[6] = t*y*z - s*x;
   
      mat[8] = t*x*z - s*y;
      mat[9] = t*y*z + s*x;
      mat[10] = t*z*z + c;
      */
      
      fmatrix mat;
      mat.V1 = { t*x*x +   c, t*x*y + s*z, t*x*z - s*y };
      mat.V2 = { t*x*y - s*z, t*y*y +   c, t*y*z + s*x };
      mat.V3 = { t*x*z + s*y, t*y*z - s*x, t*z*z +   c };
      
      int arrowSize = 40;
      float arrowScale = ((float)arrowSize/0x1000)*scale;
      
      fpoint arrowPtA = { X1 +        0.0, Y1 + 0.0, Z1 + arrowScale };
      fpoint arrowPtB = { X1 + arrowScale, Y1 + 0.0, Z1 +        0.0 };
      fpoint arrowPtC = { X1 - arrowScale, Y1 + 0.0, Z1 +        0.0 };
      
      transform(mat, arrowPtA);
      transform(mat, arrowPtB);
      transform(mat, arrowPtC);
 
      sectionPath.arrows[lp].A = arrowPtA;
      sectionPath.arrows[lp].B = arrowPtB;
      sectionPath.arrows[lp].C = arrowPtC;  

      P1 = P2;        
    }
  }
}