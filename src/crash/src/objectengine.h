#ifndef OBJECT_ENGINE_H
#define OBJECT_ENGINE_H

#include "goolengine.h"
#include "zoneengine.h"    //for finding zones
#include "matrix.h"        //for matrix transformations
#include "control.h"

#include "crash/r3000a.h"

#define PI 3.14159265d

#define CTCVC(C) (C << 8)
#define CTGVC(C) (C >> 8)
#define CTCA(A) (signed long)((A * 0x800) / PI)
#define CTGA(A) (PI * (A / 0x800))

#define FIXED2FLOAT(C) ((float)C / 0x1000)
#define FLOAT2FIXED(C) (unsigned long)(C * 0x1000)

#define cabs(V) ((V < 0) ? -V : V)

// global variable definitions
#define dispAniGlob  globals[0x9]
#define dispAniGlobF globals[0x4]

typedef struct
{
  cspace space;
  object *obj;
} spacePair;

typedef struct
{
  unsigned long mode;
  unsigned long angle;
  unsigned long velocityScale;
} cmove;

typedef struct
{
  unsigned long accel;
  unsigned long maxSpeed;
  unsigned long unknown;
  unsigned long decel;
} caccel;

typedef struct
{
  unsigned char traversal[0x1000];
  bool traversed;
  unsigned long traversalLen;
  cspace nodeSpace;     //defines the 3-dimensional region of space surrounding the area of nearby nodes for the solid collision test
  cspace colliderSpace; //defines the 3-dimensional region of space that the colliding object occupies for the solid collision test
  signed long floor;
  signed long ceil;
  signed long floorNodesY;
  signed long solidNodesY;
  signed long solidObjsY;
  unsigned long counter;
} traversalInfo;

typedef struct
{
  object *sender;
  unsigned long event;
  unsigned long argCount;
  unsigned long mode;
  unsigned long count;
  unsigned long *args;
} eventQuery;

unsigned long handleObjects(bool flag);
unsigned long handleObject(object *obj, bool flag);

void renderObject(object *obj);

void objectBound(object *obj, cvector *scale);
unsigned long objectPath(object *obj, unsigned long progressIndex, cvector *trans);
signed long objectRotate(signed long angA, signed long angB, signed long speed, object *obj);
signed long objectRotateB(signed long angA, signed long angB, signed long speed, object *obj);
signed long approach(signed long oldVal, signed long newVal, signed long maxVal);
signed long leastAngDist(signed long angA, signed long angB);
void objectSpace(object *obj);
unsigned long objectColors(object *obj);
unsigned long objectPhysics(object *obj, bool flag);
void getSpace(cvector *location, cbound *bound, cspace *output);

bool isOutOfRange(object *obj, cvector *pointA, cvector *pointB, unsigned long boundW, unsigned long boundH, unsigned long boundD);
bool isColliding(cpoint *point, cspace *space);
bool isColliding(cspace *spcA, cspace *spcB);  //sub_80026CA8
bool isContained(cspace *src, cspace *space);
bool isInsideOf(cspace *src, cspace *space);
bool isOutsideOf(cspace *src, cspace *space);

unsigned long transSmoothStopAtSolid(object *obj, cvector *velocity, traversalInfo *trav);
void transPullStopAtSolid(object *obj, traversalInfo *trav, cvector *trans, cvector *velocity);
void transStopAtSolid(object *obj, traversalInfo *trav, cvector *trans, cvector *pull, cvector *newTrans);

signed long stopAtFloor(object *obj, cvector *oldTrans, cvector *newTrans, traversalInfo *trav, signed long *output);
unsigned long traverseNodes(cvector *newTrans, object *obj, traversalInfo *trav);
void handleNodes(object *obj, traversalInfo *trav, cspace *nodeSpace, cspace *colliderSpace, signed long height, signed long *output, signed long defY, bool (*routine)(object*, unsigned long));
bool processNode(object *obj, unsigned long nodeValue);
signed long findHighestBelowObjectY(object *obj, cvector *trans, cvector *newTrans, traversalInfo *trav);
void plotWalls(cvector *trans, object *obj, traversalInfo *trav);
void plotZoneWalls(traversalInfo *trav, cspace *nodeSpace, bool flag, signed long yvalA, signed long yvalB, signed long transY, signed long transX, signed long transZ);
void plotObjWalls(cvector *trans, object *obj, traversalInfo *trav, bool flag);
void plotWall(signed char x, signed char z);
void plotWallB(signed char x, signed char z);
unsigned long stopAtWalls(cvector *trans, unsigned long reqMovePtX, unsigned long reqMovePtZ, unsigned long *movePtX, unsigned long *movePtZ, object *obj, unsigned long ret);
unsigned long replotWalls(bool noClear, bool flags, cvector *trans, object *obj);
signed long findCeil(object *obj, cvector *trans, traversalInfo *trav);
signed long findAvgY(object *obj, traversalInfo *trav, cspace *nodeSpace, cspace *colliderSpace, unsigned long typeA, unsigned long typeB, signed long defY);
unsigned long stopAtZone(object *obj, cvector *trans);
unsigned long objectCollide(object *collidee, cspace *collideeSpc, object *collider, cspace *colliderSpc);

unsigned long issueToAll(object *sender, unsigned long event, unsigned long mode, unsigned long count, unsigned long *args);
unsigned long issueIfColliding(object *recipient, eventQuery *query);

void queryNodes(unsigned char *zoneDim, cspace *testSpace, unsigned long *output, unsigned long &outputIndex);
void queryNodesR(unsigned char *zoneDim, unsigned short node, cvolume division, cbound testBound, int level, unsigned long *output, unsigned long &outputIndex);

void readyNodeQuery(traversalInfo *trav, cspace *nodeSpace);

#include "integ.h"

#endif