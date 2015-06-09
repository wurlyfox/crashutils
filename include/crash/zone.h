#ifndef CRASH_ZONE_H
#define CRASH_ZONE_H

#include "nsd.h"
#include "nsf.h"
#include "chunk.h"
#include "entry.h"
#include "eid.h"
#include "r3000a.h"


typedef struct
{
  unsigned long world;
  unsigned long reserved[0xF];
} crash_zone_wgeo;

typedef struct
{
  unsigned long worldCount;
  crash_zone_wgeo world[8];
  unsigned long headerColCount;
  unsigned long sectionCount;
  unsigned long entityCount;
  unsigned long neighborCount;
  unsigned long neighbor[8];
  unsigned long tpageCount;
  unsigned long tchunkCount;
  unsigned long tpage[8];
  unsigned long tchunk[32];
  unsigned long gameFlags;
  unsigned long vramFillHeight;
  unsigned long unknownA;
  unsigned long visibilityDepth;
  unsigned long unknownB;
  unsigned long unknownC;
  unsigned long unknownD;
  unsigned long unknownE;
  unsigned long flags;
  unsigned long deathY;
  unsigned long unknownF;
  unsigned long unknownG;
  unsigned long unknownH;
  unsigned char vramFillR;
  unsigned char vramFillG;
  unsigned char vramFillB;
  unsigned char unusedA;
  unsigned char farColorR;
  unsigned char farColorG;
  unsigned char farColorB;
  unsigned char unusedB;
  unsigned short objectLightMatrix[9];
  unsigned short objectBackColor[3];
  unsigned short objectColorMatrix[9];
  unsigned short objectBackColorIntensity[3];
  unsigned short playerLightMatrix[9];
  unsigned short playerBackColor[3];
  unsigned short playerColorMatrix[9];
  unsigned short playerBackColorIntensity[3];
} crash_zone_header;

typedef struct
{
  signed long X;
  signed long Y;
  signed long Z;
  unsigned long width;
  unsigned long height;
  unsigned long depth;
  unsigned long unknown;
  struct crash_collision
  {
    unsigned short rootIndex;
    unsigned short maxDepthX;
    unsigned short maxDepthY;
    unsigned short maxDepthZ;
    unsigned short nodes[];
  } collision[];
} crash_zone_dimensions;

typedef struct
{
  unsigned long slst;
  unsigned long parentZone;
  unsigned long neighborSectionCount;
  struct neighborSectionDescriptor
  {
    unsigned char relation;
    unsigned char neighborZoneIndex;
    unsigned char sectionIndex;
    unsigned char goal;
  } neighborSection[4];
  unsigned char entranceIndex;
  unsigned char exitIndex;
  unsigned short length;
  unsigned short camMode;
  unsigned short avgNodeDist;
  unsigned short camZoom;
  unsigned short unknownA;
  unsigned short unknownB;
  unsigned short unknownC;
  signed short pathDirectionX;
  signed short pathDirectionY;
  signed short pathDirectionZ;
  struct sectionCamPathNode
  {
    signed short camX;
    signed short camY;
    signed short camZ;
    signed short camRotX;
    signed short camRotY;
    signed short camRotZ;
  } *camPath;
} crash_zone_section;

typedef struct
{
  unsigned long reserved;
  unsigned short initModeFlags;
  unsigned short group;
  unsigned short ID;
  unsigned short pathCount;
  unsigned short initFlagsA;
  unsigned short initFlagsB;
  unsigned short initFlagsC;
  unsigned char type;
  unsigned char subtype;
  struct objectCamPathNode
  {
    signed short camX;
    signed short camY;
    signed short camZ;
  } *camPath;
} crash_zone_entity;

typedef struct
{
  signed long X;
  signed long Y;
  signed long Z;
  unsigned long width;
  unsigned long height;
  unsigned long depth;
} crash_zone_space;

void readCrashZoneHeader(entry *zone, crash_zone_header &crashZoneHeader);
void readCrashZoneSpace(entry *zone, crash_zone_space &crashZoneSpace);
void readCrashZoneSection(entry *zone, int section, crash_zone_section &crashZoneSection);
void readCrashZoneSection(unsigned char *zoneSection, crash_zone_section &crashZoneSection);
void readCrashZoneEntity(entry *zone, int entity, crash_zone_entity &crashZoneEntity);
void readCrashZoneEntity(unsigned char *zoneEntity, crash_zone_entity &crashZoneEntity);

unsigned char *getCrashZoneSection(entry *zone, int section);
unsigned char *getCrashZoneEntity(entry *zone, int entity);

#endif