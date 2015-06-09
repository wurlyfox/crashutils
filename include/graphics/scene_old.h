#ifndef SCENE_H
#define SCENE_H

#include "geometry.h"
#include "camera.h"
#include "zone.h"
#include "model.h"

#define SCENE_VIEWMODE_VIEWWGEO             0x1
#define SCENE_VIEWMODE_WGEOSINGLE           0x2
#define SCENE_VIEWMODE_VIEWSVTX             0x4
#define SCENE_VIEWMODE_SVTXSINGLE           0x8
#define SCENE_VIEWMODE_VIEWSPRITE          0x10
#define SCENE_VIEWMODE_SPRITESINGLE        0x20
#define SCENE_VIEWMODE_VIEWZONE            0x40
#define SCENE_VIEWMODE_ZONESINGLE          0x80
#define SCENE_VIEWMODE_VIEWMODELS         0x100
                                    
#define SCENE_VIEWMODE_VIEWWGEOSINGLE \
        (SCENE_VIEWMODE_VIEWWGEO   | \
         SCENE_VIEWMODE_WGEOSINGLE)
        
#define SCENE_VIEWMODE_VIEWSVTXSINGLE \
        (SCENE_VIEWMODE_VIEWSVTX | \
        SCENE_VIEWMODE_SVTXSINGLE)
        
#define SCENE_VIEWMODE_VIEWZONESCUMULATIVE \
        (SCENE_VIEWMODE_VIEWZONE   | \
         SCENE_VIEWMODE_ZONESINGLE | \
         SCENE_VIEWMODE_VIEWWGEO   | \
         SCENE_VIEWMODE_VIEWMODELS)
         
#define SCENE_VIEWMODE_VIEWZONESALL \
        (SCENE_VIEWMODE_VIEWZONE | \
         SCENE_VIEWMODE_VIEWWGEO | \
         SCENE_VIEWMODE_VIEWMODELS)
         

#define SCENE_VIEWFLAGS_ZONEVIEWBOUNDS     0x1
#define SCENE_VIEWFLAGS_ZONEVIEWSECTIONS   0x2
#define SCENE_VIEWFLAGS_ZONEVIEWCOLLISIONS 0x4
#define SCENE_VIEWFLAGS_ZONEVIEWWORLDS     0x8
#define SCENE_VIEWFLAGS_ZONEVIEWOBJECTS    0x10
#define SCENE_VIEWFLAGS_VIEWWIREFRAME      0x20

#define SCENE_VIEWFLAGS_ALL \
        (SCENE_VIEWFLAGS_ZONEVIEWBOUNDS     | \
         SCENE_VIEWFLAGS_ZONEVIEWSECTIONS   | \
         SCENE_VIEWFLAGS_ZONEVIEWCOLLISIONS | \
         SCENE_VIEWFLAGS_VIEWWIREFRAME)

class scene
{
  friend class context_wgl;
  
  protected:

	geometry *geom;
  NSD *nsd;
	NSF *nsf;
  
	camera *cam;
  
  long zoneCount;
  zone *zoneBuffer[MAX_ZONES];
  long modelCount;
  model *modelBuffer[MAX_MODELS];   //-instances- of [object] models
  long colorCount;
  fcolor colorBuffer[MAX_VERTICES*MAX_MODELS]; //buffer for their colors as computed by matrices

  bool inited;
  bool loaded;
  unsigned long viewMode;
  unsigned long viewFlags;
  int viewZoneIndex;
  
  model *allocateModel() { return modelBuffer[modelCount++]; }
  zone  *allocateZone()  { return   zoneBuffer[zoneCount++]; }
  
  public:

  scene(NSD *_nsd, NSF *_nsf);  
  virtual ~scene() {};
  
  void reset();

  camera *getCamera()
  {
    return cam;
  }
  
  void addModel(entry *svtx, int frame, 
                vector *trans, angle *rot, vector *scale, 
                slightmatrix *light, scolormatrix *color, scolor *back, scolor *backIntensity);
  void addModel(unsigned long texEID, unsigned char *texInfoArray, unsigned long scale, vector *trans);
  void clearModels();
  
  void viewSingleWorld(entry *wgeo);
  void viewSingleObject(entry *svtx, int frame);
  void startAtZone(entry *zone);
  zone *addZone(entry *zone);
  zone *loadZone(entry *zone, int index);
  void viewToZone(int index);
  void viewToSection(int section);
  void viewToSection(int index, int section);

  void setViewFlags(unsigned long flags);
  void clearViewFlags(unsigned long flags);
  void toggleViewFlags(unsigned long flags);  
  void setViewMode(unsigned long mode);
  
  virtual void init() { inited = true; }
  virtual void draw();
  virtual bool isInited() { return inited; };
  virtual bool isReady() { return loaded; }
};
  
#endif