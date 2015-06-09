#include "scene.h"

scene::scene(NSD *_nsd, NSF *_nsf)
{
  nsd = _nsd;
  nsf = _nsf;
  
	sceneFlags = 0;
	
  zoneBuffer = new zone*[MAX_ZONES];
  modelBuffer = new model*[MAX_MODELS];

  for (int lp = 0; lp < MAX_ZONES; lp++)
    zoneBuffer[lp]  = 0;
  for (int lp = 0; lp < MAX_MODELS; lp++)
    modelBuffer[lp] = 0;
     
  zoneCount = new int;
  modelCount = new int;
  *zoneCount = 0;
  *modelCount = 0;
  
  viewMode = new unsigned long;
  viewFlags = new unsigned long;
  *viewMode = 0;
  *viewFlags = 0;
  
  loaded = new bool;
  inited = new bool;
  *loaded = false;
  *inited = false;
  
  //modelRequestQueue = new queue<modelRequest>;
  //modelRequestQueue->allocate(10);

  cam  = 0;
  geom = 0;
  
  colorCount = 0;
}
  
scene::scene(scene *shared, unsigned long flags)
{
  nsd = shared->nsd;
  nsf = shared->nsf;
    
  sceneFlags = flags;

  if (flags & SCENE_SCENEFLAGS_SHAREDGEOM)
  {
    inited = shared->inited;
    loaded = shared->loaded;
    geom   = shared->geom;
  }
  else
  {
    inited = new bool;
    loaded = new bool;
    geom = 0;
  }
    
  if (flags & SCENE_SCENEFLAGS_SHAREDCAM)
    cam = shared->cam;
  else
    cam = 0;
    
  if (flags & SCENE_SCENEFLAGS_SHAREDVIEW)
  {
    viewMode  = shared->viewMode;
    viewFlags = shared->viewFlags;
  }
  else
  {
    viewMode = new unsigned long;
    viewFlags = new unsigned long;
  }
  
  if (flags & SCENE_SCENEFLAGS_SHAREDZONES)
  {
    zoneCount = shared->zoneCount;
    zoneBuffer = shared->zoneBuffer;
  }
  else
  {
    zoneCount = new int;
    zoneBuffer = new zone*[MAX_ZONES];
  }
  
  if (flags & SCENE_SCENEFLAGS_SHAREDMODELS)
  {
    modelCount = shared->modelCount;
    modelBuffer = shared->modelBuffer;
    
    //modelRequestQueue = 0;
  }
  else
  {
    modelCount = new int;
    modelBuffer = new model*[MAX_MODELS];
    
    //modelRequestQueue = new queue<modelRequest>;
    //modelRequestQueue->allocate(10);
  }
}

scene::~scene()
{
  delete[] zoneBuffer;
  delete[] modelBuffer;
  
  delete zoneCount;
  delete modelCount;
  delete viewMode;
  delete viewFlags;
  delete loaded;
  delete inited;
  delete cam;
  delete geom;
  
  //delete modelRequestQueue;
}
  
void scene::clearModels()
{
  *modelCount = 0;   
}

void scene::reset()
{ 
  *loaded = false;
  
  viewZoneIndex = 0;
    
  for (int lp = 0; lp < *zoneCount; lp++) 
    zoneBuffer[lp]->unload();
  
  *zoneCount = 0;
  
  if (!((*viewMode & SCENE_VIEWMODE_VIEWZONESCUMULATIVE) == SCENE_VIEWMODE_VIEWZONESCUMULATIVE))
  {
    clearModels();

    geom->reset();
    cam->reset();
  }
}

/*
void scene::requestModel(entry *svtx, int frame, vector* trans, angle* rot, vector* scale, slightmatrix* light, scolormatrix* color, scolor* back, scolor* backIntensity)
{
  modelRequest request;
  request.svtx = svtx;
  request.frame = frame;
  request.trans = *trans;
  request.rot = *rot;
  request.scale = *scale;
  request.light = *light;
  request.color = *color;
  request.back = *back;
  request.backIntensity = *backIntensity;
  
  modelRequestQueue->add(request);
}
*/

void scene::addModel(entry *svtx, int frame, vector *trans, angle *rot, vector *scale, slightmatrix *light, scolormatrix *color, scolor *back, scolor *backIntensity)
{
  //model *mod = allocateModel();
  model *mod = modelBuffer[*modelCount];
  
  unsigned char *svtxFrame = svtx->itemData[frame];
  
  unsigned long tgeoEID    = getWord(svtxFrame, 4, true);
  unsigned long parentCID  = nsd->lookupCID(tgeoEID);
  chunk *parentChunk       = nsf->lookupChunk(parentCID);
  entry *tgeo              = lookupEntry(tgeoEID, parentChunk);
  
  unsigned char *modelHeader = tgeo->itemData[0];
  signed long modelXscale = getWord(modelHeader,   4, true);
  signed long modelYscale = getWord(modelHeader,   8, true);
  signed long modelZscale = getWord(modelHeader, 0xC, true);
    
  mod->type           = 1;
  mod->object_model   = geom->getObject(svtx, frame);
  mod->object_model->positionMode = 1;
  mod->trans.X        = ((float)(trans->X >> 8) / 0x1000) * 8;  
  mod->trans.Y        = ((float)(trans->Y >> 8) / 0x1000) * 8;
  mod->trans.Z        = ((float)(trans->Z >> 8) / 0x1000) * 8;
  mod->rot.X          = ((((float)rot->X) / 0x1000) * 360);
  mod->rot.Y          = ((((float)rot->Y) / 0x1000) * 360);
  mod->rot.Z          = ((((float)rot->Z) / 0x1000) * 360);
  mod->scale.X        = ((float)scale->X / 0x1000) * ((float)modelXscale / 0x1000);
  mod->scale.Y        = ((float)scale->Y / 0x1000) * ((float)modelYscale / 0x1000);
  mod->scale.Z        = ((float)scale->Z / 0x1000) * ((float)modelZscale / 0x1000);
  
  mod->lightMatrix.V1.X = (float)light->V1.X / 0x1000;
  mod->lightMatrix.V2.X = (float)light->V2.X / 0x1000;
  mod->lightMatrix.V3.X = (float)light->V3.X / 0x1000;
  mod->lightMatrix.V1.Y = (float)light->V1.Y / 0x1000;
  mod->lightMatrix.V2.Y = (float)light->V2.Y / 0x1000;
  mod->lightMatrix.V3.Y = (float)light->V3.Y / 0x1000;
  mod->lightMatrix.V1.Z = (float)light->V1.Z / 0x1000;
  mod->lightMatrix.V2.Z = (float)light->V2.Z / 0x1000;
  mod->lightMatrix.V3.Z = (float)light->V3.Z / 0x1000;
  
  mod->colorMatrix.C1.R = (float)color->C1.R / 0x1000;
  mod->colorMatrix.C2.R = (float)color->C2.R / 0x1000;
  mod->colorMatrix.C3.R = (float)color->C3.R / 0x1000;
  mod->colorMatrix.C1.G = (float)color->C1.G / 0x1000;
  mod->colorMatrix.C2.G = (float)color->C2.G / 0x1000;
  mod->colorMatrix.C3.G = (float)color->C3.G / 0x1000;
  mod->colorMatrix.C1.B = (float)color->C1.B / 0x1000;
  mod->colorMatrix.C2.B = (float)color->C2.B / 0x1000;
  mod->colorMatrix.C3.B = (float)color->C3.B / 0x1000;
  
  mod->backColor.R = (((float)back->R) * ((float)backIntensity->R / 256)) / 0x1000;
  mod->backColor.G = (((float)back->G) * ((float)backIntensity->G / 256)) / 0x1000;
  mod->backColor.B = (((float)back->B) * ((float)backIntensity->B / 256)) / 0x1000;

  mod->colors = &colorBuffer[colorCount];
  //mod->colors = 0;
  colorCount += ((mod->object_model->polyTexCount + mod->object_model->polyNonCount) * 3);
  
  mod->calcColors();  
  (*modelCount)++;
}

void scene::addModel(unsigned long texEID, unsigned char *texInfoArray, unsigned long scale, vector *trans)
{
  //model *mod = allocateModel();
  
  model *mod = modelBuffer[*modelCount];
  
  mod->type           = 2;
  mod->sprite_model   = geom->getSprite(texEID, texInfoArray, scale);
  mod->trans.X        = ((float)(trans->X >> 8) / 0x1000) * 8;  
  mod->trans.Y        = ((float)(trans->Y >> 8) / 0x1000) * 8;
  mod->trans.Z        = ((float)(trans->Z >> 8) / 0x1000) * 8;
  mod->rot.X          = 0;
  mod->rot.Y          = 0;
  mod->rot.Z          = 0;  
  mod->scale.X        = 1;
  mod->scale.Y        = 1;
  mod->scale.Z        = 1;

  (*modelCount)++;
}

void scene::addModel(unsigned long texEID, int count, squad2 *quadInfo, unsigned char *texInfoArray, unsigned long scale, vector *trans)
{
  //model *mod = allocateModel();
  
  model *mod = modelBuffer[*modelCount];
  
  mod->type           = 3;
  mod->fragment_model = geom->getFragment(texEID, count, quadInfo, texInfoArray, scale);
  mod->trans.X        = ((float)(trans->X >> 8) / 0x1000) * 8;  
  mod->trans.Y        = ((float)(trans->Y >> 8) / 0x1000) * 8;
  mod->trans.Z        = ((float)(trans->Z >> 8) / 0x1000) * 8;
  mod->rot.X          = 0;
  mod->rot.Y          = 0;
  mod->rot.Z          = 0;  
  mod->scale.X        = 1;
  mod->scale.Y        = 1;
  mod->scale.Z        = 1;

  (*modelCount)++;
}

void scene::viewSingleWorld(entry *wgeo)
{
  *viewMode = SCENE_VIEWMODE_VIEWWGEOSINGLE;  // viewing just world geometry

  reset();
  cam->setMode(0);
    
  geom->getWorld(wgeo);
  *loaded = true;
}

void scene::viewSingleObject(entry *svtx, int frame)
{
  *viewMode = SCENE_VIEWMODE_VIEWSVTXSINGLE;  // viewing just object geometry

  reset();
  cam->setMode(0);   
   
  geom->getObject(svtx, frame);
  *loaded = true;   
}
  
void scene::startAtZone(entry *zone)
{
  reset();
  if (cam->getMode() != 3)
    cam->setMode(1);
  
  addZone(zone);  
  
  if (!(*viewMode & SCENE_VIEWMODE_VIEWZONESCUMULATIVE))
  {
    viewToZone(0);
    *viewMode = SCENE_VIEWMODE_VIEWZONESCUMULATIVE;
  }
}

zone *scene::addZone(entry *zone)
{
  zoneBuffer[*zoneCount]->load(zone, geom);
  *loaded = true;
  
  return zoneBuffer[(*zoneCount)++];
}

zone *scene::loadZone(entry *zone, int index)
{
  if (index > (*zoneCount-1))
    return 0;
  else if (index == (*zoneCount-1))
    (*zoneCount)++;
  else
    zoneBuffer[index]->unload();
    
  zoneBuffer[index]->load(zone, geom);
  *loaded = true;
  
  return zoneBuffer[index];
}

void scene::viewToZone(int index)
{
  viewZoneIndex = index;
  
  zone *viewZone = zoneBuffer[index];
  if (viewZone->sectionCount == 0)
  {
    cam->translate(viewZone->location.X,
                   viewZone->location.Y,
                   viewZone->location.Z);
  }
  else
  {
    int moveBack = 400;
    
    section *viewSection = viewZone->sections[0];
    vector sectionStart = viewSection->getStart();
    cam->translate(sectionStart.X,
                   sectionStart.Y,
                   sectionStart.Z + moveBack);
                     
    viewZone->selectSection(0);
  }
}

void scene::viewToSection(int section)
{
  viewToSection(viewZoneIndex, section);
}

void scene::viewToSection(int index, int sect)
{
  viewZoneIndex = index;
  
  zone *viewZone = zoneBuffer[index];
  if (viewZone->sectionCount == 0)
  {
    cam->translate(viewZone->location.X,
                   viewZone->location.Y,
                   viewZone->location.Z);
  }
  else if (sect < viewZone->sectionCount)
  {
    int moveBack = 400;
    
    section *viewSection = viewZone->sections[sect];
    vector sectionStart = viewSection->getStart();
    cam->translate(sectionStart.X,
                   sectionStart.Y,
                   sectionStart.Z + moveBack);
 
    // why only here?
    cam->rotation.X = 0;
    cam->rotation.Y = 0;
    cam->rotation.Z = 0;
    
    viewZone->selectSection(sect);
  }
}

unsigned long scene::getSceneFlags()
{
  return sceneFlags;
}

void scene::setSceneFlags(unsigned long flags)
{
  sceneFlags |= flags;
}

void scene::clearSceneFlags(unsigned long flags)
{
  sceneFlags &= !flags;
}

void scene::toggleSceneFlags(unsigned long flags)
{
  sceneFlags ^= flags;
}


unsigned long scene::getViewFlags()
{
  return *viewFlags;
}

void scene::setViewFlags(unsigned long flags)
{
  *viewFlags |= flags;
}

void scene::clearViewFlags(unsigned long flags)
{
  *viewFlags &= !flags;
}

void scene::toggleViewFlags(unsigned long flags)
{
  *viewFlags ^= flags;
}


unsigned long scene::getViewMode()
{
  return *viewMode;
}

void scene::setViewMode(unsigned long mode)
{
  *viewMode = mode;
}

// ABCDEFGH IJKLMNOP QRSTUVWX YZ123456
// 6 = single wgeo flag
// 5 = viewing wgeos flag
// 4 = single svtx flag
// 3 = viewing svtxes flag
// 2 = single sprite flag
// 1 = viewing sprites flag
// Z = single zone flag
// Y = viewing zones flag
// X = view zone collision octrees flag
// W = view zone section camera paths flag
// V = view zone worlds flag
// U = view zone objects flag

void scene::draw()
{  
  // These modes are strictly for drawing only the distinct scene geometry
  //  (as opposed to the scene's models, where multiple models can use the same
  //   geometry)
  //
  // SCENE_VIEWMODE_VIEWWGEO - used when we want to view world geometries that 
  // have been *loaded since the last clear at their intended locations, via loading 
  // their parent zone's or directly loading a requested wgeo; 
  // this allows the following behaviors:
  //  - in a single request, load to the scene a single wgeo; used when viewing
  //    only a single wgeo (use SCENE_VIEWMODE_WGEOSINGLE to force) 
  //  - in a single request, load to the scene each wgeo (at their respective 
  //    locations) from a single zone; used when viewing only a single zone  
  //  - in a single request, load to the scene all distinct world geometry from 
  //    multiple zones, possibly all zones that exist in the level; used when viewing
  //    the entire level 
  //  - cumulatively 'add' to the scene any new world geometry as their parent zones
  //    are encountered (i.e. *loaded)
  //  
  if (*viewMode & SCENE_VIEWMODE_VIEWWGEO)
  {
    if (*viewMode & SCENE_VIEWMODE_WGEOSINGLE)
    {
      geom->worldBuffer[0]->drawAt({0, 0, 0}); //drawAtLocation()?
    }
    else
    {
      for (int lp = 0; lp < geom->worldCount; lp++)
        geom->worldBuffer[lp]->drawAtLocation();
    }
  }
  
  // SCENE_VIEWMODE_VIEWSVTX - used when we want to view object geometries that 
  // have been *loaded since the last clear at their intended locations, via loading 
  // their parent zone's or directly loading a requested svtx; 
  // this allows the following behaviors:
  //  - in a single request, load to the scene a single svtx; used when viewing
  //    only a single model (use SCENE_VIEWMODE_SVTXSINGLE to force) 
  //  - in a single request, load to the scene a svtx *for each entity in a single
  //    zone; generally not used as this wastes memory by potentially loading multiple 
  //    copies of the same svtx geometry, distinct by only the start locations, for 
  //    entities in the zone  
  //
  // This is only used for SCENE_VIEWMODE_SVTXSINGLE, when we want to view only a 
  // single svtx. Loading svtxes for each object in zone(s) is instead accomplished
  // by creating 'Models' for each svtx, where multiple models can reference the same
  // geometry, thereby conserving memory.
  
  if (*viewMode & SCENE_VIEWMODE_VIEWSVTX)
  {
    if (*viewMode & SCENE_VIEWMODE_SVTXSINGLE)
    {
      geom->objectBuffer[0]->drawAt({0, 0, 0});
    }
    else
    {
      for (int lp = 0; lp < geom->objectCount; lp++)
        geom->objectBuffer[lp]->drawAtLocation();
    }
  }  
  
  if (*viewMode & SCENE_VIEWMODE_VIEWSPRITE)
  {
    if (*viewMode & SCENE_VIEWMODE_SPRITESINGLE)
    {
      geom->spriteBuffer[0]->drawAt({0, 0, 0});
    }
    else
    {
      for (int lp = 0; lp < geom->spriteCount; lp++)
        geom->spriteBuffer[lp]->drawAtLocation();
    }
  }
  
  if (*viewMode & SCENE_VIEWMODE_VIEWZONE)
  {
    if (*viewMode & SCENE_VIEWMODE_ZONESINGLE)
    {
      zoneBuffer[0]->draw(*viewFlags);
    }
    else
    {
      for (int lp = 0; lp < *zoneCount; lp++)
        zoneBuffer[lp]->draw(*viewFlags);
    }
  }
  
  if (*viewMode & SCENE_VIEWMODE_VIEWMODELS)
  {
	  /*
    if (modelRequestQueue)
    {
      if (modelRequestQueue->waiting())
        clearModels();
      while (modelRequestQueue->waiting())
      {
        modelRequest request = modelRequestQueue->process();
        addModel(request.svtx, request.frame, 
                 &request.trans, &request.rot, &request.scale,
                 &request.light, &request.color, &request.back, &request.backIntensity);
      }
    }
		*/
    
    for (int lp = 0; lp < *modelCount; lp++)
      modelBuffer[lp]->draw();
  }

  
  if (*viewMode & SCENE_VIEWMODE_VIEWMISC)
  {
    for (int lp = 0; lp < geom->miscCount; lp++)
      geom->miscBuffer[lp]->draw();
  }
  
  // Reset any buffers output to by vertex shaders so the same space is re-used thus
  // preventing overflow.
  colorCount = 0;
} 