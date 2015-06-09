#ifndef CAMERA_ENGINE_H
#define CAMERA_ENGINE_H

#include "crash/r3000a.h"
#include "crash/entry.h"
#include "crash/nsd.h"
#include "crash/nsf.h"

#include "goolengine.h"

typedef struct
{
  cvector trans;
  cvector rot;
  cvector scale;
} ccamera;

typedef struct
{
  unsigned char *curSection;
  unsigned char *nextSection;
  signed long progressV;
  signed long exitV;
  signed long entranceV;
  signed long changeProgressV;
  unsigned long dist;
  unsigned long changeFlags;
  unsigned long relationZ;
  signed long panX;
  signed long zoomZ;
  signed long panY;
  bool progressMade;
} ccameraInfo;

unsigned long cameraCalculate(unsigned char *section, signed long progressV, ccamera *cam);

bool cameraUpdate();
void cameraFollow(object *obj, bool flag);
bool cameraGetProgress(cvector *trans, unsigned char *section, ccameraInfo *cam, unsigned long bitfield, bool flag);
bool cameraGetProgressAlt(cvector *trans, unsigned char *section, ccameraInfo *cam, unsigned long bitfield, bool flag);
void adjustCamProgress(signed long camSpeed, ccameraInfo *cam); //sub_80029F6C

#include "integ.h"

#endif
