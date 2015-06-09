#include "editor.h"

#define moveBack 400

entryCache<zone*> context_plugin_editor::zoneCache = entryCache<zone*>();
zone *context_plugin_editor::viewing = 0;

entry *context_plugin_editor::currentZone = 0;
unsigned char *context_plugin_editor::currentSection = 0;
unsigned char *context_plugin_editor::currentEntity = 0;
entry *context_plugin_editor::currentWorld = 0;

void context_plugin_editor::messageRouter(int msg, param lparam, param rparam)
{ 
  switch (msg)
  {
    case CPM_RESET:           onReset();                                  break;
    
    case CPM_LOAD_LEVEL:      onLoadLevel();                              break;
    case CPM_LOAD_ZONE:       onLoadZone((entry*)lparam);  
    case CPM_LOAD_WGEO:       onLoadModel((entry*)lparam);                break;
    case CPM_LOAD_SVTX:       onLoadModel((entry*)lparam, (int)rparam);   break;

    case CPM_ADD_ZONE:        if (!onAddZone((entry*)lparam)) { return; } break;
  
    case CPM_VISUAL_ZONE_TOGGLE_OCTREES: contextScene->toggleViewFlags(SCENE_VIEWFLAGS_ZONEVIEWCOLLISIONS); break;
    case CPM_VISUAL_SECTION_TOGGLE: contextScene->toggleViewFlags(SCENE_VIEWFLAGS_ZONEVIEWSECTIONS); break;
    case CPM_VISUAL_WIREFRAME_TOGGLE: contextScene->toggleViewFlags(SCENE_VIEWFLAGS_VIEWWIREFRAME); break;

    case CPM_SELECT_ZONE:     onSelectZone((entry*)lparam);               break;
    case CPM_SELECT_SECTION:  onSelectSection((unsigned char*)lparam);    break;  
  }
  
  context_plugin::messageRouter(msg, lparam, rparam);
}

void context_plugin_editor::onInit()
{
  postMessage(CPM_RESET);
}

void context_plugin_editor::onReset()
{
  currentZone    = 0;
  currentSection = 0;
  currentEntity  = 0;
  currentWorld   = 0;
  
  previousZone    = 0;
  previousSection = 0;
  previousEntity  = 0;
  previousWorld   = 0;
  
  if (!(contextScene->getSceneFlags() & SCENE_SCENEFLAGS_SHAREDGEOM))
  {
    contextScene->reset(); 
    zoneCache.reset();
  }
}

void context_plugin_editor::onLoadLevel()
{
  reset();
  
  NSF *nsf; NSD *nsd;
  contextScene->getNSDNSF(nsd, nsf);
  
  int zoneCount = nsf->entryCount[7];
  for (int lp = 0; lp < zoneCount; lp++)
    addZone(nsf->entries[7][lp]);
    
  if (zoneCount > 0)  
    selectZone(nsf->entries[7][0]); 
}

void context_plugin_editor::onLoadZone(entry *zone)
{
  reset();
  selectZone(zone);
}
  
void context_plugin_editor::onLoadModel(entry *tgeo, int frame)
{
  reset(); 
  contextScene->viewSingleObject(tgeo, frame);
}

void context_plugin_editor::onLoadModel(entry *wgeo)
{
  reset();
  contextScene->viewSingleWorld(wgeo);
  //selectWorld(wgeo)
}

bool context_plugin_editor::onAddZone(entry *zdat)
{
  if (viewing)
    viewing->deselect();
    
  zone **theZone;
  if (!zoneCache.get(zdat, theZone))
  {
    *theZone = contextScene->addZone(zdat);
    (*theZone)->select();
    viewing = *theZone;
    return true;
  }
  
  (*theZone)->select();
  viewing = *theZone;
  return false;
}

void context_plugin_editor::onSelectZone(entry *zone)
{
  currentZone = zone;
  
  if (zone)
  {
    addZone(zone);
    contextScene->setViewMode(SCENE_VIEWMODE_VIEWZONESALL);
    
    int sections      = getWord(zone->itemData[0], 0x208, true);
    int entitys       = getWord(zone->itemData[0], 0x20C, true);  
    if (sections != 0)
    {
      unsigned char *item1 = zone->itemData[0];
      int sectionOffset    = getWord(item1, 0x204, true);
      currentSection       = zone->itemData[sectionOffset+0];
      
      selectSection(currentSection);
    }
    else
    {
      unsigned char *zoneCollisions = currentZone->itemData[1];
    
      point zoneLocation;
      zoneLocation.X = getWord(zoneCollisions, 0, true);
      zoneLocation.Y = getWord(zoneCollisions, 4, true);
      zoneLocation.Z = getWord(zoneCollisions, 8, true) + moveBack;
      
      postMessage(CPM_CAMERA_SMOOTH_TRANS, (param)&zoneLocation);
    }
  }
}

void context_plugin_editor::onSelectSection(unsigned char *section)
{
  currentSection = section; 
  
  if (section)
  {
    unsigned char *zoneCollisions = currentZone->itemData[1];
    
    point zoneLocation;
    zoneLocation.X = getWord(zoneCollisions, 0, true);
    zoneLocation.Y = getWord(zoneCollisions, 4, true);
    zoneLocation.Z = getWord(zoneCollisions, 8, true);
    
    point sectionStart;
    sectionStart.X = zoneLocation.X + (signed short)getHword(section, 0x32, true);
    sectionStart.Y = zoneLocation.Y + (signed short)getHword(section, 0x34, true);
    sectionStart.Z = zoneLocation.Z + (signed short)getHword(section, 0x36, true);
    
    point viewLocation = { sectionStart.X,
                           sectionStart.Y,
                           sectionStart.Z + moveBack };
                     
    postMessage(CPM_CAMERA_SMOOTH_TRANS, (param)&viewLocation);
  }
}

void context_plugin_editor::onHandleKeys(bool *keys)
{  
  if (keys['Q'])
    postMessage(CPM_LOAD_LEVEL);
}