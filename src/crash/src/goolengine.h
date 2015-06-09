#ifndef GOOL_ENGINE_H
#define GOOL_ENGINE_H

#include <math.h>

#include "crash/nsf.h"
#include "crash/nsd.h"

#define COUNT_GAMEOBJECTS  3592  //for states of objects thru game
#define COUNT_LEVELOBJECTS  304  //for states of objects thru level 
#define COUNT_OBJECTS        96  //for objects currently executing (minus crash and 4 other potential objects)

//use if we want to analyze execution of an object
#define DEBUG_GOOL
#define DEBUG_GOOL_OBJTYPE 0

//interpreter/routine return codes
#define GOOL_CODE_SUCCESS               0xFFFFFF01
#define GOOL_CODE_FAIL                  0xFFFFFF02

//interpreter execution state flags
#define GOOL_XSTAT_SUSPENDONRETURN      0x1   
#define GOOL_XSTAT_SUSPENDONRETURNLINK  0x2
#define GOOL_XSTAT_SUSPENDONANIMATE     0x4
#define GOOL_XSTAT_EVENTSERVICE         0x8
#define GOOL_XSTAT_STATUSPRESERVE       0x10
#define GOOL_XSTAT_RETURNEVENT          0x20
#define GOOL_XSTAT_ALL                  0X3F

//interpreter extended execution state flags (not in original assembly)
#define GOOL_XSTATEX_SUSPENDED          0x100

#define isErrorCode(val) (val > 0xFFFFFF01)

#define frameTag(start, end) ((end << 16) | start)
#define getFP(obj) (obj->process.fp == 0) ? 0 : ((unsigned long)obj->process.fp - (unsigned long)&obj->process)
#define getSP(obj) (obj->process.sp == 0) ? 0 : ((unsigned long)obj->process.sp - (unsigned long)&obj->process)

#define push(obj, val) *(obj->process.sp++) = (unsigned long)(val)
#define pop(obj)       *(--obj->process.sp)
#define peek(obj)      *(obj->process.sp)
#define peekback(obj)  *(obj->process.sp-1)

#define srcVal(ins) ((ins >> 12) & 0xFFF)
#define dstVal(ins) ((ins)       & 0xFFF)

#define getChildren(obj)        ((obj->type == 2) ? obj->children : obj->process.children)
#define setChildren(obj, child) ((obj->type == 2) ? obj->children = child : obj->process.children = child) 
#define hasParent(obj)          (obj->process.parent != 0)
#define hasChildren(obj)        (obj->process.children != 0)

struct object;

struct cdblink
{
  object *prev;
  object *next;
  unsigned long unknown;
};  

typedef struct
{
  signed long X;
  signed long Y;
  signed long Z;
} cpoint, cvector;

typedef struct
{
  signed long Y;
  signed long X;
  signed long Z;
} cangle;

typedef struct
{
  cpoint xyz1;
  cpoint xyz2;
} cbound, cspace;

typedef struct
{
  signed long W;
  signed long H;
  signed long D;
} cdimension;

typedef struct
{
  cvector pos;
  cdimension dim;
} cvolume;

typedef struct
{
  cvector V1;
  cvector V2;
  cvector V3;
} cmatrix;

typedef struct
{
  signed short X;
  signed short Y;
  signed short Z;
} csvector;

typedef struct
{
  csvector V1;
  csvector V2;
  csvector V3;
} csmatrix, cslightmatrix;

typedef struct
{
  signed short Y2;
  signed short X2;
  signed short Y1;
  signed short X1;
} csquad;

struct cdblink;

typedef struct
{
  cvector trans;             //0x80, 0x84, 0x88
  cangle rot;                //0x8C, 0x90, 0x94
  cvector scale;             //0x98, 0x9C, 0xA0
  union
  {
    cvector miscA;             //0xA4, 0xA8, 0xAC
    cdblink boxLink;
  };
  cangle  miscB;             //0xB0, 0xB4, 0xB8
  cvector miscC;             //0xBC, 0xC0, 0xC4
} cvectors;
  
typedef struct
{
  unsigned short R;
  unsigned short G;
  unsigned short B;
} cscolor;

typedef struct
{
  cscolor V1;
  cscolor V2;
  cscolor V3;
} cscolormatrix;

typedef struct
{
  cslightmatrix lightmatrix;
  cscolor color;
  cscolormatrix colormatrix;
  cscolor intensity;
} cshader;

struct cprocess
{
  object *self;
  object *parent;
  object *sibling;
  object *children;
  object *link[2];
  object *collider;
  object *invoker;
  
  cvectors vectors;
  
  unsigned long statusA;       //0xC8
  unsigned long statusB;       //0xCC
  unsigned long statusC;       //0xD0
  unsigned long initSubIndex;  //0xD4
  unsigned long PIDflags;     //0xD8 (PID in left 3 bytes, flags in right byte)
  unsigned long *sp;          //0xDC
  unsigned long *pc;          //0xE0
  unsigned long *fp;          //0xE4
  unsigned long *subFuncA;
  unsigned long *subFuncB;
  unsigned long F0;
  union
  {
    bool miscFlag;
    object *miscChild;
    entry *miscEntry;
  };
  unsigned long F8;
  unsigned long aniStamp;
  unsigned long initStamp;
  unsigned long aniCounter;
  unsigned char *aniSeq;
  unsigned long aniFrame;
  
  unsigned char *entity;
  signed long pathProgress;
  unsigned long pathCountEtc;  //num of paths in left 3 bytes, right byte unknown
  unsigned long _11C;
  unsigned long routineFlagsA;
  signed long speed;
  unsigned long displayMode;
  unsigned long _12C;
  unsigned long _130;
  signed long _134;
  unsigned long _138;
  unsigned long event;  //0x13C
  signed long camZoom;
  unsigned long _144;
  signed long _148;
  unsigned long _14C;
  unsigned long _150;  
  unsigned long _154;
  unsigned long memory[0x41];
};


struct object
{ 
  unsigned long type;
  
  union
  {
    object *children;
    unsigned long subtype;
  };
  
  cbound bound;
  entry *global;
  entry *external;
  entry *zone;
  unsigned long routine;
  cshader colors;
  cprocess process;
};

struct event
{
  unsigned long id;
  bool issue;
};


unsigned long initObjects();
unsigned long loadStates(unsigned short levelID);
inline object *findPrevChild(object *child, object *parent);
inline void addChild(object *child, object *newParent);
inline void adoptChild(object *child, object *newParent);
unsigned long familyRoutinePBC(object *obj, unsigned long (*routine)(object*,bool), bool arg);
unsigned long familyRoutineCBP(object *obj, unsigned long (*routine)(object*,void*), void* arg);

object *findObject(object *obj, object*(*routine)(object*,unsigned long), unsigned long arg);      //runs a thru a processes entire family till it finds a process that isnt of type 2 and satisfys the specified routine
object *find_routineFlags(object *obj, unsigned long flags);

unsigned long familyRoutinePBCH(object *obj, unsigned long (*routine)(object*,bool), bool arg);
unsigned long familyRoutineCBPH(object *obj, unsigned long (*routine)(object*,void*), void* arg);   //this is accomplishing what 1B6F0 does for a family of a process, but only with the first 2 generations, but then for 
bool checkObjectBit(object *obj, unsigned long bit);
object *hasPID(object *obj, unsigned long PID);

object *spawnObject(entry *entityEntry, int entityIndex);
object *addObject(object *parent, unsigned long levelListIndex, unsigned long subIDIndex, unsigned long count, unsigned long *args, bool flag);
bool terminateObject(object *obj);
bool terminateObject(object *obj, bool termEvent);  //for a processes entire 'family', set all links to refer to the initial process [by parent field] and its children [link field
unsigned long terminateZoneObject(object *obj, entry *zone);

unsigned long issueEvent(object *sender, object *recipient, unsigned long tag, unsigned long count, unsigned long *args);
void copyColors(object *dst, unsigned char *src, unsigned long offset);
void copyVectors(object *src, object *dst);
void copyEuler(cangle *src, cangle *dst);
void copyVector(cvector *src, cvector *dst);
void copyVector(csvector *src, csvector *dst);
void addVector(cvector *src, cvector *dst);
void addVector(csvector *src, csvector *dst);
void addVector(cvector *srcA, cvector *srcB, cvector *dst);
void addVector(csvector *srcA, csvector *srcB, csvector *dst);
void subVector(cvector *src, cvector *dst);
void subVector(csvector *src, csvector *dst);
void subVector(cvector *srcA, cvector *srcB, cvector *dst);
void subVector(csvector *srcA, csvector *srcB, csvector *dst);
void getSpace(cpoint *center, cbound *bound, cspace *space);
bool colSpace(cspace *spaceA, cspace *spaceB);
unsigned long initProcess(object *obj, unsigned long levelListIndex, unsigned long subIDIndex, unsigned long count, unsigned long *args);
unsigned long executeSub(object *obj, unsigned long subid, unsigned long count, unsigned long *args);
unsigned long newFrame(object *obj, unsigned long offset, unsigned long header);

bool testControls(unsigned long instruction, unsigned long port);

unsigned long interpret(object *obj, unsigned long &arg1, event &arg2);
inline unsigned long translate(object *obj, unsigned short addr);
inline unsigned long *translate(object *obj, unsigned short addr, bool &endian);
inline unsigned long translateP(object *obj, unsigned short addr);
inline unsigned long *translateP(object *obj, unsigned short addr, bool &endian);
unsigned long transA(object *obj, unsigned short addr);
unsigned long *transA(object *obj, unsigned short addr, bool &endian);

unsigned long initDebugSub(object *obj, unsigned long subid, unsigned long count, unsigned long uval);
unsigned long finishDebugSub(object *obj, int type, unsigned long result);
unsigned long finishInitProcess(unsigned long result);

int getObjectIndex(object *obj); 

void printObject(object *obj, unsigned long state);

#include "integ.h"

#endif