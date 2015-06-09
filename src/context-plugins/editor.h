#ifndef CONTEXT_PLUGIN_EDITOR_H
#define CONTEXT_PLUGIN_EDITOR_H

#include "graphics/context_plugin.h"

#include "crash/entry.h"
#include "crash/entry_cache.h"

#include "../message.h"

#include "camera.h"
//#include "visual.h"
#include "select.h"

#include "camera_control.h"
#include "camera_sync.h"
#include "camera_trans.h"

#include "polygon_select.h"
#include "zone_select.h"

class context_plugin_editor : public context_plugin
{
  friend class context_plugin_select;
 
  protected:
  
  static entryCache<zone*> zoneCache;
  static zone *viewing;
  
  // each refers to the respective crash structure of its type;
  // these are the current primary foci of the editor. 
  static entry *currentZone;
  static unsigned char *currentSection;
  static unsigned char *currentEntity;
  static entry *currentWorld;
  
  // so the editor can remember its previous state
  entry *previousZone;
  unsigned char *previousSection;
  unsigned char *previousEntity;
  entry *previousWorld;
  
  // these refer to the same structures listed above as members
  // these objects handle and keep track of any changes made to the corresponding data
  /*
  crash_modify *currentZoneHeaderM;
  crash_modify *currentZoneCollisionsM;
  crash_modify *currentSectionM;
  crash_modify *currentEntityM;
  */
      
  void messageRouter(int msg, param lparam, param rparam);
  
  void onInit();
  void onReset();
  void onLoadLevel();
  void onLoadZone(entry *zone);
  void onLoadModel(entry *wgeo);
  void onLoadModel(entry *svtx, int frame);
  bool onAddZone(entry *zone);
  void onSelectZone(entry *zone);
  void onSelectSection(unsigned char *section);

  void onHandleKeys(bool *keys);
  
  public:
  
  context_plugin_editor(context *ctxt) : context_plugin(ctxt)
  {
    context_plugin *camera = new context_plugin_camera(ctxt);
    context_plugin *select = new context_plugin_select(ctxt);

    addChild(camera);
    addChild(select);
    
    context_plugin *cameraControl = new context_plugin_camera_control(ctxt);
    context_plugin *cameraSync = new context_plugin_camera_sync(ctxt);
    context_plugin *cameraTrans = new context_plugin_camera_trans(ctxt);

    camera->addChild(cameraControl);
    camera->addChild(cameraSync);
    camera->addChild(cameraTrans);
    
    context_plugin *polygonSelect = new context_plugin_polygon_select(ctxt);
    context_plugin *zoneSelect = new context_plugin_zone_select(ctxt);
    
    select->addChild(polygonSelect);
    select->addChild(zoneSelect);
  }
  
  void reset()                               { postMessage(CPM_RESET); }
  void loadLevel()                           { postMessage(CPM_LOAD_LEVEL); }
  void loadModel(entry *wgeo)                { postMessage(CPM_LOAD_WGEO, (param)wgeo); }
  void loadModel(entry *svtx, int frame)     { postMessage(CPM_LOAD_SVTX, (param)svtx, (param)frame); }
  void addZone(entry *zone)                  { postMessage(CPM_ADD_ZONE, (param)zone); }
  void selectZone(entry *zone)               { postMessage(CPM_SELECT_ZONE, (param)zone); }
  void selectSection(unsigned char *section) { postMessage(CPM_SELECT_SECTION, (param)section); }
};

#endif