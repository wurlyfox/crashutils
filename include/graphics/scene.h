#ifndef SCENE_H
#define SCENE_H

#include "geometry.h"
#include "camera.h"
#include "zone.h"
#include "model.h"

//#include "../../crash/queue.h"

#define SCENE_SCENEFLAGS_SHAREDGEOM         0x1
#define SCENE_SCENEFLAGS_SHAREDCAM          0x2
#define SCENE_SCENEFLAGS_SHAREDVIEW         0x4
#define SCENE_SCENEFLAGS_SHAREDZONES        0x8
#define SCENE_SCENEFLAGS_SHAREDMODELS       0x10
#define SCENE_SCENEFLAGS_SHAREDALLBUTCAM \
        SCENE_SCENEFLAGS_SHAREDGEOM | \
        SCENE_SCENEFLAGS_SHAREDVIEW | \
        SCENE_SCENEFLAGS_SHAREDZONES | \
        SCENE_SCENEFLAGS_SHAREDMODELS
        
#define SCENE_VIEWMODE_VIEWWGEO             0x1
#define SCENE_VIEWMODE_WGEOSINGLE           0x2
#define SCENE_VIEWMODE_VIEWSVTX             0x4
#define SCENE_VIEWMODE_SVTXSINGLE           0x8
#define SCENE_VIEWMODE_VIEWSPRITE          0x10
#define SCENE_VIEWMODE_SPRITESINGLE        0x20
#define SCENE_VIEWMODE_VIEWZONE            0x40
#define SCENE_VIEWMODE_ZONESINGLE          0x80
#define SCENE_VIEWMODE_VIEWMODELS         0x100
#define SCENE_VIEWMODE_VIEWMISC           0x200

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
         SCENE_VIEWMODE_VIEWMODELS | \
         SCENE_VIEWMODE_VIEWMISC)
         
#define SCENE_VIEWMODE_VIEWZONESALL \
        (SCENE_VIEWMODE_VIEWZONE | \
         SCENE_VIEWMODE_VIEWWGEO | \
         SCENE_VIEWMODE_VIEWMODELS | \
         SCENE_VIEWMODE_VIEWMISC)
         

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

struct modelRequest
{
  entry *svtx;
  int frame;
  vector trans;
  angle rot;
  vector scale;
  slightmatrix light;
  scolormatrix color;
  scolor back;
  scolor backIntensity;
};

//template class queue<modelRequest>;

class scene
{
  friend class context_plugin;
  protected:

  NSD *nsd;
	NSF *nsf;
	camera *cam;
	geometry *geom;
  
  int *zoneCount;
  zone **zoneBuffer; //*zoneBuffer[MAX_ZONES];
  
  int *modelCount;
  model **modelBuffer; //*modelBuffer[MAX_MODELS];   //-instances- of [object] models
  //queue<modelRequest> *modelRequestQueue;

  long colorCount;
  fcolor colorBuffer[MAX_VERTICES*MAX_MODELS]; //buffer for their colors as computed by matrices

  unsigned long sceneFlags;
  
  bool *loaded;
  bool *inited;
  unsigned long *viewMode;
  unsigned long *viewFlags;
  int viewZoneIndex;
  
  model *allocateModel() { return modelBuffer[(*modelCount)++]; }
  zone  *allocateZone()  { return   zoneBuffer[(*zoneCount)++]; }

  public:

  scene(NSD *_nsd, NSF *_nsf);  
  scene(scene *shared, unsigned long flags);
  virtual ~scene();
  
  void getNSDNSF(NSD *&_nsd, NSF *&_nsf) { _nsd = nsd; _nsf = nsf; }
  
  void reset();

  camera *setCamera(camera *newCam) { camera *oldCam = cam; cam = newCam; return oldCam; }
  camera *getCamera()               { return cam; }
  
  geometry *getGeometry()           { return geom; }
  
  void requestModel(entry *svtx, int frame, 
                    vector* trans, angle* rot, vector* scale, 
                    slightmatrix* light, scolormatrix* color, scolor* back, scolor* backIntensity);
  void addModel(entry *svtx, int frame, 
                vector *trans, angle *rot, vector *scale, 
                slightmatrix *light, scolormatrix *color, scolor *back, scolor *backIntensity);
  void addModel(unsigned long texEID, unsigned char *texInfoArray, unsigned long scale, vector *trans);
  void addModel(unsigned long texEID, int quadCount, squad2 *quadInfo, 
                unsigned char *texInfoArray, unsigned long scale, vector *trans);

  void clearModels();
  
  void viewSingleWorld(entry *wgeo);
  void viewSingleObject(entry *svtx, int frame);
  void startAtZone(entry *zone);
  zone *addZone(entry *zone);
  zone *loadZone(entry *zone, int index);
  void viewToZone(int index);
  void viewToSection(int section);
  void viewToSection(int index, int section);

  unsigned long getSceneFlags();
  void setSceneFlags(unsigned long flags);
  void clearSceneFlags(unsigned long flags);
  void toggleSceneFlags(unsigned long flags);
  
  unsigned long getViewFlags();
  void setViewFlags(unsigned long flags);
  void clearViewFlags(unsigned long flags);
  void toggleViewFlags(unsigned long flags);
  
  unsigned long getViewMode();
  void setViewMode(unsigned long mode);
  
  virtual void init() { *inited = true; }
  virtual void draw();
  virtual bool isInited() { return *inited; };
  virtual bool isReady() { return *loaded; }
  
};
  
#endif