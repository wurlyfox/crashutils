#include "cameraengine.h"

#include "zoneengine.h"
#include "objectengine.h"

#include "control.h"

#include "../crash_system.h"

extern NSD *nsd;

extern object *crash;

extern unsigned long globals[0x100];      

extern entry *currentZone;
extern unsigned char *currentSection;
extern signed long currentProgressV;

extern ctrl controls[2];

//note: apparently we remember if we were looking back/forward at the last instant from the previous 
//level; this carries over when we start a new level (i.e neither the flag for or the lookback value 
//is reset when starting a new level)
//the binary has already explicitly initialized the values at their respective memory locations 
//to overcome these issues when 'playing the first level'/first time entering the camera engine routines  
//after a game boot
signed long lookBackZ = -0x12C00;  //gp[0xA8] Z zoom, changed due to moving backwards or forwards in f/b section
signed long zoomZ     =  0x6A400;  //gp[0xAC] Z zoom, changed due to section nearby f/b section scale gp[0xAC]
bool lookBack         = false;     //gp[0xB0] enable zoom for moving backward (corresponds to controller press down, but can be overridden)
bool lookX            = false;     //gp[0xB4] determines camera pan l or r for zones that travel X-wise (corresponds to controller press left/right)
signed long panX      = 0;         //gp[0xBC] X pan, changed due to moving left or right in l/r section
signed long panY      = 0x3E800;   //gp[0xB8] Y pan, changed due to scale of nearby u/d section

signed long camSpeed = 0;     //gp[0x1C4]

ccamera camera;               //0x57864
cvector camRotAfter;          //0x57934
cvector camRotBefore;         //0x57940

//progressV must be a valid (24:8 bit fixed pt) value between 0
//and specified sections depth

//sub_800296A8(section, progressV, cam)
unsigned long cameraCalculate(unsigned char *section, signed long progressV, ccamera *cam)  //item is an item 3 of a t7 (not the first zone though)
{
  entry *parentZone = (entry*)getWord(section, 4, true);
  
  cvector *camTrans = &cam->trans;
  cvector *camRot   = &cam->rot;
    
  progressV = abs(progressV);

  unsigned long progress  = (progressV >> 8);
  unsigned long pathIndex = progress * 12;
  
  #define SECTION_PATHS 0x32
  
  //shouldnt be done this way...
  //entry *parentZone = getWord(section, 4, true); //refers to (next?) T7 entity/zone entry? 
  unsigned char *zoneHeader    = parentZone->itemData[0];
  unsigned char *zoneCollision = parentZone->itemData[1];

  signed short camPathX = getHword(section, 0x32+pathIndex, true);    //lh
  signed short camPathY = getHword(section, 0x34+pathIndex, true);
  signed short camPathZ = getHword(section, 0x36+pathIndex, true);
  unsigned long zoneX   = getWord(zoneCollision, 0, true);  //X
  unsigned long zoneY   = getWord(zoneCollision, 4, true);
  unsigned long zoneZ   = getWord(zoneCollision, 8, true);
  
  //cvector camTransOld;
  //camTransOld.X = camTrans->X;
  //camTransOld.Y = camTrans->Y;
  //camTransOld.Z = camTrans->Z;
  
  camTrans->X = (camPathX + zoneX) << 8;
  camTrans->Y = (camPathY + zoneY) << 8;
  camTrans->Z = (camPathZ + zoneZ) << 8;

  signed short camPathRotY = getHword(section, 0x38+pathIndex, true);
  signed short camPathRotX = getHword(section, 0x3A+pathIndex, true);
  signed short camPathRotZ = getHword(section, 0x3C+pathIndex, true);
  
  camRot->Y = camPathRotY;
  camRot->X = camPathRotX;
  camRot->Z = camPathRotZ;
    
  //the following code allows for calculations for the fractional portions of boundary 
  //camera section progress values/at the edge of the section to account for the 
  //proceeding section's starting point to allow for smooth transition between sections
  bool mode1 = true;
  signed long nextCamPathX, nextCamPathY, nextCamPathZ;
  signed short nextCamPathRotX, nextCamPathRotY, nextCamPathRotZ;
  signed short sectionPathCount = getHword(section, 0x1E, true);
  signed short sectionDepth     = sectionPathCount - 1;

  unsigned short fractional = progressV & 0xFF; //save the fractional bits
  
  unsigned long neighbSectionCount = getHword(section, 0x8, true);
  if ((progress == sectionDepth) && (fractional != 0) && (neighbSectionCount != 0))
  {
    for (int count = 0; count < neighbSectionCount; count++)
    {
      if (section[0xC + (count*4)] & 2)
      {
        unsigned long nsectZoneIndex = section[0xD + (count*4)];
        unsigned long nsectZoneEID   = getWord(zoneHeader, 0x214 + (nsectZoneIndex*4), true);

        //unsigned long nsectZone        = EID_PROCESS(nsectZoneEID);
        entry *nsectZone = crashSystemPage(nsectZoneEID);
        
        unsigned char *nsectZoneHeader = nsectZone->itemData[0];
        unsigned char *nsectZoneCol    = nsectZone->itemData[1];
        unsigned long nsectZoneCSkip   = getWord(nsectZoneHeader, 0x204, true);
        unsigned long nsectIndex       = section[0xE + (count*4)];
        unsigned char *nsection        = nsectZone->itemData[nsectZoneCSkip+nsectIndex];
        
        unsigned long nsectDepth   = getHword(nsection, 0x1E, true);
        unsigned long nsectCamMode = getHword(nsection, 0x20, true);
        if (nsectCamMode != 1)
        {
          //we want to grab the "first" camera path point/node
          //(first or last structurally, dependent on path in front or behind)
          unsigned long nProgFirst;
          
          //if section behind the current
          if ((section[0xF + (count*4)] & 2) == 2)
            nProgFirst = nsectDepth - 1;
          else
            nProgFirst = 0;

          signed short nCamPathX = getHword(nsection, 0x32 + (nProgFirst*12), true);
          signed short nCamPathY = getHword(nsection, 0x34 + (nProgFirst*12), true);
          signed short nCamPathZ = getHword(nsection, 0x36 + (nProgFirst*12), true);
          
          unsigned long nsectZoneX   = getWord(nsectZoneCol, 0, true); 
          unsigned long nsectZoneY   = getWord(nsectZoneCol, 4, true);
          unsigned long nsectZoneZ   = getWord(nsectZoneCol, 8, true);
  
          nextCamPathX = (nsectZoneX + nCamPathX) << 8;
          nextCamPathY = (nsectZoneY + nCamPathY) << 8;
          nextCamPathZ = (nsectZoneZ + nCamPathZ) << 8;
          
          nextCamPathRotY = getHword(nsection, 0x38 + (nProgFirst*12), true);
          nextCamPathRotX = getHword(nsection, 0x3A + (nProgFirst*12), true);
          nextCamPathRotZ = getHword(nsection, 0x3C + (nProgFirst*12), true);
          
          mode1 = false;
        }
        
        break;
      }
    }
  }

  if (mode1)
  {
    //comparez = (scalar >> 8) + 1;
    //z = (item[0x1E] - 1);
    unsigned long newProgress = progress + 1;
    unsigned long sectDepth = getHword(section, 0x1E, true) - 1;
    if (sectDepth < newProgress)
      newProgress = sectDepth;

    //index = (comparez*12);
    //valD1 = item[0x32 + index];    
    //valD2 = item2[0];
    unsigned long newPathIndex = (newProgress*12);
    camPathX = getHword(section, 0x32+newPathIndex, true);    //lh
    camPathY = getHword(section, 0x34+newPathIndex, true);
    camPathZ = getHword(section, 0x36+newPathIndex, true);
    zoneX    = getWord(zoneCollision, 0, true);  //X
    zoneY    = getWord(zoneCollision, 4, true);
    zoneZ    = getWord(zoneCollision, 8, true);
  
    nextCamPathX = (camPathX + zoneX) << 8;
    nextCamPathY = (camPathY + zoneY) << 8;
    nextCamPathZ = (camPathZ + zoneZ) << 8;
  
    nextCamPathRotY = getHword(section, 0x38+newPathIndex, true);
    nextCamPathRotX = getHword(section, 0x3A+newPathIndex, true);
    nextCamPathRotZ = getHword(section, 0x3C+newPathIndex, true);
  }

  //the following calculations allow the fractional portions of boundary camera section 
  //progress values/at the edge of the section to account for the proceeding section's 
  //starting point to allow for smooth transition between sections
  camTrans->X += (((nextCamPathX - camTrans->X) * fractional) >> 8);
  camTrans->Y += (((nextCamPathY - camTrans->Y) * fractional) >> 8);
  camTrans->Z += (((nextCamPathZ - camTrans->Z) * fractional) >> 8);

  //adjust cam rot x, y, z
  camRot->Y += (((nextCamPathRotY - camRot->Y) * fractional) >> 8);
  //result = sub_800245F0(transRotCam[0x10], nextCamPathRotX);
  //transRotCam[0x10] = transRotCam[0x10] + (((result) * fractional) >> 8);
  camRot->X += (((nextCamPathRotX - camRot->X) * fractional) >> 8);
  camRot->Z += (((nextCamPathRotZ - camRot->Z) * fractional) >> 8);
  
  return sectionPathCount;
}

//sub_8002B2BC()
bool cameraUpdate()
{
  if (!crash) { return false; }

  //we use this routine for the camera when crash dies ('woah' sequence)
  //if (dispAniGlob & 0x10000)
  //{
  //  sub_8002BAB4(0x618B4);
  //  return 0;
  //}

  if (dispAniGlob & 2)
  {
    unsigned short camMode = getHword(currentSection, 0x20, true); //lh

    switch (camMode)
    {
      //follow crash 
      case 5:
      case 6:
      {
        globals[0x11] = 0x100;  //0x618D0
     
        cameraFollow(crash, 0);
        
        return 1;
      }
      break;
    
      //no camera
      case 0:
      {
        globals[0x11] = 0x100;  //0x618D0

        return 1;
      }
      break;

      //auto-cam 
      case 1:
      case 3:
      {
        unsigned char *zoneHeader = currentZone->itemData[0];
        unsigned long zoneFlags = getWord(zoneHeader, 0x2FC, true);
        
        //if forceCheck is true, after traversing through neighbor zones,
        //as soon as one is found that is camera mode/case 1 or 3 it is 
        //forcibly loaded
        bool stopTransition = false;
        if  (controls[0].tapState & 0xF0)
        {
          if (zoneFlags & 0x810000 == 0) { stopTransition = true; }
        }
          
        unsigned short sectionDepth = getHword(currentSection, 0x1E, true);
        unsigned long neighborMode = 0;
        
        do
        {
          //we add 1 to get the condition to stop updating and load the next section, this happens since
          //currentProgressV reaches a max of sectionDepth - 1, therefore it will always be less than sectionDepth
          //unless we add 1
          unsigned long currentProgress = (currentProgressV >> 8) + 1;
          
          if (zoneFlags & 0x1000 == 0) { globals[0x11] = 0; }  //0x618D0

          //if the transition hasn't reached the end of the camera path
          //and the transition isn't scheduled to stop, then continue automatically incrementing
          //progress value to do the camera transition
          if ((currentProgress < sectionDepth) && !stopTransition) //lh
          {
            //no new zone to load, handle the current
            updateLevel(currentZone, currentSection, currentProgressV + 0x100, 0);
            return 1;
          }
          else
          {
            unsigned long neighbSectionCount = getWord(currentSection, 0x8, true);
            
            if (neighbSectionCount) 
            {
              unsigned char zoneIndex = currentSection[0xD]; //which of neighbor zones in the current zone's neighbor zone list contains section
              unsigned char sectIndex = currentSection[0xE]; //index of the section in that neighbor zone

              unsigned long neighborEID = getWord(zoneHeader, 0x214+(zoneIndex*4), true);
              
              //neighborZone = EID_PROCESS(neighborEID);
              entry *neighborZone = crashSystemPage(neighborEID);
        
              unsigned char *neighborZoneHeader     = neighborZone->itemData[0];
              unsigned long neighbZoneCollisionSkip = getWord(neighborZoneHeader, 0x204, true);
              unsigned char *neighborSection        = neighborZone->itemData[neighbZoneCollisionSkip + sectIndex];
             
              //first record the camera mode of the neighbor section
              neighborMode = getHword(neighborSection, 0x20, true);
              
              //determine the start point of the next section 
              unsigned long neighborStartV;            
              unsigned short neighborDepth = getHword(neighborSection, 0x1E, true);
              
              if ((currentSection[0xF] & 1) == 0)
                neighborStartV = (neighborDepth - 1) << 8;
              else
                neighborStartV = 0;

              //and update the level to the next section and the start point
              updateLevel(neighborZone, neighborSection, neighborStartV, 0);
            
              //if zone has this bit set, record new state information about the zone
              unsigned long neighborFlags = getWord(neighborZoneHeader, 0x2FC, true);
              
              if ((neighborFlags & 0x1000) == 0)
              {
                //crash = *(0x566B4);     
                //state = *(0x57974);  
                //sub_80026460(crash, states, 1);
              }

              if (!stopTransition) { break; }
            }
            else
              break;
          }
        } while (neighborMode == 3 || neighborMode == 1);

        return true;
      }
   
      /* unimplemented camera modes...
      case 8:

      found = -1;
      count  = 0;
      struct = *(0x5791C);  //a1

      v0 = *(0x61994);

      numOf = struct[8];

      if (numOf != 0)
      {
        //find a zone oppposite the previous' flags?
        a3 = (*(0x61994) & 3) ^ 3;
        
        do
        {
          if (item3[0xC+(count*4)] & 3 == a3) { found = count; break }
          count++;

        } while (count < numOf)
      }


      scaleProgress = (*(0x57920)/256) + 1;

      if (*(0x61994) & 1 == 0) 
      {
        currentZone       =  *(0x57914);
        item3         =  *(0x5791C);
        progress      =  *(0x57920);
        currentZoneHeader = currentZone[0x10];
        scaleProgress = (*(0x57920)/256) - 1;

        //if current progress 'not at the start of the zone'
        if (scaleProgress >= 0)
        {
          progress =  *(0x57920) - 256;  //back one
          sub_80025A60(currentZone, item3, progress, 0);
        }
        else if (found != -1)   
        {
          bitfield       = item3[0xC + (found*4)];  //lbu
          newZoneIndex   = item3[0xD + (found*4)];
          newZoneI3Index = item3[0xE + (found*4)];
          
          newZone = EID_PROCESS(currentZoneHeader[0x214 + (newZoneIndex * 4)]);
          newZoneHeader = newZone[0x10];
          
          newZoneItem3 = newZone[0x10 + ((newZoneI3Index + newZoneHeader[0x204])*4)];
        
          if (bitfield & 1 == 0)
          {
            a2 = (itemN[0x1E] - 1) * 256;
          }
          sub_80025A60();
        }
      }
      else if (val < struct[0x1E]) 
      {  

        a0 =         *(0x57914);
        a1 =             struct;
        a2 = *(0x57920) + 0x100;
        a3 =                  0;

        sub_80025A60();
        
      }
      else if (found != -1) 
      { 

        byteA = struct[0xD + (found*4)];  //lbu
        byteB = struct[0xE + (found*4)];
        byteC = struct[0xF + (found*4)];

        gInfo = *(0x57914)[0x10];
        a0    = gInfo[0x214 + (byteA*4)];

        entry = EID_PROCESS(a0); //a3

        item1 = entry[0x10];
        itemN = entry[0x10 + ((byteB + item1[0x204])*4)];
        
        a0 = entry;
        a1 = itemN;
        a2 = 0;
        a3 = 0;
        if (byteC & 1 == 0)
        {
          a2 = (itemN[0x1E] - 1) * 256;
        }
        sub_80025A60();

      }


      if (struct[0x20] != 8) 
      {
        *(0x61994) = 1;
      }
       
      return 1; 


      ----

      case 7:

      if (*(0x61994) == 0) { return 0; }

      if (*(0x61994) < 0) 
      {
        if (*(0x61994) == -1) { 2b7a4 }
        else                  { 2b7bc }
      }
      else
      {
        if      (*(0x61994) > 7) { 2b7bc; }

        //s3 = 0x22;   
        else if (*(0x61994) < 5) { 2b7bc; } 
       
        else                     { 2b810; }
      }

      2b7a4:

      v0 = *(0x6198C) & 0xFFF;

      s4 =   1;
      s3 = -v0;

      goto 2b810

      --

      2b7bc:

      a0 = *(0x57874);
      a1 = *(0x6198C);
      rval = sub_800245F0();

      v0 = abs(rval);

      if (v0 < 0x17) { s3 = 0; }
      else
      {

        if      (rval  > 0) { s3 = 1; }
        else if (rval <= 0) { s3 = 0; }  //or is it -1 when < 0?
        
      }

      2b810:

      s7 = 0xFFFFFFFF;

      s6 = s4 + 4
      fp = s4 + 3

      struct = *(0x5791C);  //
      a0     = *(0x57920);

      do
      {
        if (s3 == 0) { exit loop to 2ba20; }

        s2 = struct;
        s5 = *(0x57920);

        v0 = *(0x61994);

        if (*(0x61994) == -1)
        {
          offset =  ((*(0x57920)/256)*12;
          a0     = struct[0x3A + offset];
          
          if (s3 >= 0) { a1 = s3; } else { a1 = -s3; }
          rval = sub_80024F50();

          v0 = abs(rval);

          if (v0 < 0x17) { exit loop to 2ba20; }
        }

        if (s6 == 0)
        {
          a0 = *(0x57920) + 256;
        }
        else
        {
          a0 = *(0x57920) + 1024;
        }

        if (a0 < (struct[0x1E]*256))  { 2b9e4; }

        found = -1;
        if (struct[8] != 0)
        {
          count = 0;
          do
          {
            val = struct[0xF + (count*4)];

            if (val == s4 || (found == -1 && (struct[8] == 1 || (val & 3) == fp)))
            { 
              found = count;
            }
                   
            count++;
          } while (count < struct[8])
        }

        if (found == -1)
        {
       
          if (struct[8] != 0)
          {

            a1 = struct[8];
            a0 = struct;

            count = 0;   //v1
            do
            {
              val = struct[0xF + (count*4)];

              if ((val & 4) == 0) { found = count; exit loop; }

              count++;
            } while (count < struct[8])

          }
        
        }

        byteA = struct[0xD + (found*4)];  //a0
        byteB = struct[0xE + (found*4)];
        byteC = struct[0xF + (found*4)]; 
       
        pointer = struct[4];  

        offset = (pointer[0x10] + (byteA * 4));

        EID = pointer[0x214 + offset];

        entry = EID_PROCESS(EID);

        item1 = entry[0x10];
        itemN = entry[0x10 + ((byteB + item1[0x204]) * 4)];  //s1


        if (byteC & 1 == 0)
        {
          a0 = (itemN[0x1E] - 1) * 256;
        }
        else
        {
          a0 = 0;
        }

        if (s6 == 0 && s3 <= 0) 
        { 
          if (struct           !=    itemN) { continue looping; }
          if ((*(0x57920)/256) != (a0/256)) { continue looping; }  
        }

        s2 = itemN;
        s5 = a0;
        
        2b92c:
        exit loop to 2ba20
          
      }
          
      2ba20:

      if (*(0x61994) == -1) { *(0x61994) = s4; }  //this is where 0x61994 is set

      if (itemN != 0)                             //if itemN was not null
      {

        if (itemN == struct)                      //then struct this whole time was just an itemN?
        {
          if (a0 == *(0x57920))   { return 1; }
        }

        a0 = struct[4];  //the pointer
        a1 =    struct;
        a2 =        s5;
        a3 =         0;
        sub_80025A60();
      }

      return 1; */
    }
  }
  
  return false;
}

//sub_8002A82C(obj, flag)
void cameraFollow(object *obj, bool flag)
{  
  unsigned long progress      = currentProgressV >> 8;
  unsigned short sectionDepth = getHword(currentSection, 0x1E, true);
  unsigned long objectY       = obj->process.vectors.trans.Y;
      
  //for smaller sections ( < 0x32 ) both flags should set
  unsigned long progressFlags = 0;
  
  //if at less than halfway but max of 0x32 into section
  if (progress < (sectionDepth/2)  || progress < 0x32)
    progressFlags |= 1;
  
  //if at more than halfway but max of 0x32 from end of section
  if (progress >= (sectionDepth/2) || ((sectionDepth/2)-progress) < 0x32)
    progressFlags |= 2;
    
  unsigned char *zoneHeader = currentZone->itemData[0];
  unsigned long zoneFlags   = getWord(zoneHeader, 0x2FC, true);
  
  //if zone is intended to be travelled forward
  if ((zoneFlags & 0x80) == 0 && (0x31FFF < zoomZ))           
  {
    //if up not pressed
    if ((controls[0].heldState & 0x1000) == 0)
    {
      //and down pressed
      if (controls[0].heldState & 0x4000)   
        lookBack = true;    //moving backward, so look back
    }
    else
        lookBack = false;   //moving forward, so look forward      
  }
  else 
  {
    //cam is always forced to either look forward or back, depending on zone flag
    if ((zoneFlags & 0x8000) == 0) 
      lookBack = false;    
    else
      lookBack = true;
  }
  
  //if left not pressed
  if ((controls[0].heldState & 0x8000) == 0) 
  {
    //and right pressed
    if (controls[0].heldState & 0x2000)    
      lookX = true;           //moving right so look right
  }
  else
    lookX = false;            //else moving left so look left
    
  //adjust the z lookback by either incrementing or decrementing
  //at a rate of 0x3200 [depending on whether we look back or forward]
  //restrict to the range (-0x12C00, 0x12C00)
  if (lookBack)
  {
    if (nsd->level.LID == 3)  //different zoom back cutoff for cortex power
    {
      lookBackZ += 0x3200;
      
      if (lookBackZ > 0x4B000)
        lookBackZ = 0x4B000;
    }
    else
    {
      lookBackZ += 0x3200;
      
      if (lookBackZ > 0x12C00)
        lookBackZ = 0x12C00;
    }
  }
  else
  {
    lookBackZ -= 0x3200;
    
    if (lookBackZ < -0x12C00)
      lookBackZ = -0x12C00;
  }
    
  //get the progress vector in the x direction; 
  //this tells whether the section travels left or right from start to end
  signed short sectionDirX = getHword(currentSection, 0x2C, true);
  
  //if the zone is not to be travelled left/right
  if ((zoneFlags & 0x4000) == 0) { panX = 0; }  //reset the X pan

  //else if no section progress is due to moving left/right 
  else if   (sectionDirX == 0) {  }           //do nothing   
  
  //else modify X pan much like we did Z zoom
  else                                        
  {
    signed long newPanX;
    if (lookX)
    {
      newPanX = panX - 0x6400;
      
      if (sectionDirX < 0)
      {
        if (newPanX < 0xFFFB5000)
          newPanX = 0xFFFB5000;
      }
      else
      {
        if (newPanX < 0)
          newPanX = 0;
      }   
    }
    else
    {
      newPanX = panX + 0x6400;
      
      if (sectionDirX > 0)
      {
        if (0x4B000 < newPanX)
          newPanX = 0x4B000;
      }
      else
      {
        if (newPanX < 0)
          newPanX = 0;        
      }
    }
    
    panX = newPanX;
  }
  
  //=progressFlags determines which half of the current section we reside in
  //-when more than half way into the current section we check only proceeding sections in relation to
  // the current section
  //-when less than half way we check only preceding sections in relation to the current section
  //=we are finding a section that travels forward/backward AND/or a section that travels up/down (if any)
  // and grabbing the last found scale value for the respective section(s).
  //
  //=additionally we check whether the current section travels forward/back AND/or up/down, whose scale
  // value would replace either of the ones found for the respective same direction travelling sections;
  // special flags are set for when the current section travels in either of the requested directions
  
  bool curNoDetY = true;
  bool curNoDetZ = true;
  unsigned long progScaleY = 0;
  unsigned long progScaleZ = 0;
  
  //for each neighboring section to the current section
  unsigned long neighbSectionCount = getWord(currentSection, 8, true);
  for (int count = 0; count < neighbSectionCount; count++)
  { 
    unsigned long offset = count*4; 
    
    unsigned char relation  = currentSection[0xC + offset]; //before or after?
    unsigned char zoneIndex = currentSection[0xD + offset]; //which of neighbor zones in the current zone's neighbor zone list contains section
    unsigned char sectIndex = currentSection[0xE + offset]; //index of the section in that neighbor zone
    
    if (relation & progressFlags)
    {
      unsigned long neighborEID = getWord(zoneHeader, 0x214+(zoneIndex*4), true);
      
      //neighborZone = EID_PROCESS(neighborEID);
      entry *neighborZone = crashSystemPage(neighborEID);
        
      unsigned char *neighborZoneHeader = neighborZone->itemData[0];
      unsigned long neighbZoneCollisionSkip  = getWord(neighborZoneHeader, 0x204, true);
      unsigned char *neighborSection = neighborZone->itemData[neighbZoneCollisionSkip + sectIndex];
      
      //determine y and z components of section direction to see if it is to be travelled in either
      //direction (i.e. how much of progress into the neighbor is determined by Y and Z)
      signed short progDetY = getHword(neighborSection, 0x2E, true);
      signed short progDetZ = getHword(neighborSection, 0x30, true);
      
      unsigned long progScale = getHword(neighborSection, 0x24, true) << 8;
      
      if (progDetY != 0)
        progScaleY = progScale;  
      if (progDetZ != 0)
        progScaleZ = progScale;
    }
    
  }
  
  signed short progDetY = getHword(currentSection, 0x2E, true);
  signed short progDetZ = getHword(currentSection, 0x30, true);
  
  unsigned long progScale = getHword(currentSection, 0x24, true) << 8;
  
  //set flags for if current section travels in either of the requested directions
  if (progDetY != 0)
  {
    curNoDetY = false;
    progScaleY = progScale;
  }
  if (progDetZ != 0)
  {
    curNoDetZ = false;
    progScaleZ = progScale;
  }
  
  //view cam as any other object: has trans, rot, and scale
  //scale is how much the camera zooms for viewing a section and its group of 
  //neighboring sections (different from the group of sections contained within a zone)
 
  //we've calculated the Y and Z components of scale to be the scale values from either
  //the neighbor sections or the current section:
  //we  'zoom' back by the scale value of the nearest forward/backward section
  //and we 'pan' up by the scale value of the nearest up/down section
  
  //if the current or at least one neighbor section has progress determined by z
  //i.e travels forward/backward
  //and we are at least more than 10 units from either end of the section  
  if (progScaleZ != 0 && 
      progress >= 11 &&
      progress < (sectionDepth-10))
  {
    //if its not the current section
    if (curNoDetZ)              
      zoomZ = progScaleZ;    //set zoom to the respective neighbor scale value
    else
      //set zoomZ to gradually approach the scale value
      zoomZ = approach(zoomZ, progScaleZ, 0x1900);
  }
  
  //total camera zoom Z is calculated based on
  //-Z lookback
  //-Z zoomback
  //-desired zoom value for viewing object                                
  signed long camBack    = zoomZ + lookBackZ;  
  signed long totalZoomZ = camBack + obj->process.camZoom; 
    
  //if the current or at least one neighbor section has progress determined by y  
  //i.e. travels up/down
  if (progScaleY != 0)
  {    
    //if it is not the current section 
    if (curNoDetY != 0)
      panY = progScaleY;   //set y pan to the respective neighbor scale value
    else 
      //set the Y pan to gradually/linearly increase approach that scale value
      panY = approach(panY, progScaleY, 0x6400); 
  }
 
  //here allocate space on the stack for 5 'camera structures'; 
  //we will occupy these with information about the view of crash or positioning of the camera
  //in terms the current section and each of its neighboring sections that we approach
  //max 4 neighbor sections, 1 self section; create a cam for each
  ccameraInfo sectionCamera[5]; 
  unsigned long curCam = 0;  //use this iterator to keep index of current cam

  //save the pan and zoom values for the first camera
  sectionCamera[curCam].panX  = panX;   
  sectionCamera[curCam].panY  = panY;   
  sectionCamera[curCam].zoomZ = totalZoomZ;       
  
  bool success = false;
  
  //if some of section progress determined by moving forward/backward
  /*if (progDetZ != 0)
  {
    unsigned long objX = obj->process.vectors.trans.X;
    unsigned long objY = obj->process.vectors.trans.Y;
    unsigned long objZ = obj->process.vectors.trans.Z;
        
    cvector objTrans = { objX, objY, objZ };
    
    success = cameraGetProgressAlt(&objTrans, currentSection, &sectionCamera[curCam], 0, 0);
  }
  else*/
  {
    
    unsigned long objX = obj->process.vectors.trans.X;
    unsigned long objY = obj->process.vectors.trans.Y;
    unsigned long objZ = obj->process.vectors.trans.Z;
        
    cvector objTrans = { objX+panX,objY+panY,objZ+totalZoomZ};
    
    success = cameraGetProgress(&objTrans, currentSection, &sectionCamera[curCam], 0, 0);
  }
  
  //if progress made or (exceed 'max and min progress' and still same item3)
  if (success)
  {
    signed long newProgressV = sectionCamera[curCam].progressV;
    signed long oldProgressV = currentProgressV;
    
    //change in progress
    sectionCamera[curCam].changeProgressV = abs(newProgressV - oldProgressV);
    
    //was the change forward or backward?
    if (newProgressV < oldProgressV)
      sectionCamera[curCam].changeFlags = 1;
    else
      sectionCamera[curCam].changeFlags = 2;
      
    curCam++; //change the camera to a new one, because the current occupied by the cur
    //section which has been found to undergo change
  }
  
  bool sameDirection = false;
  for (int count = 0; count < neighbSectionCount; count++)
  { 
    unsigned long offset = count*4; //neighbor section = 4 byte tag

    if (currentSection[0xF] & 4)
    {
      //elapsed = *(0x60E04);
      //other = *(0x61990);
      
      //if (elapsed - other >= 0x10) { exit to end of loop }
    }
      
    unsigned char relation  = currentSection[0xC + offset]; //before or after?
        
    //progressFlags bit 1 = less than halfway into section
    //progressFlags bit 2 = more than halfway into zone
    //(only two bits that will be set)
      
    //then this check must determine the sections that are
    //-before the current if less less than halfway into it
    // AND/OR
    //-after the current if more than halfway into it   
    if (relation & progressFlags)
    {
      unsigned char zoneIndex = currentSection[0xD + offset]; //which of neighbor zones in the current zone's neighbor zone list contains section
      unsigned char sectIndex = currentSection[0xE + offset]; //index of the section in that neighbor zone

      unsigned long neighborEID = getWord(zoneHeader, 0x214+(zoneIndex*4), true);
      
      //neighborZone = EID_PROCESS(neighborEID);
      entry *neighborZone = crashSystemPage(neighborEID);
      
      unsigned char *neighborZoneHeader = neighborZone->itemData[0];
      unsigned long neighbZoneCollisionSkip  = getWord(neighborZoneHeader, 0x204, true);
      unsigned char *neighborSection = neighborZone->itemData[neighbZoneCollisionSkip + sectIndex];
      
      //how much of progress into the neighbor is determined by Y and Z?
      signed short neighbProgDetY = getHword(neighborSection, 0x2E, true); //lh
      signed short neighbProgDetZ = getHword(neighborSection, 0x30, true);
      signed short progDetY = getHword(currentSection, 0x2E, true);
      signed short progDetZ = getHword(currentSection, 0x30, true);
      
      if ((progDetY == neighbProgDetY) && (progDetZ == neighbProgDetZ))
        sameDirection = true; //t0-
        
      sectionCamera[curCam].panX = panX;   //change in camera RELATIVE TO ZONE due to zone x offset and face l/r offset
      sectionCamera[curCam].panY = panY;   //change in camera RELATIVE TO ZONE due to zone y offset and face f/b offset
      sectionCamera[curCam].zoomZ = totalZoomZ;       
      
      /*
      if (neighbProgDetZ != 0)
      {
        //in the assembly, trans.Y is grabbed at the beginning
        //of the sub-in case it undergoes change beforehand
        unsigned long objX = obj->process.vectors.trans.X;
        unsigned long objY = obj->process.vectors.trans.Y;
        unsigned long objZ = obj->process.vectors.trans.Z;
        
        cvector objTrans = { objX, objY, objZ };
        
        unsigned char minMax = currentSection[0xF + offset]; 
      
        success = cameraGetProgressAlt(&objTrans, neighborSection, &sectionCamera[curCam], minMax, sameDirection);        
      }
      else*/
      {
        unsigned long objX = obj->process.vectors.trans.X;
        unsigned long objY = obj->process.vectors.trans.Y;
        unsigned long objZ = obj->process.vectors.trans.Z;
        
        cvector objTrans = { objX+panX,objY+panY,objZ+totalZoomZ};

        unsigned char minMax = currentSection[0xF + offset];

        success = cameraGetProgress(&objTrans, neighborSection, &sectionCamera[curCam], minMax, sameDirection);
      }
      
      if (success)
      {        
        //examples:
        //   before && behind = most levels
        //   after && infront = most levels
        //   after && behind  = boulders, parts of jaws of darkness, temple ruins 
        //   before && infront = ^^^^^^^^
        //
        
        //*****change in progress = previous distance from exit  pt of 'current' section 
        //*****                   + new      distance from entry pt of     'new' section (or the progressV recorded for it)
        
        unsigned long distExit;        
        //---first calculate the exit point of the current section and our distance from it---                          
        //before this section: calculate how far we are from exiting the start of the current section
        if (currentSection[0xC + offset] & 1)
        {
          sectionCamera[curCam].exitV = 0;  //leaving at the start of the section
          distExit = currentProgressV;       //how far we are from leaving the section   
        }
        //after this section: calculate how far we are from exiting the end of the current section
        else 
        { 
          sectionCamera[curCam].exitV = ((sectionDepth << 8)-1); //leaving at the end of the section
          distExit =  ((sectionDepth << 8)-1) - currentProgressV; //how far we are from leaving the section
        }
        
        //---then calculate the entry point of the new section and our distance from it---
                
        //in front of current section: calculate how far we'd have entered from the front of the new section
        if (currentSection[0xF + offset] & 1)
        {          
          //record point of entry
          sectionCamera[curCam].entranceV       = 0;  
          
          //calculate the change in progress by adding how far we are from leaving the 
          //current section to how far we'd have entered the FRONT of the new section
          sectionCamera[curCam].changeProgressV = distExit + sectionCamera[curCam].progressV + 0x100; //between sections
          
          //current section is behind the new section
          sectionCamera[curCam].relationZ       = 2;
        }
        //behind current section: calculate how far we'd have entered from the back of the new section
        else
        {
          unsigned short neighborDepth = getHword(neighborSection, 0x1E, true);
          
          //record point of entry
          sectionCamera[curCam].entranceV       = ((neighborDepth << 8) - 1);
          
          //calculate the change in progress by adding how far we are from leaving the 
          //current section to how far we'd have entered the BACK of the new section
          sectionCamera[curCam].changeProgressV = distExit + (((neighborDepth << 8) - 1) - sectionCamera[curCam].progressV) + 0x100;
         
           //current section is in front of the new section
          sectionCamera[curCam].relationZ       = 1;
        }
       
        //also save after/before relation as change flags
        sectionCamera[curCam].changeFlags = currentSection[0xC + offset];
      
        //next cam
        curCam++;
      }
    }
  }
      
  if (curCam == 0) { return; }
  
  unsigned long closestDist = 0x7FFFFFFF; //largest pos
  
  ccameraInfo *sigCam = &sectionCamera[0];
  for (int count = 0; count < curCam; count++)
  { 
    unsigned char *section = sectionCamera[count].curSection;
    
    //if the section it refers to is cam mode 1
    if (getHword(section, 0x20, true) == 1) //lh
      break;
           
    //-find closest of the camera structs to crash
    // or at least one that exhibits change
    if ((!sigCam->progressMade && sectionCamera[count].progressMade) ||
        (sigCam->dist < closestDist && 
         sigCam->progressMade == sectionCamera[count].progressMade))
    {
      closestDist = sigCam->dist;
      sigCam      = &sectionCamera[count];
    }
  }
    
  //if no change in progress
  if (sigCam->changeProgressV == 0) { return; }
  
  //else rescale the change
  unsigned long scaleChangeProgV = sigCam->changeProgressV * getHword(currentSection, 0x22, true);
  
  //if small change in progress (dependent on section prog scale)
  if (scaleChangeProgV < 0x7531)  //30001, average approx < 2.9296 change
  {
    unsigned char *section = sigCam->curSection;
    
    entry *sectionZone = (entry*)getWord(section, 4, true);
    //use 
    //- corresponding zone entry for item3 referred to by struct
    //- item 3 referred to by struct
    //- field 8 of the struct is then the zone progress position
    //- (*(0x57920) then would be current zone progress?)
    
    updateLevel(sectionZone, section, sigCam->progressV, 0);
        
    //save cam velocity
    camSpeed = sigCam->changeProgressV;
  }
  else
  {
    //if change <= 2 then halve
    if (sigCam->changeProgressV < 0x201)
    {    
      unsigned long sign = (sigCam->changeProgressV < 0);
      camSpeed = (sigCam->changeProgressV + sign)/2;
    }
    //else if change from 2 to 5, restrict to 2
    else if (sigCam->changeProgressV < 0x500)
    {
      camSpeed = 0x200;
    }
    //else if change > 5
    else
    {
      //speedy scroll for large, impossible values/continuously halve
      unsigned long sign = (sigCam->changeProgressV < 0);
      unsigned long newSpeed = (sigCam->changeProgressV + sign)/2;
      unsigned long maxSpeed = camSpeed + 0x100;
  
      if (newSpeed < maxSpeed)
        newSpeed = maxSpeed;
        
      camSpeed = newSpeed;
    }
    
    adjustCamProgress(camSpeed, sigCam);     

  }
}      
      
void adjustCamProgress(signed long camSpeed, ccameraInfo *cam) //sub_80029F6C
{
  //if forward change in progress
  if (cam->changeFlags & 2)
  {
    //if the cam is at a new section
    if (cam->nextSection)
    {
      signed long newProgressV  = currentProgressV + camSpeed;
      signed long newProgress   = newProgressV >> 8;
      
      unsigned short sectionDepth = getHword(currentSection, 0x1E, true);
      unsigned long sectionDepthV = sectionDepth << 8;
      
      //since the cam is at a new section we *should* exceed the current section depth at 
      //the new progress
      if (newProgress >= sectionDepth)
      {
        signed long exceedProgV = (newProgressV - sectionDepthV) - 1;
        
        //behind it's preceding section
        if ((cam->relationZ & 2) == 0)
        {
          exceedProgV = -exceedProgV;
        }
        
        //so... how is this any different from how we calculated progress originally?
        //maybe this just ensures we don't have any bogus values for larger cam speeds
        signed long minProgV = cam->entranceV + exceedProgV;
        
        entry *nextSectZone = (entry*)getWord(cam->nextSection, 4, true);
        if (minProgV < cam->progressV)
          return updateLevel(nextSectZone, cam->nextSection, minProgV, 0);
        else
          return updateLevel(nextSectZone, cam->nextSection, cam->progressV, 0);
      }
    }
    
    updateLevel(currentZone, currentSection, currentProgressV + camSpeed, 0);
  }
  else
  {
    if (cam->nextSection)
    {
      signed long newProgressV = currentProgressV - camSpeed;
      signed long newProgress  = newProgressV >> 8;
      
      if (newProgressV < 0)
      {
        signed long exceedProgV = (camSpeed - currentProgressV);
        
        //behind it's preceding section
        if ((cam->relationZ & 2) == 0)
          exceedProgV = -exceedProgV;
        
        signed long minProgV = cam->entranceV + exceedProgV;
        
        //entry *nextSectZone = getWord(cam->nextSection, 0x4, true);
                
        entry *nextSectZone = (entry*)getWord(cam->nextSection, 4, true);
        if (cam->progressV < minProgV)
          return updateLevel(nextSectZone, cam->nextSection, minProgV, 0);
        else
          return updateLevel(nextSectZone, cam->nextSection, cam->progressV, 0);
      }
    }
    
    updateLevel(currentZone, currentSection, currentProgressV - camSpeed, 0);
  }
}
    
    
bool cameraGetProgress(cvector *trans, unsigned char *section, ccameraInfo *cam, unsigned long bitfield, bool flag)
{
  entry *sectionZone = (entry*)getWord(section, 4, true);
  
  unsigned char *zoneHeader = sectionZone->itemData[0];
  unsigned char *zoneCol    = sectionZone->itemData[1];
  unsigned long zoneX = getWord(zoneCol, 0, true);
  unsigned long zoneY = getWord(zoneCol, 4, true);
  unsigned long zoneZ = getWord(zoneCol, 8, true);
  
  cam->progressMade = true;
  
  unsigned long transX = trans->X;
  unsigned long transY = trans->Y;
  unsigned long transZ = trans->Z;
 
  //section position/first camera point is stored -relative- to zone
  signed short zoneSectionX = getHword(section, 0x32, true);
  signed short zoneSectionY = getHword(section, 0x34, true);
  signed short zoneSectionZ = getHword(section, 0x36, true);
  
  //calculate actual section position
  unsigned long sectionX = (zoneX + zoneSectionX);
  unsigned long sectionY = (zoneY + zoneSectionY);
  unsigned long sectionZ = (zoneZ + zoneSectionZ);
  
  // << 0 is the normal precision (all bits are integral, no fractional)
  //------------------------------------
  //trans vector             -  <<  8 the normal precision/ 8 fractional bits
  //section/zone coordinates -  <<  0 the normal precision/ 0 fractional bits
  //fixed point              -  << 12 the normal precision/12 fractional bits
  
  //naming scheme for scalars should be: 
  //PV1PV2DIM - PV1 = point or vector relative to PV2 = point or vector component for DIM = dimension
  //(the shifting here allows for simultaneous subtraction and conversion to 4 bit precision)
  signed long transSectionX = (transX >> 4) - (sectionX << 4); //mult of 16
  signed long transSectionY = (transY >> 4) - (sectionY << 4);
  signed long transSectionZ = (transZ >> 4) - (sectionZ << 4);

  //now we have position relative to the section..
  //the next step is to grab the section 'progress determinant vector', a unit vector pointing in the
  //direction that will completely affect the camera when travelled along directly. with this we can 
  //multiply and find the considered section progress in each axis. 
  //**these components are stored as fixed point values**
  signed short progDetX = getHword(section, 0x2C, true);
  signed short progDetY = getHword(section, 0x2E, true);
  signed short progDetZ = getHword(section, 0x30, true);
  
  //note: multiplying fixed point values (precision 12) with precision 4 values kicks the precision to 
  //16 in the result
  signed long progressX = progDetX * transSectionX;
  signed long progressY = progDetY * transSectionY;
  signed long progressZ = progDetZ * transSectionZ;
  
  //the resulting values (16 bit prec.) are to be scaled to vector precision (8 bit) and zone 
  //precision (0 bit) 
  //the purpose is to dot progDet and transSection vectors to get the overall progress value (just
  //add the respective component multiplication results=progress vector) but
  //since one progress value spans a range of different section/zone coordinate values this is further
  //scaled by another value from the section item
  //note that we calculate progress as a signed value since we may lie entirely before the section
  //so we must be able to reflect this (this includes the cast of progScale to signed long since
  //we need to retain the sign of the division)
  unsigned long progScale = getHword(section, 0x22, true);
  signed long progressV = ((progressX + progressY + progressZ) >> 8)/(signed long)progScale;  //t4
  signed long progress  = (progressV) >> 8;  //a1
  
  //we don't need to save the item if is already the current
  if (section == currentSection)
    cam->nextSection = 0;
  else
  {
    //if some X accounts for progress, and -0x3200 down from section
    if (progDetX != 0 && transSectionY < -0x3200) //-3.125
    {
      if (getWord(zoneHeader, 0x2FC, true) & 0x40000) { return 0; }
      //no further adjustments if these flags?
    }
    
    cam->nextSection = section;
  }
  
  unsigned char minProgress = 0;
  unsigned char maxProgress = 0;
  
  if (flag)
  {
    if (bitfield & 1)
      minProgress = section[0x1C];
      
    if (bitfield & 2)
      maxProgress = section[0x1D]; //lb
  }
  
  unsigned short sectionDepth = getHword(section, 0x1E, true);
  
  //if we exceed the min progress
  if (progress >= minProgress)
  {
    //if we exceed the max progress (2a26c)
    if (progress >= (sectionDepth - maxProgress))
    {
      //if the section is not the current then abandon this camera (return false)
      if (section != currentSection)
        return false;
      
      //otherwise force our progress to the end of the section
      progressV = (sectionDepth << 8) - 1;
      progress  = sectionDepth - 1;         
      
      cam->progressMade = false;
    }
  }
  else  //we fall under the min progress
  {    
    //if the section has a min progress (does not default to 0) then abandon this camera
    if (minProgress != 0)
      return false;  

    //if the section is not the current then abandon this camera
    if (section != currentSection)
      return false;
        
    //otherwise force our progress to the start of the section
    progressV = 0;
    progress  = 0;   
      
    cam->progressMade = false;
  }
    
  //progress then determines offset into zone 'camera route' structures
  //this used to calculate the total distance from crash to the camera
  //for the given zone progress
  unsigned long offset = progress * 12;
  
  unsigned long adjustX = (cam->panX + crash->process.vectors.trans.X) >> 8;
  unsigned long adjustY = (cam->panY + crash->process.vectors.trans.Y) >> 8;
  unsigned long adjustZ = (cam->zoomZ + crash->process.vectors.trans.Z) >> 8;
  
  signed short sectionCamX = getHword(section, 0x32 + offset, true);
  signed short sectionCamY = getHword(section, 0x34 + offset, true);
  signed short sectionCamZ = getHword(section, 0x36 + offset, true);
  
  signed long distX = adjustX - (zoneX + sectionCamX);
  signed long distY = adjustY - (zoneY + sectionCamY);
  signed long distZ = adjustZ - (zoneZ + sectionCamZ);
  
  //can't be this far from camera for each dimension
  if (abs(distX) >= 0xC81) { return 0; }
  if (abs(distY) >= 0xC81) { return 0; }
  if (abs(distZ) >= 0xC81) { return 0; }
  
  //save the item3 and progress*256
  cam->curSection= section;
  cam->progressV = progressV;
  
  //and calculate euclidian distance from crash to camera
  unsigned long dist = sqrt((distX * distX) + (distY * distY) + (distZ * distZ));
  cam->dist = dist;
  
  return true;
}
      
//this calculates the progress better for forward/back sections because it factors in the x rotation
//and we need that for forward sections
bool cameraGetProgressAlt(cvector *trans, unsigned char *section, ccameraInfo *cam, unsigned long bitfield, bool flag)
{
  entry *sectionZone = (entry*)getWord(section, 4, true); 
  
  unsigned char *zoneHeader = sectionZone->itemData[0];
  unsigned char *zoneCol    = sectionZone->itemData[1];  
  
  signed long progressV = -1;
  
  //assume progress made
  cam->progressMade = true;
  
  //var_38 = -cam->zoomZ*16;
  
  signed long sectionDepth = getHword(section, 0x1E, true);
 
  //find the closest camera path node to the player; its index is the calculated progress
  if (sectionDepth >= 0)
  {
    unsigned long count   = 0;
    
    signed long prevFovLen      = 0;
    unsigned long prevAbsFovLen = 0;
    
    do
    {
      unsigned long offset = (count*12);
    
      unsigned long transX = trans->X >> 8;
      unsigned long transZ = trans->Z >> 8;
      unsigned long zoneX  = getWord(zoneCol, 0, true);
      unsigned long zoneZ  = getWord(zoneCol, 8, true);
            
      signed short sectCamPathPtX = getHword(section, 0x32 + offset, true);
      signed short sectCamPathPtZ = getHword(section, 0x36 + offset, true);
      
      unsigned long camPathPtX = zoneX + sectCamPathPtX;
      unsigned long camPathPtZ = zoneZ + sectCamPathPtZ;
      
      signed long distCamX = transX - camPathPtX;  //var_88
      signed long distCamZ = transZ - camPathPtZ;  //var_80
      
      signed short camPathPtRotX = getHword(section, 0x3A + offset, true);
       
      double radians = (camPathPtRotX * (PI/0x800));

      signed long distFocusX = FLOAT2FIXED(distCamX * sin(radians));
      signed long distFocusZ = FLOAT2FIXED(distCamZ * cos(radians));
      
      signed long fovLen = (-cam->zoomZ*16) - (distFocusX + distFocusZ);
      signed short sectionDirZ = getHword(section, 0x30, true);
      
      //if we travel forward through the section
      if (sectionDirZ <= 0)
      {
        if (count == 0 && fovLen < 0)
        {
          progressV = 0;
          
          if (fovLen >= -0x1F400)
            cam->progressMade = false;
          
          break;
        }
        else
        {
          unsigned long sectionEnd = getHword(section, 0x1E, true);
          if (count == (sectionEnd - 1) && fovLen > 0)
          {
            if (fovLen < 0x1F400)
              cam->progressMade = false;
              
            progressV = (sectionEnd << 8) - 1;
            
            break;
          }
        }
      }
      else
      {
        if (count == 0 && fovLen > 0)
        {
          progressV = 0;
          
          if (0x1F400 < fovLen)
            cam->progressMade = false;
          
          break;
        }
        else
        {
          unsigned long sectionEnd = getHword(section, 0x1E, true);
          if (count == (sectionEnd - 1) && fovLen < 0)
          {
            if (fovLen < -0x1F400)
              cam->progressMade = false;
              
            progressV = (sectionEnd << 8) - 1;
            
            break;
          }
        }
      }
        
      unsigned long absFovLen = abs(fovLen);
      
      if (count == 0)    
      { 
        prevFovLen    = fovLen;
        prevAbsFovLen = absFovLen;
      }
      //if we found one closer to the camera?
      else if (absFovLen < prevAbsFovLen)      
      {
        
        if ((fovLen ^ prevFovLen) >= 0) { progressV = count << 8; }
        else                            { progressV += (prevAbsFovLen/(prevAbsFovLen + fovLen)) << 8; }
            
        prevFovLen    = fovLen;
        prevAbsFovLen = absFovLen;
      }
      
      count++;
    } while (count < sectionDepth); //i.e. zoneDepth
  }
      
  if (section != currentSection)
    cam->nextSection = section;
  else
    cam->nextSection = 0;
    
  unsigned char sectionStartCut = 0;
  unsigned char sectionEndCut = 0;
  
  if (!flag)
  {
    if (bitfield & 1)      
      sectionStartCut = section[0x1C]; //lb
    if (bitfield & 2)
      sectionEndCut = section[0x1D]; //lb
  }
  else
  {
    sectionStartCut = 0;
    sectionEndCut = 0;
  }
    
  signed long progress = progressV >> 8;
  
  unsigned long sectionStart = sectionStartCut;
  unsigned long sectionEnd   = sectionDepth - sectionEndCut;
  
  if (progress < sectionStart)
  {
    if (sectionStartCut || section != currentSection)          
      return 0; 
      
    progressV = 0;
    cam->progressMade = false;
  }
  else if (progress >= sectionEnd)
  {
    if (sectionEndCut || section != currentSection)         
      return 0;

    progressV = (sectionDepth << 8) - 1;
    cam->progressMade = false;
  }
  
  unsigned long zoneX = getWord(zoneCol, 0, true);
  unsigned long zoneY = getWord(zoneCol, 4, true);
  unsigned long zoneZ = getWord(zoneCol, 8, true);

  unsigned long camX = (crash->process.vectors.trans.X + cam->panX)  >> 8;
  unsigned long camY = (crash->process.vectors.trans.Y + cam->panY)  >> 8;
  unsigned long camZ = (crash->process.vectors.trans.Z + cam->zoomZ) >> 8;
 
  signed short camOffsetX = getHword(section, 0x32 + (progress*12), true); 
  signed short camOffsetY = getHword(section, 0x34 + (progress*12), true);
  signed short camOffsetZ = getHword(section, 0x36 + (progress*12), true);
  
  signed long distX = camX - (zoneX + camOffsetX);  //var_78
  signed long distY = camY - (zoneY + camOffsetY);  //var_74
  signed long distZ = camZ - (zoneZ + camOffsetZ);  //var_70
  
  cam->curSection = section;
  cam->progressV  = progressV;
  
  unsigned long dist = sqrt((distX * distX) + (distY * distY) + (distZ * distZ));
  
  cam->dist = dist;
  
  return 1;
}
