#ifndef ZONE_H
#define ZONE_H

#include <string.h>  //for memset

#include "crash/nsd.h"
#include "crash/nsf.h"
#include "crash/eid.h"

#include "goolengine.h"
#include "objectengine.h"
#include "cameraengine.h"

// global variable definitions
#define var_61A30 globals[0x69]
#define var_61A5C globals[0x74]
#define var_61A60 globals[0x75]
#define var_61A64 globals[0x76]

unsigned long initLevel();
void initZone(entry *zone);
void updateLevel(entry *zone, unsigned char *section, signed long progressV, unsigned long flags);
void updateLevelMisc(unsigned char *headerMisc, unsigned long flags);

entry *findZone(entry *zone, cpoint *location);

typedef struct
{
  cvector playerLoc;
  cangle playerRot;
  cvector playerScale;
  unsigned long zoneEID;
  unsigned long sectionID;
  unsigned long zoneProgress;
  unsigned long levelID;
  bool flag;
  unsigned long states[304];
  unsigned long boxCount;
} zoneState;

#include "integ.h"

#endif