#include "crash_system.h"

// singleton crash system
crash_system *global;

// crash-end: r/w access to majority of game parameters thru just these fields
extern NSD *nsd;
extern NSF *nsf;
extern ccamera camera;
extern entry *currentZone;
extern unsigned char *currentSection;
extern unsigned long currentProgressV;
extern object *objects;
extern object *player;

extern bool *keys;

void crashSystemDraw()    
{ 
  global->postMessage(CSM_DRAW);
  global->postMessage(CSM_SYNC);
  global->postMessage(CSM_POSTDRAW);
}
entry *crashSystemPage(unsigned long EID) 
{
  entry *result;
  global->postMessage(CSM_PAGE, (param)EID, (param)&result);
  return result;
}
unsigned long crashSystemControl()
{
  unsigned long control;
  global->postMessage(CSM_CONTROL, (param)&control);
  return control;
}
unsigned long crashSystemTime()
{
  unsigned long csmtime;
  global->postMessage(CSM_TIME, (param)&csmtime);
  return csmtime;
}
void crashSystemZoneWorldModels(entry *zone)
{
  global->postMessage(CSM_ZONE_WORLD_MODELS, (param)zone);
}
void crashSystemObjectModel(entry *svtx, int frame,
                           cvector *trans, cangle *rot, cvector *scale,
                           cslightmatrix *light, cscolormatrix *color, cscolor *back, cscolor *backIntensity)
{
  param_cmodel_obj params = { svtx, frame, trans, rot, scale, light, color, back, backIntensity };
  global->postMessage(CSM_GFX_OBJMODEL, (param)&params);
}
void crashSystemSpriteModel(unsigned long texEID, unsigned char *texInfoArray, int scale, cvector *trans)
{
  param_cmodel_spr params = { texEID, texInfoArray, scale, trans };
  global->postMessage(CSM_GFX_SPRMODEL, (param)&params);
}
void crashSystemFragmentModel(unsigned long texEID, int quadCount, csquad *quads,
                             unsigned char *texInfoArray, int scale, cvector *trans)
{
  param_cmodel_frg params = { texEID, quadCount, quads, texInfoArray, scale, trans };
  global->postMessage(CSM_GFX_FRGMODEL, (param)&params);
}        
void crashSystemViewMatrix()
{
  global->postMessage(CSM_VIEWMATRIX);
}
void crashSystemCameraUpdate()
{
  global->postMessage(CSM_CAMERA_UPDATE);
}
void crashSystemHandleObjects()
{
  global->postMessage(CSM_HANDLE_OBJECTS);
}


crash_system::crash_system(csystem *par)
{
  timer = 0;
  controller = 0;
  
  par->addChild(this);
  postMessage(CSM_CREATE);

  global = this;
	
  thread *theThread;
  postMessage(MSG_NEW_THREAD, (param)&theThread);
  create(theThread);
}

void crash_system::run()
{
  postMessage(CSM_INIT);
  
  crash_init();
  crash_main();
}

//These functions define all communications between the Crash engine and
//all non-psx platform independent implementations
void crash_system::messageRouter(csystem *src, int msg, param lparam, param rparam)
{
  csystem::messageRouter(src, msg, lparam, rparam);
  
  switch (msg)
  {
    // CrashPC local messages (outgoing notifications/requests)
    case CSM_CREATE:     onCreate();                                       break;
    case CSM_INIT:         onInit();                                       break;
    case CSM_SYNC:         onSync();                                       break;

    case CSM_DRAW:          postMessage(GSM_INVALIDATE);                   
                            postMessage(GSM_RENDER);
                            postMessage(MSG_HANDLE_KEYS, (param)keys);
                                                                           break;
    case CSM_POSTDRAW:      postMessage(GSM_CLEAR_MODELS);                 break;
    case CSM_GFX_OBJMODEL:  postMessage(GSM_OBJMODEL, lparam);             break;
    case CSM_GFX_SPRMODEL:  postMessage(GSM_SPRMODEL, lparam);             break;
    case CSM_GFX_FRGMODEL:  postMessage(GSM_FRGMODEL, lparam);             break;
    case CSM_PAGE:          postMessage(MSG_LOOKUP_ENTRY, lparam, rparam); break;
    
    case CSM_CAMERA_UPDATE:  onCameraUpdate();                             break;
    case CSM_VIEWMATRIX:     onViewMatrix();                               break;
    case CSM_HANDLE_OBJECTS: onHandleObjects();                            break;
    
    case CSM_CONTROL:
    {
      unsigned long *control = (unsigned long*)lparam;
      *control = controller->readControls();
    }
    break;
    case CSM_TIME:
    {
      unsigned long *time = (unsigned long*)lparam;
      *time = timer->getElapsedTime(true);
    }
    break;
    
    // CrashPC system local messages (additional incoming requests)
    case CSM_CONTROL_TOGGLE_ACTIVE: onControlToggleActive(); break;
    case CSM_CREATE_USER_EVENT: onCreateUserEvent((userEventEntry*)lparam); break;
    case CSM_CREATE_USER_PROCESS: onCreateUserProcess((userProcessEntry*)lparam); break;
    case CSM_SPAWN_USER_OBJECT: onSpawnUserObject((userObjectEntry*)lparam); break;
    
    // Global messages from other systems (incoming notifications)
    case MSG_INIT_NSD: onInitNSD((NSF*)lparam, (NSD*)rparam);
    case MSG_HANDLE_KEYS: onHandleKeys((bool*)lparam); break;
    case MSG_SELECT_ZONE: onSelectZone((entry*)lparam); break;
    case MSG_SELECT_SECTION: onSelectSection((unsigned char*)lparam); break;
  }
}

// Outgoing request/notification messages from CrashPC
void crash_system::onCreate()
{
  postMessage(MSG_NEW_TIMER, (param)&timer);
  controller = new control();
  
  userObjectQueue.allocate(10);
  userProcessQueue.allocate(10);
  userEventQueue.allocate(10);
}

void crash_system::onInit()
{
  // suspend until nsd & nsf inited
  if (nsd == 0 || nsf == 0) { suspend(); }
  
  timer->init(60, 0.002f);

  postMessage(GSM_ALLOW_THREAD);
}

void crash_system::onSync()
{
  timer->frameCap();
}

void crash_system::onCameraUpdate()
{
  if (currentZone != ::currentZone && ::currentZone)
    postMessage(MSG_SELECT_ZONE, (param)::currentZone);
    
  if (currentSection != ::currentSection && ::currentSection)
    postMessage(MSG_SELECT_SECTION, (param)::currentSection);
    
  if (currentProgressV != ::currentProgressV)
    currentProgressV = ::currentProgressV;
}

void crash_system::onViewMatrix()
{
  postMessage(GSM_CAMERA_SYNC_EXTERNAL, (param)&camera.trans, (param)&camera.rot);
}

void crash_system::onHandleObjects()
{
  // Process request messages
  while (!userObjectQueue.empty())
  {
    userObjectEntry args = userObjectQueue.remove();
    spawnObject(args.zone, args.entityIndex);
  }
  while (!userProcessQueue.empty())
  {
    userProcessEntry args = userProcessQueue.remove();
    addObject(args.parent, args.codeID, args.routineID, args.count, args.args, true);
  }
  while (!userEventQueue.empty())
  {
    userEventEntry args = userEventQueue.remove();
    issueEvent(args.sender, args.recipient, args.event, args.count, args.args);
  }
}

// Incoming notification messages from other systems
void crash_system::onInitNSD(NSF *nsfA, NSD *nsdA)
{
  nsf = nsfA;
  nsd = nsdA;
  resume();
}

void crash_system::onHandleKeys(bool *keys)
{
  if (controller)
    controller->setKeyboardBuffer(keys);
}

void crash_system::onSelectZone(entry *zone)
{
  currentZone = zone;
  ::currentZone = zone;
}

void crash_system::onSelectSection(unsigned char *section)
{
  currentSection = section;
  ::currentSection = section;
}

// Incoming request messages from other systems
void crash_system::onControlToggleActive()
{
  controller->toggleActive();
}

void crash_system::onSpawnUserObject(userObjectEntry *objectEntry)
{
  userObjectQueue.add(*objectEntry);
}

void crash_system::onCreateUserProcess(userProcessEntry *processEntry)
{
  userProcessQueue.add(*processEntry);
}

void crash_system::onCreateUserEvent(userEventEntry *eventEntry)
{
  userEventQueue.add(*eventEntry);
} 