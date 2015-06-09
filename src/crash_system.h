#ifndef CRASH_SYSTEM_H
#define CRASH_SYSTEM_H

#include "system.h"
#include "thread.h"
#include "time.h"
#include "queue.h"

#include "message.h"

#include "crash/control.h"
#include "crash/src/main.h"

typedef struct
{
  entry *zone;
  unsigned long entityIndex;
} userObjectEntry;

typedef struct
{
  object *parent;
  unsigned long codeID;
  unsigned long routineID;
  unsigned long count;
  unsigned long *args;
} userProcessEntry;

typedef struct
{
  object *sender;
  object *recipient;
  unsigned long event;
  unsigned long count;
  unsigned long *args;
} userEventEntry;

typedef struct
{
  entry *svtx;
	unsigned long frame;
	cvector *trans;
	cangle *rot;
	cvector *scale;
	cslightmatrix *light;
	cscolormatrix *color;
	cscolor *back;
	cscolor *backIntensity;
} param_cmodel_obj;

typedef struct
{
  unsigned long texEID;
	unsigned char *texInfoArray;
	unsigned long scale;
	cvector *trans;
} param_cmodel_spr;

typedef struct
{
  unsigned long texEID;
	unsigned long quadCount;
	csquad *quads;
	unsigned char *texInfoArray;
	unsigned long scale;
	cvector *trans;
} param_cmodel_frg;

template class queue<userObjectEntry>;
template class queue<userProcessEntry>;
template class queue<userEventEntry>;

class crash_system : public csystem, public thread
{
  private:
  
  void run();

  time *timer;
  control *controller;
 
  // used to track changes in the respective game variables of same names
  entry *currentZone;
  unsigned char *currentSection;
  unsigned long currentProgressV;
  
  // used for user-requested in-game routine invocations
  queue<userObjectEntry> userObjectQueue;
  queue<userProcessEntry> userProcessQueue;
  queue<userEventEntry> userEventQueue;

  protected:
  
  void messageRouter(csystem *src, int msg, param lparam, param rparam);
  
  void onCreate();  
	void onInit();
	void onSync();
	void onCameraUpdate();
	void onViewMatrix();
  void onHandleObjects();
  
  void onInitNSD(NSF *nsfA, NSD *nsdA);
  void onHandleKeys(bool *keys);
	void onSelectZone(entry *zone);
	void onSelectSection(unsigned char *section);

  void onControlToggleActive();
  void onSpawnUserObject(userObjectEntry *objectEntry);
  void onCreateUserProcess(userProcessEntry *processEntry);
  void onCreateUserEvent(userEventEntry *eventEntry);
 
  public:
  
  crash_system(csystem* par);
  ~crash_system() {};
};


#endif