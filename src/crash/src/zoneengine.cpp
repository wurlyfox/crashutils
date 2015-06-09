#include "zoneengine.h"

#include "../crash_system.h"

extern NSD *nsd;

extern unsigned long globals[0x100];

//PROBABLY SHOULD DEFINE THESE HERE AND NOT IN MODELWINDOW.C
entry         *currentZone;
unsigned char *oldZoneHeader;
unsigned char *currentSection;
signed long  currentProgressV;

extern ccamera camera;
extern cvector camRotAfter;
extern cvector camRotBefore;

extern unsigned long states[];

unsigned long flagUnknown;  //gp[0x2C4];

unsigned long var_57964;
unsigned long var_57968;
unsigned long var_5796C;

unsigned long var_5CFC0;  //used in paging system?

//sub_800253A0():          //ldat initB, loads level initial zone
unsigned long initLevel()
{
  //TODO: have nsd struct also record pointer to the loading screen info
  //v0 = *(0x5C538);  //modifies this line
    
  //*(0x61A30) = 0;
  var_61A30 = 0;    
  
  //EID = v0[0];      //this line
  //EID != 0x6396347F
  if (nsd->ldat != EID_NULL)  //and this line
  {
    //processBitList = 0x5FF58;
  
    //gp[0x2E8]  =      0;
    //gp[0x2EC]  =      0;  //*(0x566E8) =      0;
    
    //*(0x6189C) = 0xFFFF;  //(animate display flags to reset with for each frame of intro sequence?)
    //*(0x618B0) = 0xFFFF;  //set global process animate and display flags
    dispAniGlobF = 0xFFFF;
    dispAniGlob  = 0xFFFF;
    
    memset(states, 0, 0x4C0);
    
    //levelID = *(0x5C53C)[4];   //also have NSD record extended info
    unsigned long levelID = nsd->level.LID;
    
    loadStates(levelID);     //and write function for this: set up process bitlist array

    unsigned long initProgress = 0;
    unsigned long zoneEntryAddBit;
    
    /*dont need this right now
    curLevel = *(0x5C52C);
    if (curLevel == 0x11 || curLevel == 0x1E)   //hog wild and whole hog need 2
      zoneEntryAddBit = 2;                       //this is value that increments 
    else                                         //the field[0xA] of the page used when loading the first zone
      zoneEntryAddBit = 1;                       //entry; (perhaps this is for loading more since the zone is travelled faster and therefore more zones need to be loaded per progress)
    */
    
    //we'll set up projection later, this is more gl functions
    //sub_80017790();   //set up projection and screen offset

    /*NO MAIN MENU STUFF YET
    if (curLevel == 0x19 || *(0x618D0) == 0x200 || gp[0x98] == 0)
    {
      //also devise routine in nsd class to 'write' over original header
      //values while still maintaining initial value
      //encodes the string in base 64 format
      *(0x5C53C)[8] = sub_80012660("0b_pz");  
    }*/

    /*NO BONUS/RETURN STUFF YET
    if (gp[0x94] == 1)                  //(return from bonus??)
    {
      *(0x5C53C)[8]   = *(0x57998);    //replace with these zones
      *(0x5C53C)[0xC] = *(0x5799C); 

      gp[0x94] = 0;
     
      s1 = *(0x579A0);
    }*/

    //firstZone = sub_80015118(*(0x5C53C)[8], 0, zoneEntryAddBit);       //T7/ZDAT/GLOBAL/INITIAL ENTITY ENTRY
    
    //*(0x566B0) = 0x6396347F;

    currentZone      = 0;     //level entry?
    currentSection   = 0;     
    currentProgressV = 0;

    //*(0x57968) = 0; //sb
    //*(0x57969) = 0;
    //*(0x5796A) = 0;
    //*(0x5796B) = 0;
    //*(0x57970) = *(0x34520);
 
    //**NO MAIN MENU/DEMO STUFF YET
    //--if coming directly from the main menu? then load demo?
    //if (*(0x618D0) == 0) 
    //  sub_8002EABC(0x566B0);
    
    //firstZoneHeader        = firstZone[0x10];
    //firstSectionIndex      = *(0x5C53C)[0xC];  //NSD file locates index of the first section
    //firstZoneSectionsIndex =    item1[0x204];  //zone indicates in header the index of the start of its section items
    //firstSection = entry[0x10 + ((firstZoneSectionsIndex + firstSectionIndex)*4)];

    //initProgress equivalent to s1
    //updateLevel(firstZone, firstSection, initProgress, false);
    //sub_80015458(0x5C53C + 8, 1, initProgress);
  }
  else
  {
    //*(0x5C53C)[0] = 0;
    nsd->level.magic = 0;
  }

  //**NO SOUND YET EITHER (if these are truly sound routines)**
  //if (*(0x56400) == 0)
  //{
  //  sub_8002EC68(1);
  //  sub_8002EBB4(1);
  //}
  
  //gp[0x98] = 0;  //seems to be main focus of this subsystem
  return GOOL_CODE_SUCCESS;
}

//sub_80025628(zdat)
void initZone(entry *zdat)
{
  unsigned char *zoneHeader = zdat->itemData[0];
  
  unsigned long collisionSkip = getWord(zoneHeader, 0x204, true);
  unsigned long sectionCount  = getWord(zoneHeader, 0x208, true);
  
  if (sectionCount > 0)
  { 
    for (int section = 0; section < sectionCount; section++)
    {
      unsigned long sectionIndex = collisionSkip + section;
      unsigned char *zoneSection = zdat->itemData[sectionIndex];
      
      setWord(zoneSection, 4, (unsigned long)zdat, false);
    }
  }
  
  unsigned long entityCount = getWord(zoneHeader, 0x20C, true);
  
  if (entityCount > 0)
  {    
    for (int entity = 0; entity < entityCount; entity++)
    {
      unsigned long entityIndex = collisionSkip + sectionCount + entity;
      unsigned char *zoneEntity = zdat->itemData[entityIndex];
  
      setWord(zoneEntity, 0, (unsigned long)zdat, false);
    }
  }
}
  
//TODO: sub_800256DC when map implemented
//TODO: sub_80025928 when auto object spawning implemented

//sub_80025A60(zone, section, progressV, flags)
void updateLevel(entry *zone, unsigned char *section, signed long progressV, unsigned long flags)
{
  //var_78 = zone
  //var_70 = section
  //var_68 = progressV
  //var_60 = flag

  if (!zone || !section) { return; }

  unsigned short sectionDepth  = getHword(section, 0x1E, true);
  unsigned short sectionDepthV = sectionDepth << 8;
  
  //restrict the progress value from the input if it does not lie in the section
  if (progressV < (sectionDepthV - 1))
  {
    if (progressV < 0)
      progressV = 0;
  }
  else
    progressV = (sectionDepthV - 1);

  unsigned char *zoneHeader    = zone->itemData[0];
  unsigned long zoneModelCount = getWord(zoneHeader, 0, true);
  
  unsigned long progress        =        progressV >> 8;
  unsigned long currentProgress = currentProgressV >> 8;
 
  //if the zone is not a blank area and contains models 
  if (zoneModelCount != 0)
  {
    if (zone == currentZone && section == currentSection && progress == currentProgress) 
    {
      //wouldn't they be the same though at this point?
      currentProgressV = progressV; 
    }
    //else there is a new zone, new zone section, or change in section progress
    else
    {
      /* we don't care about the display lists quite yet 
      although their format has been cracked...
      
      unsigned long slstEID = getWord(section, 0, true);

      changeProgress = abs((progressV - curProgressV) >> 8);

      if (section == curSection &&
          (progress >= changeProgress && (sectionDepth - (progress + 1)) >= changeProgress))
      {
        startItem = curProgress;
      }
      else
      {
        swap(gp[0x2C0], gp[0x304]);

        sectionCenter = sectionDepth / 2;

        if (progress < sectionCenter)
        {
          itemFirst = slst[0x10];

          gp[0x2BC] = sub_80029B0C(itemFirst, gp[0x2BC], gp[0x304], 1);

          startItem = 0;
        }
        else
        {
          itemLast = slst[0x10 + (sectionDepth*4)];

          gp[0x2BC] = sub_80029B0C(itemLast, gp[0x2BC], gp[0x304], 0);

          startItem = sectionDepth - 1;
        }
      }

      count     = progress - startItem;
      itemIndex = startItem;

      while (count != 0)
      {
        swap(gp[0x2C0], gp[0x304]);

        if (count < 0)
        {
          itemIndex--;
          count++;  
          flag = 0;
        }
        else //count is > 0
        {
          itemIndex++;
          count--;
          flag = 1;    
        }

        item = slst[0x10 + (itemIndex*4)];

        gp[0x2BC] = sub_80029B0C(itemLast, gp[0x2BC], gp[0x304], flag);
      }
      
      sub_80015458(section, 1);
      */
    }
  }
  
  //if the zone has no wgeo models OR there has been a change in zone, section, or progress
  if (zoneModelCount == 0 || 
      zone != currentZone || section != currentSection || progress != currentProgress)
  {
    if (currentZone != zone)
    {
      bool processPages = true;
      
      //*(0x61998) = 0;
      globals[0x43] = 0;
      
      //set by camera routine/camera mode
      //if (*(0x618D0) == 0x600)
      //  processPages = !(flags & 2);
        
      if (currentZone)
      {
        oldZoneHeader                = currentZone->itemData[0];
        unsigned char *newZoneHeader = zone->itemData[0];
        
        processPages = (flags & 2);  

        unsigned long oldNeighborCount = getWord(oldZoneHeader, 0x210, true);     //count of neighboring Zdat entrys? including itself?
        unsigned long newNeighborCount = getWord(newZoneHeader, 0x210, true); 

        for (int countA = 0; countA < oldNeighborCount; countA++)
        {
          //oldNeighborZone = EID_PROCESS(getWord(oldZoneHeader, 0x214+(countA*4), true));   //
          unsigned long EID       = getWord(oldZoneHeader, 0x214 + (countA*4), true);
          entry *oldNeighborZone  = crashSystemPage(EID);
          
          bool match = false;
          for (int countB = 0; countB < newNeighborCount; countB++)
          {
            //entry *newNeighborZone = EID_PROCESS(getWord(newZoneHeader, 0x214+(countB*4), true);
            EID                    = getWord(newZoneHeader, 0x214 + (countB*4), true);
            entry *newNeighborZone = crashSystemPage(EID);
          
            if (newNeighborZone == oldNeighborZone)
            {
              match = true;
              break;
            }
          }
    
          if (match) 
          {
            unsigned char *oldNeighbHeader = oldNeighborZone->itemData[0];
   
            //if zone is loaded then remove it from the system and reset bits
            unsigned long zoneFlags = getWord(oldNeighbHeader, 0x2DC, true);
            if (zoneFlags & 1)
            {
              //sub_8001D200(oldNeighborZone);  //remove it from the system
              setWord(oldNeighbHeader, 0x2DC, true, zoneFlags & 0xFFFFFFFC); //clear bits 1 & 2
            }
          }
        }
      }
          
      if ((flags & 1) == 0)
      {
        for (int count = 0; count < 0x130; count++)
          states[count] &= 0xFFFFFFF9;  //clear bit 2 & 3
      }
          
      currentZone      = zone;
      currentSection   = section;
      currentProgressV = progressV;    
      
      /* can't do any of the following yet; paging system not implemented 
      
      //texture page entry/chunk array routine (gets set up for load to appropriate slots?)
      if (section)
        sub_8001495C(&zoneHeader[0x234], 0);
      else
        sub_8001495C(0, 0);
   
      //load all zone tpage entries
      unsigned long texCount = getWord(zoneHeader, 0x234, true);
      for (int count = 0; count < texCount; count++)
      {
        unsigned long texEID = getWord(zoneHeader, 0x23C+(count*4), true);
        sub_80015118(texEID, 0, 1);
      }

      unsigned long CIDCount = getWord(zoneHeader, 0x238, true);
      for (int count = 0; count < CIDCount; count++)
      {
        unsigned long texCID = getWord(zoneHeader, 0x25C, true);
        sub_80014DD0(texCID, 0, 1, 0x6396347F);
      }

      if (processPages != 0)     //bit 2 from the flags if set
        sub_80013748();          //process ALL pages in the CID list

      */
      
      unsigned long neighborCount = getWord(zoneHeader, 0x210, true);
      
      unsigned char *neighborHeader;
      for (int count = 0; count < neighborCount; count++)
      {
        //neighborZone              = EID_PROCESS(neighborEID);    
        unsigned long EID       = getWord(zoneHeader, 0x214 + (count*4), true);
        entry *neighborZone     = crashSystemPage(EID);
                  
        neighborHeader = neighborZone->itemData[0];
        if ((getWord(neighborHeader, 0x2DC, true) & 1) == 0)  //if bit 1 not set
        {
          var_61A5C = 0;
          var_61A64 = 0;

          var_61A60 = 0x19000;

          setWord(neighborHeader, 0x2DC, true, getWord(neighborHeader, 0x2DC, true) | 3);  //set bits 1 & 2
        }             
        
        if (processPages)
          setWord(neighborHeader, 0x2DC, true, getWord(neighborHeader, 0x2DC, true) | 4);  //set bit 3
        else
          setWord(neighborHeader, 0x2DC, true, getWord(neighborHeader, 0x2DC, true) & 3); //clear bit 3
      }
          
      //use this function on the last neighbor
      updateLevelMisc(neighborHeader + 0x2E0, flags);
    } 
    else
    {
      currentZone      = zone;
      currentSection   = section;
      currentProgressV = progressV;
    }
  }
  
  camRotBefore.X = camera.rot.X;  //copy camera X rotation angle value before sub
  camRotBefore.Y = camera.rot.Y;  //copy camera Y rotation angle value before sub
  camRotBefore.Z = camera.rot.Z;  //copy camera Z rotation angle value before sub

  cameraCalculate(currentSection, -currentProgressV, &camera);  //adjust camera for current zone

  camRotAfter.X = camera.rot.X;   //copy camera X rotation angle value after sub
  camRotAfter.Y = camera.rot.Y;   //copy camera Y rotation angle value after sub
  camRotAfter.Z = camera.rot.Z;   //copy camera Z rotation angle value after sub

  var_5CFC0 = 0;
}

//sub_800260AC
void updateLevelMisc(unsigned char *headerMisc, unsigned long flags)
{
  //*(0x57970) = *(0x34520)
  var_57964 = var_57968 & 0xFFFFFF00;
  var_5796C = (getHword(headerMisc, 0x2C, true) << 8) | getHword(headerMisc, 0x2E, true);
  
  unsigned long zoneFlags = getWord(headerMisc, 0x1C, true);
  globals[0x1E] = zoneFlags; 
  
  if (flags & 4)
    flagUnknown = true;
}

//TODO: sub_80026140 = init routine
//TODO: sub_80026200 = init routine
//TODO: sub_800262DC = level dependent entry load routine

/*
//occupies a struct with information from the current
//zone and specified process
void saveState(object *obj, zoneState *state, bool flag)
{
  //struct = 0x57974
  unsigned char *zoneHeader = curZone->itemData[0];
  
  unsigned long zoneBits = getWord(zoneHeader, 0x2FC, true);
  if (zoneBits & 0x2000) { return; }
  
  state->flag = flag; //0x579A8
  
  //initially we copy the vectors from 
  //crash to struct[0]-struct[0x30]
  //translate, rotate, and scale
  //crash = *(0x566B4);
  //crashVectors  = crash + 0x80;
  //crashVectorsEnd = crash + 0xA0;
  //copy(struct, crashVectors, crashVectorsEnd-crashVectors);
  copyVector(&crash->process.vectors.trans, &state->playerLoc);
  copyEuler(&crash->process.vectors.rot, &state->playerRot);
  copyVector(&crash->process.vectors.scale, &state->playerScale);
  
  
  //if the specified process has bit 10 cleared in field 0xCC
  //then replace struct[0]-struct[8] with its
  //translation vector
  if (obj->process.statusB & 0x200 == 0)
  {
    state->playerLoc.X = obj->process.vectors.trans.X;
    state->playerLoc.Y = obj->process.vectors.trans.Y;
    state->playerLoc.Z = obj->process.vectors.trans.Z;
  }
  
  //if this variable 619A0 is not -1 or 0
  //replace struct[0]-struct[8] with the vector
  //at 61A24
  if (*(0x619A0) != -1 && != 0)
  {
    struct[0] = *(0x61A24);
    struct[4] = *(0x61A28);
    struct[8] = *(0x61A2C);
  }
  
  state->playerRot = { 0, 0, 0 }; //reset angle
  unsigned zoneEID    = curZone->EID;
  state->zoneEID      = zoneEID;
  state->zoneProgress = *(0x57920);
  
  //IS THIS LEVEL ID OR SOMETHING ELSE?
  state->levelID = *(0x5C53C)[4];  
  
  //
  unsigned long zoneCollisionSkip  = getWord(zoneHeader, 0x204, true);
  unsigned long zoneSectionCount   = getWord(zoneHeader, 0x208, true);
  
  curSection = *(0x5791C);
  
  for (int count = 0; count < zoneSectionCount; count++)
  {
    unsigned char *sectionItem = curZone->itemData[zoneCollisionSkip+count];
    if (curSection == sectionItem)
    {
      state->sectionID = count;
      break;
    }
  }
 
  state->boxCount = *(0x61984);
  
  for (int lp = 0; lp < 0x130; lp++)
    state->states = states[lp];
}
*/

//TODO: sub_80026650 = level reinit/restart routine

//find the 'zone' in the list of neighbor zones for the specified zone that
//contains the specified vector/end pt.

//sub_80026DD4
entry *findZone(entry *zone, cpoint *location)
{
  unsigned char *zoneHeader = zone->itemData[0];
  
  //subtracting one; we exclude itself
  unsigned long neighborCount = getWord(zoneHeader, 0x210, true) - 1;
  
  entry *found = 0;
  //work backwards thru specified zone neighbors
  do
  {
    unsigned long neighborEID = getWord(zoneHeader, 0x214+(neighborCount*4), true);
    entry *neighbor           = crashSystemPage(neighborEID);
    
    unsigned char *neighbDimItem = neighbor->itemData[1];
      
    signed long neighbW = CTCVC(getWord(neighbDimItem, 0xC, true));
    signed long neighbH = CTCVC(getWord(neighbDimItem, 0x10, true));
    signed long neighbD = CTCVC(getWord(neighbDimItem, 0x14, true));
    
    signed long neighbX1 = CTCVC(getWord(neighbDimItem, 0, true));
    signed long neighbY1 = CTCVC(getWord(neighbDimItem, 4, true));
    signed long neighbZ1 = CTCVC(getWord(neighbDimItem, 8, true));
    signed long neighbX2 = neighbX1 + neighbW;
    signed long neighbY2 = neighbY1 + neighbH;
    signed long neighbZ2 = neighbZ1 + neighbD;
    
    signed long locX = location->X;
    signed long locY = location->Y;
    signed long locZ = location->Z;
    
    //the conditions pass/loop ends when the zone is found that 
    //the point lies within 
    if (locX > neighbX1 && locY > neighbY1 && locZ > neighbZ1
      &&locX < neighbX2 && locY < neighbY2 && locZ < neighbZ2)
    {
      found = neighbor;   
      break;      
    }
  } while (neighborCount-- > 0);
  
  if (found)
    return found;
    
  //if a neighbor was not found that contains the point
  else
  {
    //then see if it can be found in the current zone's neighbors
    if (zone != currentZone)
    {
      zoneHeader = currentZone->itemData[0];
  
      //subtracting one; we exclude itself
      neighborCount = getWord(zoneHeader, 0x210, true) - 1;
      
      do
      {
        unsigned long neighborEID = getWord(zoneHeader, 0x214+(neighborCount*4), true);
        //entry *neighbor           = EID_PROCESS(neighborEID);
        entry *neighbor           = crashSystemPage(neighborEID);
        
        unsigned char *neighbDimItem = neighbor->itemData[1];
          
        signed long neighbW = CTCVC(getWord(neighbDimItem, 0xC, true));
        signed long neighbH = CTCVC(getWord(neighbDimItem, 0x10, true));
        signed long neighbD = CTCVC(getWord(neighbDimItem, 0x14, true));
        
        signed long neighbX1 = CTCVC(getWord(neighbDimItem, 0, true));
        signed long neighbY1 = CTCVC(getWord(neighbDimItem, 4, true));
        signed long neighbZ1 = CTCVC(getWord(neighbDimItem, 8, true));
        signed long neighbX2 = neighbX1 + neighbW;
        signed long neighbY2 = neighbY1 + neighbH;
        signed long neighbZ2 = neighbZ1 + neighbD;
        
        signed long locX = location->X;
        signed long locY = location->Y;
        signed long locZ = location->Z;
        
        //the conditions pass/loop ends when the zone is found that 
        //the point lies within 
        if (locX > neighbX1 && locY > neighbY1 && locZ > neighbZ1
          &&locX < neighbX2 && locY < neighbY2 && locZ < neighbZ2)
        {
          found = neighbor;   
          break;      
        }
      } while (neighborCount-- > 0);
    
      if (found) { return found; }
    }
  }

  return (entry*)0xFFFFFFE9;
}