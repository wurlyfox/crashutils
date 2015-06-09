#ifndef ZONE_H
#define ZONE_H

#include "../crash/entry.h"
#include "types.h"
#include "geometry.h"

#define ZONE_VIEWMODE_VIEWBOUNDS     0x1
#define ZONE_VIEWMODE_VIEWSECTIONS   0x2
#define ZONE_VIEWMODE_VIEWCOLLISIONS 0x4
#define ZONE_VIEWMODE_VIEWWORLDS     0x8
#define ZONE_VIEWMODE_VIEWOBJECTS    0x10

class collisionNode
{ 
  public:
  
  unsigned short data;
  unsigned char level;
  
  bool render;
  
  fregion bound;
  
  void load(entry *zdat, unsigned short node, unsigned char depth, volume division);
  virtual void draw() {};
};

class section
{
  friend class zone;
  
  protected:
  
  int ID;
  unsigned short pathLength;
  vector cameraPath[0x100];
  vector start;
  
  bool render;
  
  public:
  
  void load(entry *zdat, int sec);
  virtual void drawFrom(point loc, int scale) {};

  vector getStart() { return start; }
};

class zone
{
  friend class scene;
  
  protected:
  
  int worldCount;
  int objectCount;
  int sectionCount;
  int colCount;
  
  int polyCount;
  int vertCount;
 
  vector location;
  vector dimension;
  region bounds;
  
  section *sections[MAX_SECTIONS];
  collisionNode *collisions[MAX_COLLISIONS];
  model_wld *worlds[MAX_WGEO];
  model_obj *objects[MAX_TGEO];  
  
  bool render;
  
  void loadModels(entry *zdat, geometry *geom);
  void loadSections(entry *zdat);
  void loadCollisions(entry *zdat);
  void loadCollisionNode(entry *zdat, unsigned short node, volume division, int level);
    
  public:
  
  zone();
  virtual ~zone() {};
  
  void load(entry *zdat, geometry *geom);
  void unload();
  
  section *allocateSection() { return sections[sectionCount++]; }
  collisionNode *allocateCollision() { return collisions[colCount++]; }
  
  void addWorld(entry *wgeo, point loc, geometry *geom);
  void addObject(entry *svtx, int itemIndex, point loc, geometry *geom);
    
  virtual void draw(unsigned long flags) {};
 
  virtual void select();
  virtual void deselect(); 
  
  void selectSection(int section);
};

#endif
