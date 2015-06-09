#include "zone.h"

void readCrashZoneHeader(entry *zone, crash_zone_header &crashZoneHeader)
{
  unsigned char *zoneHeader = zone->itemData[0];
  
  crashZoneHeader.worldCount     = getWord(zoneHeader, 0, true);
  for (int lp=0; lp<8; lp++)                   
    crashZoneHeader.world[lp].world = getWord(zoneHeader, 4+(lp*0x40), true);
                                               
  crashZoneHeader.headerColCount = getWord(zoneHeader, 0x204, true);
  crashZoneHeader.sectionCount   = getWord(zoneHeader, 0x208, true);
  crashZoneHeader.entityCount    = getWord(zoneHeader, 0x20C, true);
                                               
  crashZoneHeader.neighborCount  = getWord(zoneHeader, 0x210, true);
  for (int lp=0; lp<8; lp++)                   
    crashZoneHeader.neighbor[lp] = getWord(zoneHeader, 0x214+(lp*4), true);
                                               
  crashZoneHeader.tpageCount     = getWord(zoneHeader, 0x234, true);
  crashZoneHeader.tchunkCount    = getWord(zoneHeader, 0x238, true);
                                               
  for (int lp=0; lp<8; lp++)                   
    crashZoneHeader.tpage[lp]    = getWord(zoneHeader, 0x23C+(lp*4), true);
    
  for (int lp=0; lp<32; lp++)
    crashZoneHeader.tchunk[lp]   = getWord(zoneHeader, 0x25C+(lp*4), true);
    
  crashZoneHeader.gameFlags      = getWord(zoneHeader, 0x2DC, true);
  crashZoneHeader.vramFillHeight = getWord(zoneHeader, 0x2E0, true);
  crashZoneHeader.unknownA       = getWord(zoneHeader, 0x2E4, true);
  crashZoneHeader.visibilityDepth= getWord(zoneHeader, 0x2E8, true);
  crashZoneHeader.unknownB       = getWord(zoneHeader, 0x2EC, true);
  crashZoneHeader.unknownC       = getWord(zoneHeader, 0x2F0, true);
  crashZoneHeader.unknownD       = getWord(zoneHeader, 0x2F4, true);
  crashZoneHeader.unknownE       = getWord(zoneHeader, 0x2F8, true);
  crashZoneHeader.flags          = getWord(zoneHeader, 0x2FC, true);
  crashZoneHeader.deathY         = getWord(zoneHeader, 0x300, true);  
  crashZoneHeader.unknownF       = getWord(zoneHeader, 0x304, true);
  crashZoneHeader.unknownG       = getWord(zoneHeader, 0x308, true);  
  crashZoneHeader.unknownH       = getWord(zoneHeader, 0x30C, true);
  
  crashZoneHeader.vramFillR      = zoneHeader[0x310];
  crashZoneHeader.vramFillG      = zoneHeader[0x311];
  crashZoneHeader.vramFillB      = zoneHeader[0x312];
  crashZoneHeader.unusedA        = zoneHeader[0x313];
  crashZoneHeader.farColorR      = zoneHeader[0x314];
  crashZoneHeader.farColorG      = zoneHeader[0x315];
  crashZoneHeader.farColorB      = zoneHeader[0x316];
  crashZoneHeader.unusedB        = zoneHeader[0x317];
  
  for (int lp=0; lp<9; lp++)
    crashZoneHeader.objectLightMatrix[lp] = getHword(zoneHeader, 0x318+(lp*2), true);
  
  for (int lp=0; lp<3; lp++)
    crashZoneHeader.objectBackColor[lp] = getHword(zoneHeader, 0x32A+(lp*2), true);
   
  for (int lp=0; lp<9; lp++)
    crashZoneHeader.objectColorMatrix[lp] = getHword(zoneHeader, 0x330+(lp*2), true);
   
  for (int lp=0; lp<3; lp++)
    crashZoneHeader.objectBackColorIntensity[lp] = getHword(zoneHeader, 0x342+(lp*2), true);
    
  for (int lp=0; lp<9; lp++)
    crashZoneHeader.playerLightMatrix[lp] = getHword(zoneHeader, 0x348+(lp*2), true);
  
  for (int lp=0; lp<3; lp++)
    crashZoneHeader.playerBackColor[lp] = getHword(zoneHeader, 0x35A+(lp*2), true);
   
  for (int lp=0; lp<9; lp++)
    crashZoneHeader.playerColorMatrix[lp] = getHword(zoneHeader, 0x360+(lp*2), true);
   
  for (int lp=0; lp<3; lp++)
    crashZoneHeader.playerBackColorIntensity[lp] = getHword(zoneHeader, 0x372+(lp*2), true);
}

void readCrashZoneSpace(entry *zone, crash_zone_space &crashZoneSpace)
{
  unsigned char *zoneCollisions = zone->itemData[1];
  
  crashZoneSpace.X = getWord(zoneCollisions, 0, true);
  crashZoneSpace.Y = getWord(zoneCollisions, 4, true);
  crashZoneSpace.Z = getWord(zoneCollisions, 8, true);

  crashZoneSpace.width = getWord(zoneCollisions, 0xC, true);
  crashZoneSpace.height = getWord(zoneCollisions, 0x10, true);
  crashZoneSpace.depth = getWord(zoneCollisions, 0x14, true);
}
  
void readCrashZoneSection(entry *zone, int section, crash_zone_section &crashZoneSection)
{
  unsigned char *zoneHeader = zone->itemData[0];

  unsigned long headerColCount = getWord(zoneHeader, 0x204, true);
  int sectionIndex = headerColCount + section;
  unsigned char *zoneSection = zone->itemData[sectionIndex];

  readCrashZoneSection(zoneSection, crashZoneSection);
}

void readCrashZoneSection(unsigned char *zoneSection, crash_zone_section &crashZoneSection)
{
  crashZoneSection.slst                 = getWord(zoneSection, 0, true);
  crashZoneSection.parentZone           = getWord(zoneSection, 4, true);
  crashZoneSection.neighborSectionCount = getWord(zoneSection, 8, true);
  
  for (int lp=0; lp<4; lp++)
  { 
    crash_zone_section::neighborSectionDescriptor *neighborSection = &crashZoneSection.neighborSection[lp];
    neighborSection->relation           = zoneSection[0xC+(lp*4)];
    neighborSection->neighborZoneIndex  = zoneSection[0xD+(lp*4)];
    neighborSection->sectionIndex       = zoneSection[0xE +(lp*4)];
    neighborSection->goal               = zoneSection[0xF+(lp*4)];
  }
  
  crashZoneSection.entranceIndex        = zoneSection[0x1C];
  crashZoneSection.exitIndex            = zoneSection[0x1D];
  crashZoneSection.length               = getHword(zoneSection, 0x1E, true);
  crashZoneSection.camMode              = getHword(zoneSection, 0x20, true);
  crashZoneSection.avgNodeDist          = getHword(zoneSection, 0x22, true);
  crashZoneSection.camZoom              = getHword(zoneSection, 0x24, true);
  crashZoneSection.unknownA             = getHword(zoneSection, 0x26, true);
  crashZoneSection.unknownB             = getHword(zoneSection, 0x28, true);
  crashZoneSection.unknownC             = getHword(zoneSection, 0x2A, true);
  crashZoneSection.pathDirectionX       = getHword(zoneSection, 0x2C, true);
  crashZoneSection.pathDirectionY       = getHword(zoneSection, 0x2E, true);
  crashZoneSection.pathDirectionZ       = getHword(zoneSection, 0x30, true);
  
  crashZoneSection.camPath = (crash_zone_section::sectionCamPathNode*)malloc(crashZoneSection.length * sizeof(crash_zone_section::sectionCamPathNode));
  for (int lp=0; lp<crashZoneSection.length; lp++)
  {
    crashZoneSection.camPath[lp].camX    = getHword(zoneSection, 0x32+(lp*12), true);
    crashZoneSection.camPath[lp].camY    = getHword(zoneSection, 0x34+(lp*12), true);
    crashZoneSection.camPath[lp].camZ    = getHword(zoneSection, 0x36+(lp*12), true);
    
    crashZoneSection.camPath[lp].camRotX = getHword(zoneSection, 0x38+(lp*12), true);
    crashZoneSection.camPath[lp].camRotY = getHword(zoneSection, 0x40+(lp*12), true);
    crashZoneSection.camPath[lp].camRotZ = getHword(zoneSection, 0x42+(lp*12), true);
  }
}

void readCrashZoneEntity(entry *zone, int entity, crash_zone_entity &crashZoneEntity)
{
  unsigned char *zoneHeader = zone->itemData[0];

  unsigned long headerColCount = getWord(zoneHeader, 0x204, true);
  unsigned long sectionCount   = getWord(zoneHeader, 0x208, true);

  int entityIndex = (headerColCount + sectionCount) + entity;
  
  unsigned char *zoneEntity = zone->itemData[entityIndex];
  
  readCrashZoneEntity(zoneEntity, crashZoneEntity);
}

void readCrashZoneEntity(unsigned char *zoneEntity, crash_zone_entity &crashZoneEntity)
{
  crashZoneEntity.reserved      = getWord(zoneEntity, 0, true);
  crashZoneEntity.initModeFlags = getHword(zoneEntity, 0x4, true);
  crashZoneEntity.group         = getHword(zoneEntity, 0x6, true);
  crashZoneEntity.ID            = getHword(zoneEntity, 0x8, true);
  crashZoneEntity.pathCount     = getHword(zoneEntity, 0xA, true);
  crashZoneEntity.initFlagsA    = getHword(zoneEntity, 0xC, true);
  crashZoneEntity.initFlagsB    = getHword(zoneEntity, 0xE, true);
  crashZoneEntity.initFlagsC    = getHword(zoneEntity, 0x10, true);
  crashZoneEntity.type          = zoneEntity[0x12];
  crashZoneEntity.subtype       = zoneEntity[0x13];
  
  crashZoneEntity.camPath = (crash_zone_entity::objectCamPathNode*)malloc(crashZoneEntity.pathCount * sizeof(crash_zone_entity::objectCamPathNode));
  for (int lp=0; lp<crashZoneEntity.pathCount; lp++)
  {
    crashZoneEntity.camPath[lp].camX = getHword(zoneEntity, 0x14+(lp*6), true);
    crashZoneEntity.camPath[lp].camY = getHword(zoneEntity, 0x16+(lp*6), true);
    crashZoneEntity.camPath[lp].camZ = getHword(zoneEntity, 0x18+(lp*6), true);
  }
}

unsigned char *getCrashZoneSection(entry *zone, int section)
{  
  unsigned char *zoneHeader = zone->itemData[0];

  unsigned long headerColCount = getWord(zoneHeader, 0x204, true);
  unsigned long sectionCount   = getWord(zoneHeader, 0x208, true);
	
	if (section >= sectionCount)
	  return 0;

	int sectionIndex = (headerColCount) + section;
	return zone->itemData[sectionIndex];
}

unsigned char *getCrashZoneEntity(entry *zone, int entity)
{
  unsigned char *zoneHeader = zone->itemData[0];

  unsigned long headerColCount = getWord(zoneHeader, 0x204, true);
  unsigned long sectionCount   = getWord(zoneHeader, 0x208, true);
	unsigned long entityCount    = getWord(zoneHeader, 0x20C, true);
	
	if (entity >= entityCount)
	  return 0;
	
	int entityIndex = (headerColCount + sectionCount) + entity;
  return zone->itemData[entityIndex];
}