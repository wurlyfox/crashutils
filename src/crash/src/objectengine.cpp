#include <iostream>

//#include <time.h>
#include <math.h>

#include "objectengine.h"

/*OBJECT SYSTEM = ALL CODE FOR OBJECTS INCLUDING:
                  1) HANDLING OF EXECUTION OF 
                    - OBJECT ANIMATION GOOL CODE
                    - 'PER FRAME' GOOL CODE
                  2) DEFAULT OBJECT PHYSICS ROUTINES                  
                  3) ROUTINES TO DISPLAY OBJECT

//for a process of type 3: (non-type 3 will quit the sub)
//1) if crash process exists, read controller data and issue a controller event
//2) remember last process to run this subroutine in a buffer
//
//(bit 4 of 618B0 must be set to run part A)
//A) if bit 16 of 618B0 set then:
       if bit  26 of process field  0xCC
       or bit  18 of process field 0x120 
       then the following check can be skipped:
         for process executable type 0x100, if bit 6 of 618B0
         for process executable type 0x300, if bit 8 of 618B0
         for process executable type 0x400, if bit 12of 618B0
         for process executable type 0x500, if bit 8 of 618B0
         for process executable type 0x600, if bit 8 of 618B0
         for process executable type 0x200, if bit 9 of 618B0 (also if 4==process[0xD4] and header1=4 or 7 then override flag)     
       
       if the check passed (or was skipped) & flag is set
         some additional important stuff (process execution) is done that needs documentation

       decrement process [0x104] counter and clear hi bit of process[0xCC] if counter has reached 0

//B) if process[0x108] field is null
     or process[0xCC] bitfield bit 9 is set 
     or 618B0 bit 3 is clear
     ---
     forget the process that runs this subroutine in the buffer and return
       

//C) if bit 15 of 618B0 set then:
       if bit  26 of process field  0xCC
       or bit  18 of process field 0x120 
       then the following check can be skipped:
         for process executable type 0x100, if bit 5 of 618B0
         for process executable type 0x300, if bit 7 of 618B0
         for process executable type 0x400, if bit 12of 618B0
         for process executable type 0x500, if bit 7 of 618B0
         for process executable type 0x600, if bit 7 of 618B0
         for process executable type 0x200, if bit 10 of 618B0 (also if 4==process[0xD4] and header1=4 or 7 then override flag)     

       if the check passed then sub_8001DE78(process)

       forget the process that runs this subroutine in the buffer and return

--------------------------------------------------------------------*/

#include "../crash_system.h"

extern NSD *nsd;

extern object headObjects[8];
extern object *crash;

extern object *var_60E00;           //0x60E00
extern unsigned long framesElapsed; //0x60E04
spacePair spacePairs[96];           //0x60E08
unsigned long spacePairCount;       //0x61888

cvector zeroVector = { 0, 0, 0 };

unsigned long globalVelocity = 0x11;

extern entry *currentZone;                              //0x57914
unsigned long baseAngle = 0; //setting to 0 for now     //0x57930

traversalInfo zoneTraversal;                            //0x5CFEC

extern ctrl controls[2];

//these are located in scratch memory in the binary
unsigned long wallCache[0x100];
unsigned long wallMap[0x100];

//this is copied to scratch memory during execution of the binary
unsigned long circleMap[32] = {

 0b00000000000011111111000000000000,
 0b00000000001111111111110000000000,
 0b00000000111111111111111100000000,
 0b00000001111111111111111110000000,
 0b00000011111111111111111111000000,
 0b00000111111111111111111111100000,
 0b00001111111111111111111111110000,
 0b00011111111111111111111111111000,
 0b00111111111111111111111111111100,
 0b00111111111111111111111111111100,
 0b01111111111111111111111111111110,
 0b01111111111111111111111111111110,
 0b11111111111111111111111111111111,
 0b11111111111111111111111111111111,
 0b11111111111111111111111111111111,
 0b11111111111111111111111111111111,
 0b11111111111111111111111111111111,
 0b11111111111111111111111111111111,
 0b11111111111111111111111111111111,
 0b11111111111111111111111111111111,
 0b01111111111111111111111111111110,
 0b01111111111111111111111111111110,
 0b00111111111111111111111111111100,
 0b00111111111111111111111111111100,
 0b00011111111111111111111111111000,
 0b00001111111111111111111111110000,
 0b00000111111111111111111111100000,
 0b00000011111111111111111111000000,
 0b00000001111111111111111110000000,
 0b00000000111111111111111100000000,
 0b00000000001111111111110000000000,
 0b00000000000011111111000000000000
 
};

unsigned char sortedDists[304] = { 1,  0,  1, 1,  2, 0,  2, 1,  2, 2,  3, 0,  3, 1,  3, 2,  
                                   4,  0,  4, 1,  3, 3,  4, 2,  5, 0,  4, 3,  5, 1 , 5, 2,         
                                   4,  4,  5, 3,  6, 0,  6, 1,  6, 2,  5, 4,  6, 3 , 7, 0,         
                                   7,  1,  5, 5,  6, 4,  7, 2,  7, 3,  6, 5,  8, 0,  7, 4,         
                                   8,  1,  8, 2,  6, 6,  8, 3,  7, 5,  8, 4,  9, 0,  9, 1,         
                                   7,  6,  9, 2,  8, 5,  9, 3,  9, 4,  7, 7,  8, 6, 10, 0,       
                                   10, 1, 10, 2,  9, 5, 10, 3,  8, 7, 10, 4,  9, 6, 11, 0,     
                                   11, 1, 11, 2, 10, 5,  8, 8,  9, 7, 11, 3, 10, 6, 11, 4,
                                   12, 0,  9, 8, 12, 1, 11, 5, 12, 2, 10, 7, 12, 3, 11, 6,                                            
                                   12, 4,  9, 9, 10, 8, 12, 5, 13, 0, 13, 1, 11, 7, 13, 2,
                                   13, 3, 12, 6, 10, 9, 11, 8, 13, 4, 12, 7, 13, 5, 14, 0,       
                                   14, 1, 10,10, 14, 2, 11, 9, 14, 3, 13, 6, 12, 8, 14, 4,       
                                   13, 7, 14, 5, 11,10, 12, 9, 15, 0, 15, 1, 15, 2, 14, 6,       
                                   13, 8, 15, 3, 15, 4, 11,11, 12,10, 14, 7, 13, 9, 15, 5,       
                                   16, 0, 16, 1, 14, 8, 16, 2, 15, 6, 16, 3, 12,11, 13,10,
                                   16, 4, 15, 7, 14, 9, 16, 5, 12,12, 15, 8, 13,11, 16, 6,
                                   14,10, 16, 7, 15, 9, 13,12, 14,11, 16, 8, 15,10, 16, 9,
                                   13,13, 14,12, 15,11, 16,10, 14,13, 15,12, 16,11, 14,14,
                                   15,13, 16,12, 15,14, 16,13, 15,15, 16,14, 16,15, 16,16  };

cbound testBoxEvent =   { {  -0x9600,       0,   -0x9600 },    //<-9.375, 0, -9.375>
                          {   0x9600,  0x29900,   0x9600 } };  //<9.375, 41.5625, 9.375>
cbound testBoxSurface = { {   -0x640,        0,   -0x640 },
                          {    0x640,  0x29900,    0x640 } };
cbound testBoxZone    = { { -0x12C00, -0x10B80, -0x12C00 },   //<-768, -684.8, -768>
                          {  0x12C00,  0x3A480,  0x12C00 } }; //<768, 2387.2, 768>
cbound testBoxObj     = { {  -0x4B00,        0,  -0x4B00 },
                          {   0x4B00,  0x29900,   0x4B00 } };
cbound testBoxObjTop  = { {  -0x4B00,  0x1F2C0,  -0x4B00 },
                          {   0x4B00,  0x29900,   0x4B00 } };                                       
cbound testBoxCeil    = { {  -0x2580,  0x1F2C0,  -0x2580 },
                          {   0x2580,  0x29900,   0x2580 } };
                                
cmove moveStates[16] = { { 8,     0, 0x100 },
                        { 0, 0x800, 0x100 },
                        { 2, 0x400, 0x100 },
                        { 1, 0x600, 0x147 },
                        { 4,     0, 0x100 },
                        { 8,     0,     0 },
                        { 3, 0x200, 0x147 },
                        { 8,     0,     0 },
                        { 6, 0xC00, 0x100 },
                        { 7, 0xA00, 0x147 },
                        { 8,     0,     0 },
                        { 8,     0,     0 },
                        { 5, 0xE00, 0x147 },
                        { 8,     0,     0 },
                        { 8,     0,     0 },
                        { 8,     0,     0 } };

caccel accelStates[7] = { {        0, 0x7D000,    0,        0 }, 
                          { 0x271000, 0x96000, 0x1E, 0x271000 },
                          { 0x138800, 0x96000, 0x1E,  0x9C400 },
                          { 0x190000, 0xAAE60,  0xF, 0x190000 },
                          { 0x271000, 0xC8000, 0x1E, 0x271000 },
                          { 0x1A0AAA, 0x64000, 0x1E, 0x271000 }, 
                          {  0xD0555, 0x64000, 0x1E,  0x9C400 } };
                                
extern unsigned long globals[0x100];
extern unsigned long drawCount;

//sub_8001D5EC(arg_a0)
unsigned long handleObjects(bool flag)
{
  //--*(0x58408)[0x7C] = offscreen draw/display environment value set to *(0x34520) at init
  //*(0x60E04) = HIWORD(*(0x58408)[0x7C] * 0xF0F0F0F1) >> 5;  
  //*(0x60E04) = *(0x58408)[0x7C] / 34;    //unsigned divide
  framesElapsed = crashSystemTime() / 34;
 
  //*(0x61888) =                     0;
  spacePairCount = 0;
  
  //*(0x619C8) =            *(0x57960);
  //another frame val?
  
  //TODO: NOT IN BINARY, SET ELSEWHERE
  //WE NEED THIS FOR CRASH TO SPIN AND MOVE IN LEVELS 
  //(UNLIKE THE MAPS)
  globals[0x11] = 0x100;
  
  //s0 = 0x60DB8;

   //if (gp[0x2B8] != 0)
  //{
  //count = 0;
     //do
    //{
        //sub_8001B84C(s0, sub_8001DA0C, arg_a0)        //for the 8 processes in the process list
        //s0+=8;
    //} while (count < 8)
  //}
  //if (crash)
  //{
    for (int group = 0; group < 8; group++)
      familyRoutinePBCH(&headObjects[group], handleObject, flag);
  //}
}


extern bool crashToggle;

//sub_8001DA0C(process, flag)
unsigned long handleObject(object *obj, bool flag)
{
  if (obj->subtype != 3) { return 0xFFFFFF01; }

  unsigned char *codeHeader = obj->global->itemData[0];
     
  unsigned long codeID   = getWord(codeHeader, 0, true);
      
  if (obj == crash)
  { 
    setWord(crash->global->itemData[2], 0x24C, 18, true);
  }
  
  
  if (obj == crash) 
    updateControls();        //if we are handling crash read controls

  //this is set at the beginning of this sub, and cleared after it
  //executes successfully; if the per-frame gool routine or main routine
  //for the process returns error, then this will remain as the process
  //that returned the error for this sub will return
  //*(0x60E00) = process; 
  var_60E00 = obj;
  
  //--
  //*(0x618B0) is global primitive type rendering bitfield?
  // -bit 1:
  // -bit 2: camera lock
  // -bit 3: do not animate or display processes
  // -bit 4:
  // -bit 5: display processes of code type 0x100 (player)
  // -bit 6: animate processes of code type 0x100 (player)
  // -bit 7: display processes of code type 0x300, 0x500, and 0x600 (enemys, and etc?)
  // -bit 8: animate processes of code type 0x300, 0x500, and 0x600 
  // -bit 9:  animate process of type 0x200 that has the code slot in nsd of the display process (PAUSE MENU)
  // -bit 10: display process of type 0x200 that has the code slot in nsd of the display process (PAUSE MENU)
  // -bit 11: 
  // -bit 12: animate and display processes of code type 0x400 (sprites)
  // -bit 13:
  // -bit 14: 
  // -bit 15: if set, always display (given bit 3 not set)
  //when 0xCC bit 26 set or header 2 [0x120] has bit 18 set
  // -bit 16: if set, always animate (given bit 3 not set) 
  //            when 0xCC bit 26 set or header 2 [0x120] has bit 18 set
  // -bit 17: alt cam stuff; seems to be for activating the cam used during a crash death
  
  
  bool animate;
  if ((dispAniGlob & 8) == 0) 
    animate = false;                
  else
  {
    if ((obj->process.statusB       & 0x2000000) || 
        (obj->process.routineFlagsA & 0x20000) && (dispAniGlob & 0x8000))
      animate = true;
    else
    { 
      //entry = process[0x20];
      //item1 =   entry[0x10];
      //note that this is not the actual order of the check, as the code is optimized 
      //(i.e. the branching in the assembly is organized by probability)
      //unsigned char *codeHeader = obj->global->itemData[0];
     
      //unsigned long codeID   = getWord(codeHeader, 0, true);
      unsigned long codeType = getWord(codeHeader, 4, true);
      
      if (codeType == 0x200) 
      { 
        animate = dispAniGlob & 0x100;
 
        if (codeID == 4)
        {
          if (obj->process.initSubIndex == codeID || obj->process.initSubIndex == 7)
            animate = true;
        }
      }
      else if (codeType == 0x100) { animate = dispAniGlob & 0x20; }
      else if (codeType == 0x300) { animate = dispAniGlob & 0x80; }      //process executable type
      else if (codeType == 0x400) { animate = dispAniGlob & 0x800;}
      else if (codeType == 0x500) { animate = dispAniGlob & 0x80; }
      else if (codeType == 0x600) { animate = dispAniGlob & 0x80; }
      else                         { animate = false; }
    }
  }
 
  if (flag && animate)
  {
    if (((obj->process.statusB & 0x1000) == 0) || (obj->process.aniCounter == 0)) 
    {
      //set for elapsed frames at THIS point in code 
      //(i.e this portion is reached when process[0x104] counter reaches 0) 
      //process[0xFC] = *(0x60E04); 
      obj->process.aniStamp = framesElapsed;
      
      if (obj->process.statusB & 0x10)
      {
        //if (obj->process.aniStamp == crash->process.aniStamp) 
        //{
          //sub_8001EEA8(process);
        //}
        objectSpace(obj);
      }
    
      //process[0xE8] is then a subroutine to execute for each 'object frame'
      //'object frame' being determined by each time the [0x104] counter 
      //reaches 0
      if (obj->process.subFuncA)  
      {
        //oldsframeOffset = process.getstackframeOffset() - 0x60;
        //oldrframeOffset = process.getreturnframeOffset() - 0x60;
        //oldStack = process.stack;
        //oldFrame = process[0xE4];
        //process.returnframe = oldStack;
    
        unsigned short frameSP = getSP(obj);
        unsigned short frameFP = getFP(obj);
    
        //process.returnframe = process.stack;   //codepointer1, the stack frame to return to
        obj->process.fp = obj->process.sp;
    
        //process.push(0xFFFF);
        //process.push(process.pc);
        //process.push(frameRange(oldrframeOffset, oldsframeOffset));
        push(obj, 0xFFFF);
        push(obj, obj->process.pc);
        push(obj, frameTag(frameFP, frameSP));
        
        #ifdef GOOL_DEBUG
        if (getWord(obj->global->itemData[0], 0, true) == GOOL_DEBUG_OBJTYPE)
        {
          cout << "entering per-frame execution burst") << endl;
          printObject(obj, 0);
        }
        #endif
        
        obj->process.pc = obj->process.subFuncA;
    
        event sub;
        unsigned long statusFlags = 3;
        unsigned long result      = interpret(obj, statusFlags, sub);
        if (isErrorCode(result)) { return result; }
      }
      
      //get the wait frames/time stamp pair/tag from last left off instruction before execution halt from animation instruction
      unsigned long waitStamp = peekback(obj);       
      unsigned long timeStamp = (waitStamp & 0x00FFFFFF);
      unsigned long wait      = (waitStamp & 0xFF000000)>>24;
  
      unsigned long elapsedSince = framesElapsed - timeStamp;
  
      //if more frames have passed since the time required to wait
      if (elapsedSince > wait)
      {
        //get rid of the now useless tag
        pop(obj);
    
        //and continue execution until another animation instruction is reached 
        event sub;
        unsigned long statusFlags = 4;
        unsigned long result      = interpret(obj, statusFlags, sub);
        if (isErrorCode(result)) { return result; }
      } 

      //IMPORTANT: IS FLAG == S1 OR SOMETHING ELSE?
      objectColors(obj);         //s1 was the flag from above;
      objectPhysics(obj, flag);

      //we have to reset the gravity acceleration flag
      obj->process.statusA &= 0xFFFFFFDF;
    }
    else
    {
      //decrement the 'object frame wait' counter
      obj->process.aniCounter--;

      if (obj->process.aniCounter == 0)
        obj->process.statusB &= 0xEFFFFFFF;
    }
  }
    
  //if 
  //    -process has no animation structure,
  //or  -process not displayable
  //or  -some global animation flag not set?

  if ((obj->process.aniSeq == 0)    || 
      (obj->process.statusB & 0x100) || 
      ((dispAniGlob & 4) == 0)) 
  {
    //indicate that no process returned error, it just can't be displayed
    var_60E00 = 0;
    
    //and don't render or display the process
    return GOOL_CODE_SUCCESS;
  }

  bool display;
  if (((obj->process.statusB & 0x2000000) || (obj->process.routineFlagsA & 0x20000))
     && (dispAniGlob & 0x4000))
    display = true;
  else
  {
    unsigned char *codeHeader = obj->global->itemData[0];
     
    unsigned long codeID   = getWord(codeHeader, 0, true);
    unsigned long codeType = getWord(codeHeader, 4, true);

    if (codeType == 0x200) 
    {
      display = dispAniGlob & 0x200;

      if (codeID == 4)
      {
        if (obj->process.initSubIndex == codeID || 
            obj->process.initSubIndex == 7)
          display = true;
      }
    }
    else if (codeType == 0x100) { display = dispAniGlob & 0x10; }
    else if (codeType == 0x300) { display = dispAniGlob & 0x40; }
    else if (codeType == 0x400) { display = dispAniGlob & 0x800;}
    else if (codeType == 0x500) { display = dispAniGlob & 0x40; }      //process executable type
    else if (codeType == 0x600) { display = dispAniGlob & 0x40; }
    else                        { display = false; }
  }

  if (display)
  {
    renderObject(obj);
  }
  
  var_60E00 = 0;
 
  return GOOL_CODE_SUCCESS;
}

//sub_8001D33C(process, scaleVector)
//uses a scale vector and the processes current animation frame (which determines the item in the current svtx entry)/current svtx item to
//determine the bounding volume for the process (whose x1,y1,z1-x2,y2,z2 coordinates are set accordingly)
void objectBound(object *obj, cvector *scale)
{
  if (!obj->process.aniSeq) { return; }

  unsigned char primType = obj->process.aniSeq[0];
  
  if (primType == 4) //lbu
  {
    //struct += 4;
    
    //if (struct & 0xFF800003 != 0x80000000) { return 0; }  //checks validity of struct address?

    //entry = EID_PROCESS(struct);   //EIDorlistEntry at struct[4]

   // if (entry >= 0xFFFFFF01) { return; }

    //struct = process[0x108];
  }

  if (primType == 1) //lbu
  {
    //struct += 4;         
    //entry = EID_PROCESS(struct);           //T1 entry
    unsigned long svtxEID   = getWord(obj->process.aniSeq, 4, true);
    entry *svtx             = crashSystemPage(svtxEID);
    
    //offset = (process[0x10C] >> 8) * 4;    
    //itemN = entry[0x10 + offset];
    unsigned long aniFrame   = (obj->process.aniFrame >> 8);
    unsigned char *svtxFrame = svtx->itemData[aniFrame];  
    
    unsigned long oldXScale = scale->X;
    scale->X = abs(scale->X);   //dont flip in the x direction for negative scale values
   
    cbound bound;
    bound.xyz1.X = getWord(svtxFrame, 0x14, true);
    bound.xyz1.Y = getWord(svtxFrame, 0x18, true);
    bound.xyz1.Z = getWord(svtxFrame, 0x1C, true);
    bound.xyz2.X = getWord(svtxFrame, 0x20, true);
    bound.xyz2.Y = getWord(svtxFrame, 0x24, true);
    bound.xyz2.Z = getWord(svtxFrame, 0x28, true);

    //if crash, then we readjust the box center for collision purposes
    if (obj == crash)  
    {
      cvector center;
      center.X = getWord(svtxFrame, 0x2C, true);
      center.Y = getWord(svtxFrame, 0x30, true);
      center.Z = getWord(svtxFrame, 0x34, true);
    
      bound.xyz1.X += center.X;
      bound.xyz1.Y += center.Y;
      bound.xyz1.Z += center.Z;
      bound.xyz2.X += center.X;
      bound.xyz2.Y += center.Y;
      bound.xyz2.Z += center.Z;
    }
    
    obj->bound.xyz1.X = ((bound.xyz1.X >> 8) * scale->X) >> 4;
    obj->bound.xyz1.Y = ((bound.xyz1.Y >> 8) * scale->Y) >> 4;
    obj->bound.xyz1.Z = ((bound.xyz1.Z >> 8) * scale->Z) >> 4;
    obj->bound.xyz2.X = ((bound.xyz2.X >> 8) * scale->X) >> 4;
    obj->bound.xyz2.Y = ((bound.xyz2.Y >> 8) * scale->Y) >> 4;
    obj->bound.xyz2.Z = ((bound.xyz2.Z >> 8) * scale->Z) >> 4;
  
    scale->X = oldXScale;
  }
  else   //bounding box is a 25x25x25 box, so...
  {    
    unsigned long scaleX = abs(scale->X);
    unsigned long scaleY = abs(scale->Y);
    unsigned long scaleZ = abs(scale->Z);
   
    obj->bound.xyz1.X = -scaleX * 12.5;
    obj->bound.xyz2.X = scaleX * 12.5;
    obj->bound.xyz1.Y = -scaleY * 12.5;
    obj->bound.xyz2.Y = scaleY * 12.5;
    obj->bound.xyz1.Z = -scaleZ * 12.5;
    obj->bound.xyz2.Z = scaleZ * 12.5;

     //set scale vector
    obj->process.vectors.scale.X = scale->X;
    obj->process.vectors.scale.Y = scale->Y;
    obj->process.vectors.scale.Z = scale->Z;
  }
}

unsigned long objectPath(object *obj, unsigned long progressIndex, cvector *trans)
{
  //s2 = process
  //s1 = flag
  //s3 = processVectors
  //entityItem = process[0x110];  //s5
  
  if (!obj || !obj->process.entity) { return 0xFFFFFFF2; }

  unsigned char *entity = obj->process.entity;
  entry *parentEntry = (entry*)getWord(entity, 0, true);

  unsigned char *locationDat;
  unsigned long scale;
  if (parentEntry->type == 7)
  {
    locationDat = parentEntry->itemData[1];    //item 2 of the entity's containing zone has 
    scale = 4;                                 //total camera offset at index 0
  }
  else if (parentEntry->type == 17)
  {
    locationDat = (unsigned char*)&zeroVector;  //points to the zero vector
    scale = 1;
  }

  //grab zone location for entry type 7/zero vector for entry type 17
  cvector zoneLocation;
  zoneLocation.X = getWord(locationDat, 0, true);
  zoneLocation.Y = getWord(locationDat, 4, true);
  zoneLocation.Z = getWord(locationDat, 8, true);
  
  //is val really a ulong or a ushort?
  //val = F00000000000000EDDDDDDDDAAAABBCC

  //sign F seems to be ignored?
  //dbits are index into SVECTOR structs entityItem[0x14]

  //calculate index of specified path node, based on input arg
  unsigned long pathIndex = progressIndex >> 8;
  unsigned long pathCount = getHword(entity, 0xA, true);
  unsigned long pathMaxIndex = pathCount - 1;

  //calculate velocity to travel at specified path node, based on input arg
  unsigned long progressVel;
  if ((pathIndex == pathMaxIndex) && (pathMaxIndex != 0))  //if we want the last direction vector (and there is more than 1)
  {
    progressVel = (progressIndex & 0xFF) + 0x100;
    pathIndex   = pathCount - 2;      //then there is none proceeding it, so choose the previous one
  }
  else
    progressVel = (progressIndex & 0xFF);

  //grab point data for specified path node
  unsigned char *pathNodeDat = &entity[0x14 + (pathIndex * 6)];
  
  csvector pathNode;
  pathNode.X = getHword(pathNodeDat, 0, true);
  pathNode.Y = getHword(pathNodeDat, 2, true);
  pathNode.Z = getHword(pathNodeDat, 4, true);

  //calculate output trans vector based on specified path node relative to zone location 
  trans->X = CTCVC(zoneLocation.X + (pathNode.X * scale)); //also converting to crash vector coordinates
  trans->Y = CTCVC(zoneLocation.Y + (pathNode.Y * scale)); 
  trans->Z = CTCVC(zoneLocation.Z + (pathNode.Z * scale)); 
  
  //if there is only one path node in entity data then return
  if (pathCount == 1) { return 1; }  

  //when there is more than one path node:
   
  //grab the next node after the specified index
  csvector nextNode;
  nextNode.X = getHword(pathNodeDat, 6, true);
  nextNode.Y = getHword(pathNodeDat, 8, true);
  nextNode.Z = getHword(pathNodeDat, 0xA, true);
  
  //calculate next node trans vector relative to zone location
  cvector transNext;
  transNext.X = CTCVC(zoneLocation.X + (nextNode.X * scale)); //also converting to crash vector coordinates
  transNext.Y = CTCVC(zoneLocation.Y + (nextNode.Y * scale)); 
  transNext.Z = CTCVC(zoneLocation.Z + (nextNode.Z * scale));  

  //calculate the vector from current node to next node with subtraction
  cvector travel;
  travel.X = transNext.X - trans->X; 
  travel.Y = transNext.Y - trans->Y;
  travel.Z = transNext.Z - trans->Z;  

  
  if (obj->process.statusB & 0x8000)
  {
    //find the magnitude in the XZ plane for the vector (i.e. the euclidian distance between the two
    //'points' for the vector at the index and proceeding vector)
    //xzDist = sub_80042B9C(pow((var_48 >> 8), 2) + pow((var_40 >> 8), 2)); //pythagorean theorem
    unsigned long xzDist = sqrt((CTGVC(travel.X) * CTGVC(travel.X)) + (CTGVC(travel.Z) * CTGVC(travel.Z)));    
    if (xzDist == 0) { return 0xFFFFFFF2; } //if this is a very small distance then return

    //?
    
    //var_25 = *(0x5647F);
    //var_26 = *(0x56480);
    //var_2A = *(0x5647C);
    //var_2B = *(0x5647D);

    //
    signed long xDistObj = (obj->process.vectors.trans.X - trans->X) >> 4; //v1X = gtX - tX,  x dist from cur node to object 
    signed long xDistNxt = (                 transNext.X - trans->X) >> 4; //v2X = vX  - tX,  x dist from cur node to next node
    signed long dotX     = xDistObj * xDistNxt;                            //<objx, 0> . <stdx, 0> = px  //in gte coords    

    signed long zDistObj = (obj->process.vectors.trans.Z - trans->Z) >> 4; //v1Z = gtZ - tZ  = <0, v1Z> = <0, gtZ> - <0, tZ>
    signed long zDistNxt = (                 transNext.Z - trans->Z) >> 4; //v2Z = vZ  - tZ  = <0, v2Z> = <0,  vZ> - <0, tZ>
    signed long dotZ     = zDistObj * zDistNxt;                            //<0, objz> . <0, stdz> = pz   //in gte coords

    signed long projIntZ =     dotZ / xzDist;  // = pz / ||v|| = (oz / ||v||) * sz  
    signed long projZ    = projIntZ / xzDist;  // = pz / ||v||||v|| = (oz/||v||) * (sz/||v||)

    if (projZ >= 0x100)   //projected z distance
    {
      unsigned long pathIndexP = progressIndex >> 8;

      unsigned long nextIndex = pathIndex + 1;
      if (pathIndexP < pathMaxIndex) //? or is this >= ?
        return objectPath(obj, nextIndex << 8, trans);  //calls the sub again!?
    }

    //v0     = (((var_58 - processVectors[0])/16) * a0/16) / xzDist;
    signed long xVal = (xDistNxt * (projIntZ >> 4)) / xzDist;
    
    //=(pz * (oz / ||v||))/||v|| = (oz/||v||) * (pz/||v||)  
    //var_38 = v0 + v1;  //a1 ( (oz/||v||) * (pz/||v||) + add)
    signed long xValB = abs((xVal + zDistObj) - obj->process.vectors.trans.X);
    
    //v0 = (((var_50 - processVectors[8])/16) * a0/16) / xzDist;
    signed long zVal = (zDistNxt * (projIntZ >> 4)) / xzDist;
    
    //var_30 = v0 + v1;  //v0 ( (oz/||v||) * (pz/||v||) + add)
    signed long zValB = abs((zVal + zDistObj) - obj->process.vectors.trans.Z);
    

    //is this calculating velocity or something
    if (zValB < xValB)
      obj->process.vectors.miscC.Y = ((zValB + ((zValB & 0x80000000) >> 31)) >> 1) + xValB;  //sign adds one if negative
    else
      obj->process.vectors.miscC.Y = ((xValB + ((xValB & 0x80000000) >> 31)) >> 1) + zValB;  //sign adds one if negative

    if (obj->process.vectors.miscC.Y > obj->process._154)
    {
      obj->process.vectors.miscC.Y = obj->process._154;
   
      if (zDistObj != 0 && (xzDist < 0 || xzDist > 0x100) )
        obj->process.statusA |= 0x200;
    }

    /*v1 = (var_50 - processVectors[8])*(process[0x80]>>8);
    a1 = (var_58 - processVectors[0])*(process[0x88]>>8);

    v0 = ((process[0x80])>>8)*(processVectors[8]));
    v1 = (v1 - a1)-v0;

    v0 = ((processVectors[8]>>8)*var_58);
    v1 += v0;

    if (v1 < 0)
    {
      process[0xC0] = -process[0xC0];
    }*/
  }

  //we want to save the previous status before the next potential change
  //to determine if there has actually been a change
  unsigned long prevStatusA = obj->process.statusA;
  
  //-----the change happens here: involves whether the object is
  //     progressing along or against the path it is to travel---------
  
  //if the object is currently travelling with its path
  if (obj->process.pathProgress >= 0) 
  {
    unsigned long pathIndexP = progressIndex >> 8;
    
    //if the requested node from routine args exceeds the path length then
    //we must behave as if travelling against the path
    if (pathIndexP >= pathMaxIndex)
      obj->process.statusA |= 4;
    else  //else travel with the path
      obj->process.statusA &= 0xFFFFFFFB;
  }
  //else the object currently travelling against its path
  else
    obj->process.statusA |= 4;  
  //------------------------------------------------------------------
  
  //has there been a change?: has the object gone from travelling
  //along to against its path or vice-versa? 
  if ((prevStatusA & 4) == (obj->process.statusA & 4))  //IF NOT                             
    obj->process.statusA &= 0xFFFFFFEF;            //clear bit 5
  else //there has been a change
  {
    if (obj->process.statusA & 0x10)      //if bit 5 set: there was previously a change
      obj->process.statusA &= 0xFFFFFFEF; //then clear it
    else
      obj->process.statusA |= 0x10;       //otherwise set it
  }
  
  //for process[0xCC]... if bit 4 set
  if (obj->process.statusB & 4)                     //travelling downwards?
  {

    signed long XZangle;
    float arctan;
    //if bit 2 set (& specified sub index greater than the number of entity translation vectors)
    if ((obj->process.statusB & 2) && (obj->process.statusA & 4))
    {
      arctan = atan2(-travel.X, -travel.Z);     //reverse angle? backwards
      XZangle = CTCA(arctan);
    }                                           //sub uses inverse cosine and definition of dot product
    else                                        //to find angle in the XZ plane between the specified the 
    { 
      arctan = atan2(travel.X, travel.Z);     //specified entity vector and the proceeding one
      XZangle = CTCA(arctan);     //                             
    }
    
    obj->process.vectors.miscB.X = XZangle;                      //set process[0xB4] to the calculated angle 

    if (obj->process.statusB & 0x800)
    {
      arctan = atan2(travel.X, travel.Z);
      obj->process.vectors.rot.Z = CTCA(arctan);  //the angle in its particular quadrant
              
      signed long T;              
      if (abs(travel.Z) < abs(travel.X))                           
        T = abs(travel.X) + (abs(travel.Z) >> 1);
      else
        T = abs(travel.Z) + (abs(travel.X) >> 1); //abs(xdist) + (abs(zdist)/2 + sign(zdist))  //sign returns 1 when neg, 0 when pos

      arctan = atan2(travel.Y, T);
      obj->process.vectors.miscB.Z = -CTCA(arctan);    //this is set to angle downwards
    }
  }
  else                                        //travelling upwards
  {
    if (obj->process.statusB & 0x800)
    {
      float arctan;
      signed long T;              
      if (abs(travel.Z) < abs(travel.X))                           
        T = abs(travel.X) + (abs(travel.Z) >> 1);
      else
        T = abs(travel.Z) + (abs(travel.X) >> 1); //abs(xdist) + (abs(zdist)/2 + sign(zdist))  //sign returns 1 when neg, 0 when pos

      arctan = atan2(travel.Y, T);
      obj->process.vectors.miscB.Z = CTCA(arctan);                      //this is set to angle upwards
      
      arctan = atan2(-travel.X, -travel.Z);
      obj->process.vectors.rot.Z = CTCA(arctan);   //the reversed angle in its particular quadrant (90-angle)
    }
  }

  //then regrab scaled version of path from initial specified vector to proceeding vector
  cvector newTrans;
  newTrans.X = CTGVC(travel.X * progressVel);
  newTrans.Y = CTGVC(travel.Y * progressVel);
  newTrans.Z = CTGVC(travel.Z * progressVel);

  //and add it to the total translation
  trans->X += newTrans.X;
  trans->Y += newTrans.Y;
  trans->Z += newTrans.Z;

  //also return total number of paths
  return pathCount;
}  

//sub_800243A0(angA, angB, val, proc)
signed long objectRotate(signed long angA, signed long angB, signed long speed, object *obj)
{
  unsigned long velocityScale = globalVelocity;
  if (velocityScale > 0x66)
    velocityScale = 0x66;

  signed long velocity = velocityScale * speed;
  
  angA &= 0xFFF;
  angB &= 0xFFF; 
  
  if (velocity < 0)  { velocity = (velocity + 0x3FF) >> 10; }
  else               { velocity = velocity >> 10; }
  
  //absang180 = nearest 180 degree angle between 2 angs
  //we want to calculate the smallest angular distance between the two given angles
  //either the distance in the clockwise OR counter-clockwise direction
  unsigned long absang = abs(angB - angA);
  signed long ang180;
  unsigned long absang180;
  
  if (absang < 0x801)  //are the angs at least 180 degrees apart?
  {
    ang180    = angB - angA;     
    absang180 = abs(ang180);  //then just use this value
  }
  else 
  { 
    if ((angB - angA) > 0)  //else is angB clockwise to angA and > 180 apart?
    {
      ang180    = (angB - angA) - 0x1000;  //grab the counter-clockwise complement
      absang180 = abs(ang180);    
    }
    else                  //else angB is counter-clockwise to angA
    {
      ang180    = (angB - angA) + 0x1000;
      absang180 = abs(ang180);      //grab the clockwise complement
    }
  }
  
  //addtionally do some adjustments to velocity depending 
  //on distance values if this object flag is set
  if (obj && (obj->process.statusB & 0x2000))
  {
    if (absang180 < (velocity * 4))
    {
      if (absang180 >= (velocity * 2))
      {
        if (absang180 >= (velocity * 3))
          velocity = (velocity + ((velocity < 0) ? 1: 0))/2;
        else
          velocity = (velocity + ((velocity < 0) ? 3: 0))/4; //sign involves add 3 if neg
      }
      else if (absang180 >= velocity)
        velocity = (velocity + ((velocity < 0) ? 3: 0))/4; //sign involves add 3 if neg
      else
        velocity = (velocity + ((velocity < 0) ? 7: 0))/8; //sign involves add 7 if neg
    }
  }
  
  //then calculate the new angle based on a rotation at given velocity
  //in the calculated shortest (either clockwise or counter) direction
  if (absang180 < velocity) //if the distance to cover is small,  
  {                         //i.e. less than one unit
    if (obj)
      obj->process.statusA |= 0x800; //consider the object rotated
    
    return angB;            //and return the destination angle
  }
  else
  {
    absang180 = abs(absang180);
    
    //do a small adjustment...
    if (absang180 == 0x800)  //for parallel exact 180 degree separated objects
    {
      if (angB > 0x800)      //favor a clockwise rotation if rotating forward (in z-terms)
        ang180 = -ang180;
    }
    
    //now
    if (ang180 >= 0)
    {  
      if (obj)
        obj->process.statusA &= 0xFFFFFFF7; //clear bit 4 (was rotating counter-clockwise)
          
      return angA + velocity;
    }
    else
    {     
      if (obj)
        obj->process.statusA |= 8;        //set bit 4 (was rotating clockwise)
          
      return angA - velocity;
    }
  }

  return angA;
}  

//sub_80024528(angA, angB, val, process)
signed long objectRotateB(signed long angA, signed long angB, signed long speed, object *obj)
{

  unsigned long velocityScale = globalVelocity;
  if (velocityScale > 0x66)
    velocityScale = 0x66;

  signed long velocity = velocityScale * speed;
  
  angA &= 0xFFF;
  angB &= 0xFFF; 
  
  if (velocity < 0)  { velocity = (velocity + 0x3FF) >> 10; }
  else               { velocity = velocity >> 10; }
  
  //absang180 = nearest 180 degree angle between 2 angs
  //we want to calculate the smallest angular distance between the two given angles
  //either the distance in the clockwise OR counter-clockwise direction
  unsigned long absang = abs(angB - angA);
  signed long ang180;
  unsigned long absang180;
  
  if (absang < 0x801)  //are the angs at least 180 degrees apart?
  {
    ang180    = angB - angA;     
    absang180 = abs(ang180);  //then just use this value
  }
  else 
  { 
    if ((angB - angA) > 0)  //else is angB clockwise to angA and > 180 apart?
    {
      ang180    = (angB - angA) - 0x1000;  //grab the counter-clockwise complement
      absang180 = abs(ang180);    
    }
    else                  //else angB is counter-clockwise to angA
    {
      ang180    = (angB - angA) + 0x1000;
      absang180 = abs(ang180);      //grab the clockwise complement
    }
  }
  
  if (ang180 == 0)  //if the distance to cover is nonexistant
  {
    if (obj)
      obj->process.statusA |= 0x800; //consider the object rotated
    
    return angB;            //and return the destination angle
  }
  else
  {
    if ((absang180 >= abs(velocity)) || ((ang180 ^ velocity) == 0))
      return angA + velocity;
    else
    {
      if (obj)
        obj->process.statusA |= 0x800; //consider the object rotated
      
      return angB;            //and return the destination angle
    }
  }
}  

//sub_80024628          
signed long approach(signed long oldVal, signed long newVal, signed long maxVal)
{
  signed long diffVal = newVal - oldVal;

  if (maxVal > 0)
  {
    if (abs(diffVal) < maxVal) { return newVal; }
  }

  return ((diffVal > 0) ? (oldVal + maxVal) : (oldVal - maxVal));
}

/*
unsigned long objectRotateC(unsigned long angA, unsigned long angB, unsigned long speed)
{
  unsigned long absdist;
  
  long dist = angA - angB;
  if (speed > 0)
  {
    absdist = (dist < 0) ? -dist : dist;
    
    if (absdist < speed) { return angB; }
  }
  
  if (dist > 0)
    return (angA + speed);
  else
    return (angA - speed);
}
*/

signed long leastAngDist(signed long angA, signed long angB)
{
  angA &= 0xFFF;
  angB &= 0xFFF;
  
  long dist = angB - angA;
  
  unsigned long absdist = (dist < 0) ? -dist : dist;
  
  if (absdist < 0x801)
    return dist;
  else 
  { 
    if (dist > 0)
      return (dist - 0x1000);
    else
      return (dist + 0x1000);
  }
}

void objectSpace(object *obj)
{
  spacePair *pair = &spacePairs[spacePairCount++];
  pair->obj = obj;
  
  if (obj->process.statusA & 0x8000)              //if bounding box needs recalculating
  {
    objectBound(obj, &obj->process.vectors.scale);
    obj->process.statusA &= 0xFFFF7FFF;
  }
  
  unsigned char primType = obj->process.aniSeq[0];
  if (primType != 1)
  {
    pair->space.xyz1.X = obj->process.vectors.trans.X + obj->bound.xyz1.X;
    pair->space.xyz1.Y = obj->process.vectors.trans.Y + obj->bound.xyz1.Y;
    pair->space.xyz1.Z = obj->process.vectors.trans.Z + obj->bound.xyz1.Z;
    pair->space.xyz2.X = obj->process.vectors.trans.X + obj->bound.xyz2.X;
    pair->space.xyz2.Y = obj->process.vectors.trans.Y + obj->bound.xyz2.Y;
    pair->space.xyz2.Z = obj->process.vectors.trans.Z + obj->bound.xyz2.Z;
  }
  else
  {
    unsigned long svtxEID   = getWord(obj->process.aniSeq, 4, true);
    entry *svtx             = crashSystemPage(svtxEID);

    unsigned long aniFrame   = (obj->process.aniFrame >> 8);
    unsigned char *svtxFrame = svtx->itemData[aniFrame];  
      
    unsigned long angleX = obj->process.vectors.rot.X & 0xFFF;  //s2
    unsigned long scaleX = obj->process.vectors.scale.X;

    cvector spaceLoc;
    
    //if the object is facing forward and is of normal scale
    //then there is no need to transform (scale and rotate)
    //the current model's forward vector, so it is added directly to
    //the translation vector to calculate the new total location of the
    //objects space
    if ((angleX == 0) && (scaleX == 0x1000)) //default scaleX/angleX value
    { 
      spaceLoc.X = obj->process.vectors.trans.X + getWord(svtxFrame, 0x2C, true);
      spaceLoc.Y = obj->process.vectors.trans.Y + getWord(svtxFrame, 0x30, true);
      spaceLoc.Z = obj->process.vectors.trans.Z + getWord(svtxFrame, 0x34, true);
    }
    //otherwise: given the object's tait-bryan angle vector and scale vector,
    //first transform the forward vector for it's current model before adding
    //to the translation vector to calculate the new total position of the
    //object in three space
    else
    {
      //the binary supplies the first argument as a pointer directly to the vector in
      //the item data which cannot be done here because of the difference in endianness
      
      cvector forward = { getWord(svtxFrame, 0x2C, true), 
                          getWord(svtxFrame, 0x30, true), 
                          getWord(svtxFrame, 0x34, true) };
                          
      transform(&forward, 
                &obj->process.vectors.trans, 
                &obj->process.vectors.rot, 
                &obj->process.vectors.scale,
                &spaceLoc);
    }
    
    //using this new position and adding it to the bounding region for
    //the object will calculate the specific region in three-space that the 
    //object will occupy
    //space is calculated/positioned differently based on angle quadrant in XZ plane
    if ((angleX - 0x200) >= 0xC01)
    {
      pair->space.xyz1.X = spaceLoc.X + obj->bound.xyz1.X;
      pair->space.xyz1.Y = spaceLoc.Y + obj->bound.xyz1.Y;
      pair->space.xyz1.Z = spaceLoc.Z + obj->bound.xyz1.Z;  

      pair->space.xyz2.X = spaceLoc.X + obj->bound.xyz2.X;
      pair->space.xyz2.Y = spaceLoc.Y + obj->bound.xyz2.Y;
      pair->space.xyz2.Z = spaceLoc.Z + obj->bound.xyz2.Z;  
    }
    else if ((angleX - 0x200) < 0x600)
    {
      pair->space.xyz1.X = spaceLoc.X + obj->bound.xyz1.Z;
      pair->space.xyz2.X = spaceLoc.X + obj->bound.xyz2.Z;
      pair->space.xyz1.Y = spaceLoc.Y + obj->bound.xyz1.Y;
      pair->space.xyz2.Y = spaceLoc.Y + obj->bound.xyz2.Y;
      pair->space.xyz1.Z = spaceLoc.Z - obj->bound.xyz2.X;
      pair->space.xyz2.Z = spaceLoc.Z - obj->bound.xyz1.X;      
    }
    else if ((angleX - 0x200) < 0xA00)
    {
      pair->space.xyz1.X = spaceLoc.X + obj->bound.xyz1.X;
      pair->space.xyz2.X = spaceLoc.X + obj->bound.xyz2.X;
      pair->space.xyz1.Y = spaceLoc.Y + obj->bound.xyz1.Y;
      pair->space.xyz2.Y = spaceLoc.Y + obj->bound.xyz2.Y;
      pair->space.xyz1.Z = spaceLoc.Z - obj->bound.xyz2.Z;
      pair->space.xyz2.Z = spaceLoc.Z - obj->bound.xyz1.Z;   
    }
    else
    {
      pair->space.xyz1.X = spaceLoc.X - obj->bound.xyz1.Z;
      pair->space.xyz2.X = spaceLoc.X - obj->bound.xyz2.Z;
      pair->space.xyz1.Y = spaceLoc.Y + obj->bound.xyz1.Y;
      pair->space.xyz2.Y = spaceLoc.Y + obj->bound.xyz2.Y;
      pair->space.xyz1.Z = spaceLoc.Z + obj->bound.xyz1.X;
      pair->space.xyz2.Z = spaceLoc.Z + obj->bound.xyz2.X;  
    }
  }
  
  //if rendered the same frame as crash
  //if (obj->process.aniStamp == crash->process.aniStamp)
  //{
  
    /*
    cspace crashSpace = { { crash->process.vectors.trans.X + crash->bound.xyz1.X,
                            crash->process.vectors.trans.Y + crash->bound.xyz1.Y,
                            crash->process.vectors.trans.Z + crash->bound.xyz1.Z },
                          { crash->process.vectors.trans.X + crash->bound.xyz2.X,
                            crash->process.vectors.trans.Y + crash->bound.xyz2.Y,
                            crash->process.vectors.trans.Z + crash->bound.xyz2.Z } };
                              
                              
    //if crash were to eventually collide with the current object
    if (!isColliding(&crashSpace, &pair->space))
      obj->process.collider = 0;
    else
      objectCollide(obj, &pair->space, crash, &crashSpace);
    */
  //}
  
} 

void getSpace(cvector *location, cbound *bound, cspace *output)
{
  output->xyz1.X = bound->xyz1.X + location->X;
  output->xyz1.Y = bound->xyz1.Y + location->Y;
  output->xyz1.Z = bound->xyz1.Z + location->Z;
  output->xyz2.X = bound->xyz2.X + location->X;
  output->xyz2.Y = bound->xyz2.Y + location->Y;
  output->xyz2.Z = bound->xyz2.Z + location->Z;
}
 
/*

sub_8001EEA8(process)
{
  *(0x61888)++;    //counter field

  offset = *(0x61888) * 28;  //then determines index into array of some 28 byte structs
  struct = 0x60E08[offset];

  //the process associated with the bounding box in this structure
  struct[0x18] = process;
  
  //ok so at 0x60E08 is an array of structures
  //for each call to this routine a new structure is created
  //struct 
  //{
  //  point P1, P2;
  //  object *process;
  //}
  //(P1=x1,y1,y2) - (P2=x2,y2,z2) is the bounding box with which to update 
  //the corresponding process.

  //After all objects have run this sub for a frame, these structures 
  //will probably be processed/eliminated where necessary.. then each
  //object can be updated accordingly
  
  if (process[0xC8] & 0x8000)     //check bit 16
  {
    //uses a scale vector and the processes current animation frame (which determines the item in the current svtx entry)/current svtx item to
    //determine the bounding volume for the process (whose x1,y1,z1-x2,y2,z2 coordinates are set accordingly)
    sub_8001D33C(process, process+0x98);

    process[0xC8] &= 0xFFFF7FFF;  //clear bit 16
  }

  animStruct = process[0x108];
  animMode = animStruct[0];    //lb

  //for animation of type != 1 bounding region is already set
  //so the space occupied by the object is just calculated by adding
  if (animMode != 1)
  { 
    struct[0] = process[0x80] + process[8];       //x translation vector + x1 position
    struct[4] = process[0x84] + process[0xC];     //y translation vector + y1 position
    struct[8] = process[0x88] + process[0x10];    //z translation vector + z1 position

    struct[0xC] = process[0x80] + process[0x14];  //x translation vector + x2 position
    struct[0x10]= process[0x84] + process[0x18];  //y translation vector + y2 position
    struct[0x14]= process[0x88] + process[0x1C];  //z translation vector + z2 position
  }
  //for anim type 1, the bounding region is determined by the current frame
  //which is read from the current svtx entry  
  else
  {
    animEntry = EID_PROCESS(animStruct[4]);                   //holds the entry with the animation data

    animFrame = process[0x10C];
    itemOffset = (animFrame >> 8) * 4;
    animItem = animEntry[0x10 + itemOffset];  //a0

    angleX = process[0x90] & 0xFFF;  //s2
    scaleX = process[0x98];

    //v0 = &var_38
    //if the object is facing forward and is of normal scale
    //then there is no need to transform (scale and rotate)
    //the current model's forward vector, so it is added directly to
    //the translation vector to calculate the new total position of the
    //object in three space    
    if (angleX == 0 && scaleX == 0x1000) //default scaleX/angleX value
    {
      directionX = process[0x80];
      directionY = process[0x84];
      directionZ = process[0x88];
 
      totalAnimDirX = directionX + animItem[0x2C];
      totalAnimDirY = directionY + animItem[0x30];
      totalAnimDirZ = directionZ + animItem[0x34];
 
      var_38 = totalAnimDirX;
      var_34 = totalAnimDirY;
      var_30 = totalAnimDirZ;
    }
    //otherwise: given the object's tait-bryan angle vector and scale vector,
    //first transform the forward vector for it's current model before adding
    //to the translation vector to calculate the new total position of the
    //object in three space
    else
    {
      sub_8002465C(animItem+0x2C, process+0x80, process+0x8C, process+0x98, var_38);
    }
   
    //using this new position and adding it to the bounding region for
    //the object will calculate the specific region in three-space that the 
    //object will occupy
    if (angleX - 0x200 >= 0xC01)
    {
      struct[0] = var_38 + process[8];
      struct[4] = var_34 + process[0xC];
      struct[8] = var_30 + process[0x10];

      struct[0xC]  = var_38 + process[0x14];
      struct[0x10] = var_34 + process[0x18];
      struct[0x14] = var_30 + process[0x1C];
    }
    else if (angleX - 0x200 < 0x600)
    {
      struct[0]   = var_38 + process[0x10];
      struct[0xC] = var_38 + process[0x1C];
      struct[4] =   var_34 + process[0xC];

      struct[0x10]  =var_34 + process[0x18];
      struct[8]    = var_30 - process[0x14];
      struct[0x14] = var_30 - process[8]; 
    }
    else if (angleX - 0x200 < 0xA00)
    {
      struct[0]   = var_38 + process[0x8];
      struct[0xC] = var_38 + process[0x14];
      struct[4]   = var_34 + process[0xC];
     
      struct[0x10]= var_34 + process[0x18];
      struct[8]   = var_30 - process[0x1C];
      struct[0x14]= var_30 - process[0x10];
    }
    else
    {
      struct[0]   = var_38 - process[0x1C];
      struct[0xC] = var_38 - process[0x10];
      struct[4]   = var_34 + process[0xC];
 
      struct[0x10]= var_34 + process[0x18];
      struct[8]   = var_30 + process[0x8];
      struct[0x14] =var_30 + process[0x14];    
    }  }

  crashProcess = gp[0x2B8];

  if (process[0xFC] == crashProcess[0xFC])
  {
    crashDirX = crashProcess[0x80];
    crashDirY = crashProcess[0x84];
    crashDirZ = crashProcess[0x88];

    crashX1 = crashProcess[0x8];
    crashY1 = crashProcess[0xC];
    crashZ1 = crashProcess[0x10];
    crashX2 = crashProcess[0x14];
    crashY2 = crashProcess[0x18];
    crashZ2 = crashProcess[0x1C];

    var_28 = crashDirX + crashX1;
    var_24 = crashDirY + crashY1;
    var_20 = crashDirZ + crashZ1;
    var_1C = crashDirX + crashX2;
    var_18 = crashDirY + crashY2;
    var_14 = crashDirZ + crashZ2;
    
    //if crash were to eventually collide with the current object
    if (sub_80026CA8(&var_28, struct) == 0)
    {
      process[0x78] = 0;
    }
    else
    {
      sub_80024AD4(process, struct, crashProcess, &var_28)
    }
  }
}
     

-------------------------
*/

//determines color intensity of a object/process based on
//-process[0x128] = visibility phase (fading in/out, done fading in/out, terminating, etc)
//-process[0x12C] = frames elapsed at initiation?

unsigned long objectColors(object *obj)
{
  //s0 = process
  //s1 = &process[0x30]  i.e. colors

  // some GOOL macro likely was used to/[for its ultimate preprocessing and subsequent 
  // compilation into instructions that would] set a timestamp in the object field at 
  // 0x12C, such that after the elapsed times as tested in the comparisons for each of
  // the display modes, the appropriate nested operations (including displayMode/state 
  // changes) may take place
  
  // the following can easily be modeled as a state machine, with displayMode as state
  // and transitions corresponding to those that would be taken after the elapsed times
  
  switch(obj->process.displayMode)
  {
    case 6:
    {
      if ((framesElapsed - obj->process._12C) >= 16)
      {
        obj->process.statusB |= 0x80;
        obj->process.displayMode = 0; 
      }
      
      return 0xFFFFFF01;
    }
    
    case 7:
    {
      if (((framesElapsed - obj->process._12C) >= 16)
       || ((obj->process.statusA & 1) == 0)) 
      {
        obj->process.statusB |= 0x80;
        obj->process.displayMode = 0; 
      }
      
      return 0xFFFFFF01;
    }
    
    case 4:
    {
      if ((framesElapsed - obj->process._12C) >= 0x3D)
      {
        //copy the color intensity from the current
        //zone for the object to itself
        
        entry *zone = (obj->zone ? obj->zone: currentZone);
        unsigned char *zoneHeader = zone->itemData[0];
        
        obj->process.displayMode = 0;
      
        unsigned char *zoneColors;
        if (obj == crash)
          zoneColors = &zoneHeader[0x348];
        else
          zoneColors = &zoneHeader[0x318];
        
        //process[0x5A] = zonecols[0x2A];  
        //process[0x5C] = zonecols[0x2C];  
        //process[0x5E] = zonecols[0x2E];
        obj->colors.intensity.R = getHword(zoneColors, 0x2A, true);
        obj->colors.intensity.G = getHword(zoneColors, 0x2C, true);
        obj->colors.intensity.B = getHword(zoneColors, 0x2E, true);
      }
    }
    //notice no break
    
    case 3:
    {
      if (obj->process.displayMode == 3) 
      {
        //elapsed = *(0x60E04); //v0
      
        if ((framesElapsed - obj->process._12C) >= 0x1C4) 
        {
          entry *zone = (obj->zone ? obj->zone: currentZone);
          unsigned char *zoneHeader = zone->itemData[0];

          obj->process.displayMode = 0;
      
          unsigned char *zoneColors;
          if (obj == crash)
            zoneColors = &zoneHeader[0x348];
          else
            zoneColors = &zoneHeader[0x318];
        
          obj->colors.intensity.R = getHword(zoneColors, 0x2A, true);
          obj->colors.intensity.G = getHword(zoneColors, 0x2C, true);
          obj->colors.intensity.B = getHword(zoneColors, 0x2E, true);
          
        }
      }
      else if (obj->process.displayMode == 4 && obj->process.collider)
      {
        //codeEntry  = process[0x20];
        //codeHeader = codeEntry[0x10];
        unsigned char *codeHeader = obj->global->itemData[0];
        
        //codeType = codeHeader[4];
        unsigned long codeType = getWord(codeHeader, 0, true);
        
        if (codeType == 0x300)
        {
          //var_18 = { 0, ? }, or is argument 5 -not- 0 and instead &var_18 
          issueEvent(obj, obj->process.collider, 0xA00, 1, 0);
        }
      }
    }
    
    case 5:
    {
      if (obj->process.displayMode == 5)
      {
        //elapsed = *(0x60E04); //v0
      
        if ((framesElapsed - obj->process._12C) >= 0x25B) 
        {
          entry *zone = (obj->zone ? obj->zone: currentZone);
          unsigned char *zoneHeader = zone->itemData[0];

          obj->process.displayMode = 0;
      
          unsigned char *zoneColors;
          if (obj == crash)
            zoneColors = &zoneHeader[0x348];
          else
            zoneColors = &zoneHeader[0x318];
        
          obj->colors.intensity.R = getHword(zoneColors, 0x2A, true);
          obj->colors.intensity.G = getHword(zoneColors, 0x2C, true);
          obj->colors.intensity.B = getHword(zoneColors, 0x2E, true);
        }
      }
    }
        
    case 2:
    {
      //quarter is one of 4 quarters calculated based on modulus of
      //the 0x57960 counter
      unsigned long quarter = (drawCount >= 0 ? drawCount : (drawCount+3)) % 4;
      unsigned long quarterScale = quarter << 8;
      
      //quarterscale scales the value to an appropriate color
      //also additional calculations determine the fade intensity
      unsigned long fade;
      if ((quarterScale) < 0x100)
        fade = quarterScale + 0x7F;
      else
        fade = 0x47F - quarterScale;
          
      entry *zone = (obj->zone ? obj->zone: currentZone);
      unsigned char *zoneHeader = zone->itemData[0];

      obj->process.displayMode = 0;
      
      unsigned char *zoneColors;
      if (obj == crash)
        zoneColors = &zoneHeader[0x348];
      else
        zoneColors = &zoneHeader[0x318];
       
      //todo: code for write to entry data
      setHword(zoneColors, 0x2A, true, fade);
      setHword(zoneColors, 0x2C, true, fade);
      setHword(zoneColors, 0x2E, true, fade);
      
      return 0xFFFFFF01;
    }
    
    default:
    {
      if (obj == crash)
      {
        if (obj->process.statusB & 0x400)
        {
          entry *zone = (obj->zone ? obj->zone: currentZone);
          unsigned char *zoneHeader = zone->itemData[0];
          unsigned char *zoneColors = &zoneHeader[0x348];
          
          //...seems like the latter byte is ignored in one of the intensity values
          obj->colors.intensity.R = getHword(zoneColors, 0x2A, true);
          obj->colors.intensity.G = getHword(zoneColors, 0x2C, true);
          obj->colors.intensity.B = getHword(zoneColors, 0x2E, true);
        }
      }
      
      return 0xFFFFFF01;
    }
  }
}  



                          
unsigned long objectPhysics(object *obj, bool flag)
{
  unsigned long scale = globalVelocity;
  if (scale > 0x66)
    scale = 0x66;
  
  cmove  *moveState;
  caccel *accelState;

  if (obj->process.statusB & 0x80 /*|| camModeGlob == 0x100*/)
  {
    accelState = &accelStates[0];       //stopped
    
    //moving on the ground
    if (obj->process.routineFlagsA & 4)    
    {
      if (obj->process.statusA & 0x2000) 
        accelState = &accelStates[5];   //starting a bounce on ground?
      else
        accelState = &accelStates[1];   //moving on ground
    }
    
    if (obj->process.displayMode == 5)
      accelState = &accelStates[4];
      
    //moving in the air
    if (obj->process.routineFlagsA & 8)    
    { 
      if (obj->process.statusA & 0x2000)
        accelState = &accelStates[6];   //bouncing in air?
      else
        accelState = &accelStates[2];   //moving in air
    }

    //dunno yet
    if (obj->process.routineFlagsA & 0x10) 
    { 
      accelState = &accelStates[3]; 
    }
    
    //0 = none
    //1 = up     efff
    //2 = right  dfff
    //4 = down   bfff
    //8 = left   7fff
    //controller pressed released state...
    
    unsigned char bitDirection = ((controls[0].heldState >> 12) & 0xF);
    
    //standard angle to travel based on controller U/D/L/R combination

    //mode     = 0x52B64[offset];
    //angXZ    = 0x52B64[offset+4];  //second component is angle in the XZ plane
    //velocity = 0x52B64[offset+8];  //third component is value to scale by (would be velocity in this direction)
            
    moveState = &moveStates[bitDirection];
    
    //EDIT: process[0x124] seems to be velocity along the path for crash to travel
    
    //if we're pressing a valid direction
    if (moveState->mode != 8)  
    {
      //calculate: how much does the desired angle of approach (determined based
      //on move structure determined by controller direction added to a base 
      //angle) deviate from the current angle of approach? (diffangX)
      unsigned long angXZ      = (moveState->angle + baseAngle) & 0xFFF;
      unsigned long approachXZ = obj->process.vectors.miscB.X;
      unsigned long diffangX   = abs(angXZ - approachXZ); 
      
      //given the results of the above calculations, the following code updates
      //speed and possibly the new approach angle of the object 
      
      //when the object is in the air/in a jump, this happens by accumulating 
      //a multiple of the cosine of the difference from current approach angle 
      //to desired approach angle
      
      //in effect this allows the object to speed up when we are already
      //approaching the desired direction, but considerably less the 
      //more perpendicular we are from the desired approach angle, accumulating
      //no speed when we are entirely perpendicular (90 degrees), and eventually
      //slowing down the more we approach completely turning around (180 degrees)
      
      //rotational intertia? wish i payed at least some attention in physics
      
      //this is why we slow down as we turn around, i.e we lose all the speed
      //we once had, gain or lose no speed when turning left or right, and
      //gain speed as we travel in the same direction
      
      //the code additionally ensures that when we -are- turning completely around
      //to factor in additional deceleration 
      
      //for large changes in angle
      if (diffangX > 0x801) //angles start to reverse past 0x800 (i.e. 0x799, 798..)
        diffangX = 0x1000 - diffangX;      //else 1
      
      //if in air
      if (obj->process.routineFlagsA & 8)
      {
        //total acceleration is then determined based on however much a 
        //change from the current angle of approach to desired angle of
        //approach would cause in displacement
        signed long accelV   = (accelState->accel * scale);
        if (accelV < 0)       { accelV = (accelV + 0x3FF) >> 10; }
        else                  { accelV = accelV >> 10; }
          
        double radians = diffangX * (PI/0x800);
        signed long totAccel = ((cos(radians)) * accelV); 
        obj->process.speed  += totAccel;
        
        //if we are now travelling at a low enough speed
        if (obj->process.speed < 0x101)
        { 
          //we can now set the approach angle directly to the desired angle
          obj->process.vectors.miscB.X = angXZ;
        }
        //otherwise if we're basically not turning completely around
        else if (diffangX < 0x7C8)
        {
          //we can smoothly approach the desired angle at speed 0xF00
          obj->process.vectors.miscB.X = objectRotate(obj->process.vectors.miscB.X, angXZ, 0xF00, 0);
        }
        //otherwise we are trying to turn around but we are travelling
        //too fast to do so        
        else
        {          
          //so we must decelerate (in addition to the deceleration already gained
          //by trying to turn around)
          signed long decelV   = (accelState->decel * scale);
          if (decelV < 0)        { decelV = (decelV + 0x3FF) >> 10; }
          else                   { decelV = decelV >> 10; }
          
          //if this deceleration would leave us at a positive speed
          if ((obj->process.speed - decelV) >= 0)
            obj->process.speed -= decelV;   //then decelerate
          else
            obj->process.speed = 0;         //otherwise we've stopped moving
        }
      }
      else
      {
        //if we're travelling slow enough, regardless of the change from 
        //approach angle to desired angle, or that change says we are 
        //trying to rotate at least 90 degrees 
        if ((obj->process.speed < 0x101) || (diffangX < 0x401))
        {
          //then set our approach angle directly to the desired angle
          obj->process.vectors.miscB.X = angXZ;
          
          //and accelerate the appropriate amount
          signed long accelV   = (accelState->accel * scale) >> 10;
          obj->process.speed  += accelV;
        }          
        //otherwise we are travelling fast enough and trying to rotate more than
        //90 degrees
        else
          obj->process.speed = 0;  //so we stop
      }
      
      //terminal velocity?
      if (obj->process.speed > accelState->maxSpeed)
        obj->process.speed = accelState->maxSpeed;
    }
    else
    {
      signed long decelV   = (accelState->decel * scale);
      if (decelV < 0)        { decelV = (decelV + 0x3FF) >> 10; }
      else                   { decelV = decelV >> 10; }  
      
      if ((obj->process.speed - decelV) >= 0)
        obj->process.speed -= decelV;   //decelerate
      else
        obj->process.speed = 0;         //otherwise we've stopped moving
    }    

    //then velocity for components in X/Z directions can be calculated
    //(based on angle of approach, different from facing angle)
    
    //speed      = scaled velocity/directional precision
    //speedScale = 8 bit fixed precision
    //result     = (speed * speedScale) >> 8  //shift 8 bits back to get normal scaled vel precision
    //R2         = (result * (FIXEDPTPREC >> 4)) >> 8  //shift 12 bits back to get normal scaled vel precision
    
    unsigned long sScale = moveState->velocityScale;
    
    unsigned long angXZ = obj->process.vectors.miscB.X;
    unsigned long angval = (obj->process.speed * sScale) >> 8; //speedScale is 8 bit fixed pt fraction
    
    double radians = (angXZ * (PI/0x800));
    
    //we don't shift right 12 here like in the binary 
    //results of sin and cos are fixed pt in the binary
    //[all possible fixed pt numbers in the range -1.0 to 1.0]
    //thus a multiplication by angval would widen that range
    //and a shift right by 12 would convert the widened fixed
    //point result to normal precision, losing the fractional portion
    
    //sin and cos in math.h return double precision values
    //such a sequence of scaling followed by precision conversion 
    //can be replicated without explicit shifting by the implicit 
    //narrowing of double to long types
    obj->process.vectors.miscA.X = sin(radians) * angval;        
    obj->process.vectors.miscA.Z = cos(radians) * angval;        
  }
  
  if (obj->process.statusA & 1)   //toggles on and off on ground
  {
    //what is this check?
    if (obj->process.event != 0x1200)
      obj->process.statusA &= 0xFFFFDFFF; //clear bit 14
  }      
  
  //clear bits 1, 8-13, 15, 17, 19, 21, and 22
  obj->process.statusA &= 0xFFCAA07E;
  
  //here is where the actual 'rotation' occurs
  if (obj->process.statusB & 1)   //set for crash
  {
    //finally rotate object in XZ based on approaching angles
    obj->process.vectors.rot.X = objectRotate(obj->process.vectors.rot.X, obj->process.vectors.miscB.X, obj->process.vectors.miscB.Y, obj);
  }

  if (obj->process.statusB & 0x80000)
  {
    //this does it a different way?
    obj->process.vectors.rot.X = objectRotateB(obj->process.vectors.rot.X, obj->process.vectors.miscB.X, obj->process.vectors.miscB.Y, obj);
  }
  
  if (obj->process.statusB & 0x2000)  //also set for crash
  {
    //rotate object in YZ based on approach angles?
    obj->process.vectors.rot.Y = objectRotate(obj->process.vectors.rot.Y, obj->process.vectors.miscB.Z, obj->process._144, 0);
  }
  

  //if object is not Aku Aku, reset colliding object pointer
  unsigned char *codeHeader = obj->global->itemData[0];     
  unsigned long codeID      = getWord(codeHeader, 0, true);
  
  if (codeID != 5)
    obj->process.collider = 0;
  
  
  if (obj->process.statusB & 0x40)  //also set for crash
  {        
    signed long xVel = (obj->process.vectors.miscA.X * scale);
    signed long yVel = (obj->process.vectors.miscA.Y * scale);
    signed long zVel = (obj->process.vectors.miscA.Z * scale);
    if (xVel < 0) { xVel += 0x3FF; }
    if (yVel < 0) { yVel += 0x3FF; }
    if (zVel < 0) { zVel += 0x3FF; }
    
    cvector velocity = 
    {
      xVel >> 10,
      yVel >> 10,
      zVel >> 10
    };

    cvector trans;
    copyVector(&obj->process.vectors.trans, &trans);
    
    //flag for object is stopped by anything solid
    if (obj->process.statusB & 8)   //set for crash
    {
      obj->process.event = 0xFF;
      
      transSmoothStopAtSolid(obj, &velocity, &zoneTraversal);
    }
    else
    {
      //object is finally translated from its current position by the scaled velocity vector 
      //process[0x80] += var_48;
      //process[0x84] += var_44;
      //process[0x88] += var_40;
      addVector(&velocity, &obj->process.vectors.trans);
    }
    
    //flag for object ground determined by its initial Y position?
    if (obj->process.statusB & 0x8000)  //NOT set for crash
    {
      cvector pathTrans;
      objectPath(obj, 0, &pathTrans);
      
      obj->process._11C = pathTrans.Y;
      
      //flag for: object translation is not affected by velocity
      if (obj->process.statusA & 0x200)
      {
        //process[0x80] = var_38;
        //process[0x84] = var_34;
        //process[0x88] = var_30;        
        copyVector(&trans, &obj->process.vectors.trans);
      }
    }
    
    //unknown (does [0xB8] serve a different purpose in this case)
    //
    if (obj->process.statusB & 0x1000)  //NOT set for crash
    {
      if (obj->process.vectors.miscA.X > obj->process.vectors.miscB.Z)
        obj->process.vectors.miscA.X = obj->process.vectors.miscB.Z;
      else if (obj->process.vectors.miscA.X < -obj->process.vectors.miscB.Z)
        obj->process.vectors.miscA.X = -obj->process.vectors.miscB.Z;
        
      if (obj->process.vectors.miscA.Y > obj->process.vectors.miscB.Z)
        obj->process.vectors.miscA.Y = obj->process.vectors.miscB.Z;
      else if (obj->process.vectors.miscA.Y < -obj->process.vectors.miscB.Z)
        obj->process.vectors.miscA.Y = -obj->process.vectors.miscB.Z;
      
      if (obj->process.vectors.miscA.Z > obj->process.vectors.miscB.Z)
        obj->process.vectors.miscA.Z = obj->process.vectors.miscB.Z;
      else if (obj->process.vectors.miscA.Z < -obj->process.vectors.miscB.Z)
        obj->process.vectors.miscA.Z = -obj->process.vectors.miscB.Z;
    }
    
    //flag for object treats XZ plane at Y given by field 11C as solid ground
    if (obj->process.statusB & 0x4000)  //NOT set for crash
    {
      if (obj->process.vectors.trans.Y <= obj->process._11C) 
      {
        obj->process.vectors.trans.Y = obj->process._11C;
        obj->process.statusA |= 1;
        obj->process._130 = framesElapsed;
        
        if (obj->process.vectors.miscA.Y < 0)
        {
          obj->process._134 = obj->process.vectors.miscA.Y;
          obj->process.vectors.miscA.Y  = 0;
        }
      }
    }
  }
  
  //flag for object is falling and therefore gains in negative Y velocity
  //due to acceleration of gravity
  if (obj->process.statusB & 0x20) //set for crash
  {       
    long gravity = scale * 4000;
    obj->process.vectors.miscA.Y -= gravity;      
      
    if (obj->process.vectors.miscA.Y < -0x2EE000)
      obj->process.vectors.miscA.Y = -0x2EE000;
  }

  //flag for can generate a space/collide with crash?
  if (obj->process.statusB & 0x10)
  {   
    //if rendered at the same time as crash and we are within the specified range from crash
    //then record space in space pairs
    //if (obj->process.aniStamp == crash->process.aniStamp)
    //{
      //if (!isOutOfRange(obj, &obj->process.vectors.trans, &crash->process.vectors.trans, 0x7D000, 0xAF000, 0x7D000))
        objectSpace(obj);
    //}
  }
}

bool isOutOfRange(object *obj, cvector *pointA, cvector *pointB, unsigned long boundW, unsigned long boundH, unsigned long boundD)
{
  signed long xDist = pointA->X - pointB->X;

  if (boundW < xDist && xDist >= -boundW)
  {
    signed long yDist = pointA->Y - pointB->Y; 

    if (boundH < yDist && yDist >= -boundH)
    {
      signed long zDist = pointA->Z - pointB->Z;

      if (boundD < zDist && zDist >= -boundD)
        return false;
    }
  }

  if (obj)
    obj->process.statusA |= 0x8000;

  return true;
}

bool isColliding(cpoint *point, cspace *space)
{
  if (point->X >= space->xyz1.X &&
      point->Y >= space->xyz1.Y &&
      point->Z >= space->xyz2.Z &&
      point->X < space->xyz2.X &&
      point->Y < space->xyz2.Y &&
      point->Z < space->xyz2.Z)
  {
    return true;
  }
  
  return false;
}

bool isColliding(cspace *spcA, cspace *spcB)  //sub_80026CA8
{
  if (spcA->xyz1.Y >= spcB->xyz2.Y &&
      spcA->xyz2.Y <  spcB->xyz1.Y &&
      spcA->xyz2.X <  spcB->xyz1.X &&
      spcA->xyz2.Z <  spcB->xyz1.Z &&
      spcA->xyz1.X >= spcB->xyz2.X &&
      spcA->xyz1.Z >= spcB->xyz2.Z)
  {
    return true;
  }
  
  return false;
}

//variation of isColliding though used as inline code in the assembly
bool isContained(cspace *src, cspace *space) 
{
  if (src->xyz2.X >= space->xyz1.X &&
      src->xyz2.Y >= space->xyz1.Y &&
      src->xyz2.Z >= space->xyz1.Z &&
      src->xyz1.X <  space->xyz2.X &&
      src->xyz1.Y <  space->xyz2.Y &&
      src->xyz1.Z <  space->xyz2.Z)
  {
    return true;
  }
  
  return false;
}

bool isInsideOf(cspace *src, cspace *space)
{
  if (src->xyz1.X >= space->xyz1.X &&
      src->xyz1.Y >= space->xyz1.Y &&
      src->xyz1.Z >= space->xyz1.Z &&
      src->xyz2.X <  space->xyz2.X &&
      src->xyz2.Y <  space->xyz2.Y &&
      src->xyz2.Z <  space->xyz2.Z)
  {
    return true;
  }
  
  return false;
}

//cspace, nspace
bool isOutsideOf(cspace *src, cspace *space)
{
  if (src->xyz1.X <  space->xyz1.X ||
      src->xyz1.Y <  space->xyz1.Y ||
      src->xyz1.Z <  space->xyz1.Z ||
      src->xyz2.X >= space->xyz2.X ||
      src->xyz2.Y >= space->xyz2.Y ||
      src->xyz2.Z >= space->xyz2.Z)
  {
    return true;
  }
  
  return false;
}

unsigned long landOffset = 0xF424; //gp[0xC0]
bool beingStopped = false;         //gp[0x1E0]
cvector prevVelocity;

//velocity = translation precision velocity = scale * (obj.velocity >> 10)
//array    = collision structure
unsigned long transSmoothStopAtSolid(object *obj, cvector *velocity, traversalInfo *trav)
{
  if (!obj) 
  { 
    beingStopped = false; 
    return 0; 
  }

  //hog is higher off the ground
  if (nsd->level.LID == 0x11 || nsd->level.LID == 0x1E) { landOffset = 0x27AC4; }   //HOG WILD OR WHOLE HOG HAVE DIFFERENT VAL 
  else                                                  { landOffset = 0xF424; }   

  cvector curTrans, curVelocity;
  copyVector(&obj->process.vectors.trans, &curTrans); //var_38
  copyVector(velocity, &curVelocity);                 //var_28

  if (beingStopped)
  {
    cvector slopeAccel;
    subVector(&prevVelocity, &curVelocity, &slopeAccel);
    
    //if we've come to a smooth stop
    if (abs(slopeAccel.X) < 0xA && 
        abs(slopeAccel.Y) < 0xA &&
        abs(slopeAccel.Z) < 0xA) 
    { 
      curVelocity.X = 0;
      curVelocity.Y = velocity->Y;
      curVelocity.Z = 0;
    }
  }
  
  //now calling 3rd level procedure in relation to initial collision routine
  transPullStopAtSolid(obj, trav, &curTrans, &curVelocity);
  
  #define newTrans curTrans
  #define oldTrans obj->process.vectors.trans
  
  if (!beingStopped && (velocity->X != 0 || velocity->Z != 0))
  { 
    cvector displacement;
    subVector(&oldTrans, &newTrans, &displacement);
    
    //for small change in x,y,z:
    //apparently this occurs at the instant of returning to idle speed
    //(i.e. 0) either result of being stopped by solid object or just
    //slowing down (but we have to be moving with at least some velocity
    //short bursts do not allow this to occur)
    if (abs(displacement.X) < 2 && 
       abs(displacement.Y) < 2 && 
       abs(displacement.Z) < 2)
    {
      //save the velocity at this stop
      copyVector(velocity, &prevVelocity);

      //beingStopped = true;
    }
    else
      beingStopped = false;
  }
  else 
    beingStopped = false;

  copyVector(&newTrans, &obj->process.vectors.trans);

  if ((obj->process.statusA & 0x400) && 
      (((obj->process.statusA & 1) == 0) || obj->process.event != 0x900)) 
  {
    unsigned long args[1] = { 0x6400 };
    issueEvent(0, obj, obj->process.event, 1, args);
  }
  
  #undef newTrans
  #undef oldTrans
}

//velocity = translation precision!

void transPullStopAtSolid(object *obj, traversalInfo *trav, cvector *trans, cvector *velocity)
{
  trav->counter = 0;
  
  cvector maxPull = { cabs(velocity->X) / 25600,      //divide by 6.25 fractimeters
                      cabs(velocity->Y) / 153600,     //divide by 37.5 fractimeters
                      cabs(velocity->Z) / 25600 };    //divide by 6.25 fractimeters
                      
  signed long maxComp = -4515;
  
  if (maxPull.Y < maxPull.X)
  {
    if (maxPull.Z < maxPull.X)
      maxComp = maxPull.X + 1;
    else
      maxComp = maxPull.Z + 1;
  }
  else
  {
    if (maxPull.Z < maxPull.Y)
      maxComp = maxPull.Y + 1;
    else
      maxComp = maxPull.Z + 1;
  }
  
  cvector pull = { velocity->X / maxComp,
                   velocity->Y / maxComp,
                   velocity->Z / maxComp };
  
  while (velocity->X != 0 || velocity->Y != 0 || velocity->Z != 0)
  {
    if (cabs(velocity->X) < cabs(pull.X)) { pull.X = velocity->X; } 
    if (cabs(velocity->Y) < cabs(pull.Y)) { pull.Y = velocity->Y; } 
    if (cabs(velocity->Z) < cabs(pull.Z)) { pull.Z = velocity->Z; } 
    
    //EDIT: this section seems to loop when we turn a sharp corner
       
    //var_38 = deceleration (delta x, y, z)
    //var_28 = new translation
    
    cvector newTrans;
    transStopAtSolid(obj, trav, trans, &pull, &newTrans);
  
    trans->X = newTrans.X;
    trans->Y = newTrans.Y;
    trans->Z = newTrans.Z;
    
    velocity->X -= pull.X;
    velocity->Y -= pull.Y;
    velocity->Z -= pull.Z;
  }
}

void transStopAtSolid(object *obj, traversalInfo *trav, cvector *trans, cvector *pull, cvector *newTrans)
{
  cvector _pull;
  copyVector(pull, &_pull);  //var_38
  
  cvector _newTrans;
  addVector(&_pull, trans, &_newTrans);  //var_48
  
  signed long newY;  //var_28
  stopAtFloor(obj, trans, &_newTrans, trav, &newY);
  
  plotWalls(trans, obj, trav);
  
  //we WANT to move this many unscaled node plot units
  signed long reqMoveX = (_newTrans.X - trans->X) * 4;
  if (reqMoveX < 0) { reqMoveX += 0x1FFF; }
  signed long reqMoveZ = (_newTrans.Z - trans->Z) * 4;
  if (reqMoveZ < 0) { reqMoveZ += 0x1FFF; }

  //scale to scaled node plot units and calculate the requested point we want to move to
  //in terms of the plot (i.e. negative values are in (0-0xF) range and positive (0x10-0x20))
  unsigned char reqMovePtX = (signed char)(reqMoveX >> 13) + 0x10;
  unsigned char reqMovePtZ = (signed char)(reqMoveZ >> 13) + 0x10;
  
  //see if we can move to that point without hitting a wall
  //if not-find the next closest non-solid point to that point: this will be returned in movePtX and movePtZ
  unsigned long movePtX, movePtZ; //var_24, var_20
  unsigned long wallVal = stopAtWalls(trans, reqMovePtX, reqMovePtZ, &movePtX, &movePtZ, obj, 0);
  
  //if we even found a non-solid point in place of or near the requested move point
  if (wallVal)
  {
    //then offset new translation by adding the node plot coordinates, unscaled back to translation coordinates
    _newTrans.X = (((signed long)(movePtX - 0x10) << 13) / 4) + trans->X;
    _newTrans.Z = (((signed long)(movePtZ - 0x10) << 13) / 4) + trans->Z;
  }
  else
  {
    //otherwise we can't move in X/Z plane
    _newTrans.X = trans->X;
    _newTrans.Z = trans->Z;
  }
  
  //here we add to the counter
  trav->counter += 1;
  
  //if we've even moved in XZ plane 
  //(which also means we would have requested to check a point other than the center point in the plot)
  if (reqMovePtX != 0x10 || reqMovePtZ != 0x10)
  {
    //if the point we moved to is the point we wanted to move to
    //then we have not been stopped by a wall
    if (movePtX == reqMovePtX && movePtZ == reqMovePtZ)
    {
      obj->process.statusA |= 0x100;   //set status flag for 'not stopped by wall'
    }
  }
  
  //check for solid terrain or objects above crash
  signed long ceil = findCeil(obj, &_newTrans, trav);
  
  //if none found
  if (ceil == -999999999)
    trav->ceil = 0;
  else
    trav->ceil = ceil;  //else record the y of the terrain/object
    
  if (ceil != -999999999)
  {
    //if the ceiling height value within the objects bound box
    if ((ceil - 1) < (_newTrans.Y + 0x29900))
    {
      //stop the top of the box/object at the ceiling
      if (_newTrans.Y < (ceil - 0x29900))
        _newTrans.Y = (ceil - 0x29900);
      
      //set flag that we hit the ceiling
      obj->process.statusB |= 0x80;
      
      //and clear Y velocity as well
      if (obj->process.vectors.miscA.Y > 0)
        obj->process.vectors.miscA.Y = 0;        
    }   
  }
  
  //the floors of some zones can be solid as well, this factors in
  //these areas OR issues the death event if we leave the bottom
  //when the zone floor solidity flag is not set
  
  stopAtZone(obj, &_newTrans);  
  
  //return displacement
  newTrans->X = _newTrans.X;
  newTrans->Y = _newTrans.Y;
  newTrans->Z = _newTrans.Z;
}


//sub_8002c8ec
signed long stopAtFloor(object *obj, cvector *oldTrans, cvector *newTrans, traversalInfo *trav, signed long *output)
{
  //get region of 3 dimensional space positioned at the new location the object plans
  //to move whos bound box is sized for a collision test with event nodes
  getSpace(newTrans, &testBoxEvent, &trav->colliderSpace);
                                           //nodeSpace?
 
  //if this is the first traversal or the collider has moved outside of the
  //space where nodes were last recorded
  if (!trav->traversed || isOutsideOf(&trav->colliderSpace, &trav->nodeSpace))
  {
    //record nodes
    traverseNodes(newTrans, obj, trav);
  }
  
  //????? debug this part  to figure it out
  signed long changeY = oldTrans->Y - newTrans->Y;
    
  signed long height;
  //if old point y in relation to new is positive, object (crash) is moving up
  if (changeY > 0)
    height = oldTrans->Y + landOffset;  //since the object moves up from its previous position
  else                                  //we are interested in the furthest down position experienced by the object
    height = newTrans->Y + landOffset;  //else comply with this
    
  getSpace(newTrans, &testBoxSurface, &trav->colliderSpace);
   
  signed long avgNodeY[3];
  
  //handle nodes previously recorded relative to nodeSpace that collide with colliderSpace
  //this includes returning the average y of any colliding solid nodes 
  //              returning the average y of any colliding event nodes
  handleNodes(obj, trav, &trav->nodeSpace, &trav->colliderSpace, height, avgNodeY, -999999999, processNode);

  //s5 = solidNodesY
  signed long solidNodesY = avgNodeY[1];  //average height of wall/solid scenery nodes
  signed long floorNodesY = avgNodeY[2];  //average height of floor nodes/valid event nodes, level bound nodes

  //now we record both y's in the traversalInfo structure
  if (floorNodesY != -999999999)
    trav->floorNodesY = floorNodesY;
  else
    trav->floorNodesY = 0;
    
  if (solidNodesY != -999999999)
    trav->solidNodesY = solidNodesY;
  else
    trav->solidNodesY = 0;
    
  //returns Y of highest colliding object below 'obj'
  signed long solidObjsY = findHighestBelowObjectY(obj, oldTrans, newTrans, trav);
  
  //set up initial flags                            
  unsigned long aFlags = 0x40001;                   //flags for on the ground & hitting the ground (assume on ground)
  signed long groundOffset = 0;
  
  if (solidObjsY == -999999999)                     //if no colliding object
    trav->solidObjsY = 0;                           //then we cant record the lowest colliding one
  else                                              
  {                                                 
    floorNodesY      = solidObjsY;                  //highest event under is an object
    trav->solidObjsY = solidObjsY;                  //also record that object Y
    
    aFlags &= 0xFFFBFFFF;                           //and now not on the ground?
    if (obj->process.collider)                      //boxes?
    {
      object *collider = obj->process.collider;
      
      if (collider->process.statusB & 0x400000)  
        groundOffset = 0x19000;
        
      //if at least 4 units have elapsed since the last ground collision
      if ((obj->process.aniStamp - obj->process._130) < 4) 
        aFlags = 0x200000;       //atop box?
      else
        aFlags |= 0x204001;      //bouncing and breaking box?
    }
    else
      aFlags |= 0x200000;
  }
  
  //at this point we have the Y values for 3 separate potential collision instances
  //1) solidNodesY = the average Y position of solid scenery/walls collided with under the source object  
  //2) floorNodesY = the average Y position of solid floors/terrain collided with under the source object
  //3) solidObjsY  = the Y position of the highest solid object under the source object
  
  //of course, if there is a 'solidObjsY' this takes precedence over 'floorNodesY' or its value
  //is replaced. thus the revised 2 separate potential collision instances as follows
  //1) solidNodesY = the average Y position of solid scenery/walls collided with under the source object 
  //2) floorNodesY = the Y position of the highest solid object under the source object
  //                 OR IF NONE APPLICABLE then the  average Y position of solid floors/terrain 
  //                 collided with under the source object
  
  //we must choose among these 2 values (if they exist) as the Y position to force the source object to
  //effectively stopping it from falling
  
  //this of course requires that:
  
  //a) at least one of the 2 Y values is valid (this shows that collision is taking place)
  //b) the source object -is- indeed falling and NOT jumping 
   
  //solid = s5, floor = s2
  
  //if we are not colliding with beneath(a.k.a atop) the floor and not atop a wall 
  if ((floorNodesY == -999999999) && (solidNodesY == -999999999))
  {
    //if we are jumping
    if (obj->process.vectors.miscA.Y > 0)  
    {
      //clear bits 1, 15, 19, 22
      //no toggle, no bounce/break box, not on ground, no collide box
      obj->process.statusA &= 0xFFDBBFFE;  
    }
    
    trav->floor = 0;    //we are not on the floor
    return -999999999;  
  }
  //otherwise we are atop the floor or a wall
  else
  {
    //if we are jumping
    if (obj->process.vectors.miscA.Y > 0)
    {
      //clear bits 1, 15, 19, 22
      //no toggle, no bounce/break box, not on ground, no collide box
      obj->process.statusA &= 0xFFDBBFFE;  
    
      trav->floor = 0;    //we are not on the floor
      return -999999999;
    }
    //otherwise we are falling or NOT jumping
    else
    {
      signed long ground, floor;
      
      //determine what type of beneath collision there is; remember the associated Y value as the
      //'ground' Y value
      //if both exist then floor collision takes precedence over wall/solid scenery collision
      //IMPORTANT to note that if ground is determined to be wall/solid scenery collision then
      //we only set bit 1 of status flags:
      //-this must mean bit 1 is set at the instance of determining a beneath collision/i.e at the
      //instant of landing on solid terrain
      //-and bit 19 tells us whether the floor (SET) OR solid scenery/walls (CLEAR) is beneath us
      if   (floorNodesY != -999999999) { ground = floorNodesY; }
      else                             { ground = solidNodesY; aFlags = 1; }
       
      trav->floor = ground;
      floor = trav->floor + 1;
      
      signed long testY;
      if (floorNodesY == -999999999)
        testY = obj->process.vectors.trans.Y;
      else
        testY = obj->process.vectors.trans.Y + landOffset + groundOffset;
      output[0] = testY;
      
      if (testY < floor)
        floor = obj->process.vectors.trans.Y;
        
      //finally force the new Y position to the floor
      newTrans->Y = floor;
      
      //recalculate collider space
      trav->colliderSpace.xyz1.Y = floor;  
      trav->colliderSpace.xyz2.Y = floor + 0x29900;
      
      //if this is the first traversal or the collider has moved outside of the
      //space where nodes were last recorded
      
      if (!trav->traversed || isOutsideOf(&trav->colliderSpace, &trav->nodeSpace))        
      {
        //record nodes
        traverseNodes(newTrans, obj, trav);
      }

      //if we are falling at this point and we have just hit solid terrain
      signed long velocityY = obj->process.vectors.miscA.Y;
      if ((velocityY < 0) && (aFlags & 1))
      {
        obj->process._134            = velocityY;  //remember the Y velocity with which we have hit the terrain
        obj->process.vectors.miscA.Y =         0;  //clear the Y velocity; stop from falling
      }
      
      //set status flags
      obj->process.statusA |= aFlags;
      
      //set timestamp of hitting the ground
      obj->process._130 = framesElapsed;
      
      return floor;
    }       
  }
}

bool once = false;

//record nodes (at array) of the current zone and each neighboring zone
// that lie within the bound 'testBoxZone' when positioned at 'newtrans' 
unsigned long traverseNodes(cvector *newTrans, object *obj, traversalInfo *trav)
{
  trav->traversed    = true;
  trav->traversalLen =    0;
  
  getSpace(newTrans, &testBoxZone, &trav->nodeSpace);
  
  unsigned char *zoneHeader       = currentZone->itemData[0];
  unsigned long zoneNeighborCount = getWord(zoneHeader, 0x210, true); 
  
  for (int count = 0; count < zoneNeighborCount; count++)
  {
    unsigned long neighborEID = getWord(zoneHeader, 0x214+(count*4), true);
    entry *neighbor           = crashSystemPage(neighborEID);
    
    unsigned char *neighbDimItem = neighbor->itemData[1];
      
    signed long neighbX = CTCVC(getWord(neighbDimItem, 0, true));
    signed long neighbY = CTCVC(getWord(neighbDimItem, 4, true));
    signed long neighbZ = CTCVC(getWord(neighbDimItem, 8, true));
    signed long neighbW = CTCVC(getWord(neighbDimItem, 0xC, true));
    signed long neighbH = CTCVC(getWord(neighbDimItem, 0x10, true));
    signed long neighbD = CTCVC(getWord(neighbDimItem, 0x14, true));
      
    cspace neighbSpace  = { {         neighbX,         neighbY,         neighbZ },
                            { neighbX+neighbW, neighbY+neighbH, neighbZ+neighbD } };
      
    //if the zone space contains this node space then record 
    //collision information by traversing the octree for this zone 
    //given the node space 
    if (isContained(&trav->nodeSpace, &neighbSpace))
    {
      unsigned long *destTraversal = (unsigned long*)trav->traversal;
      queryNodes(neighbDimItem, &trav->nodeSpace, destTraversal, trav->traversalLen); 
    }
  }   

  //0xFFFF indicates the end of collision information
  unsigned long offset = trav->traversalLen * 8;
  unsigned short *dest = (unsigned short*)&trav->traversal[offset];
  *(dest) = 0xFFFF; 

  readyNodeQuery(trav, &trav->nodeSpace);
        
  return trav->traversalLen;  
}  

void handleNodes(object *obj, traversalInfo *trav, cspace *nodeSpace, cspace *colliderSpace, signed long height, signed long *output, signed long defY, bool (*routine)(object*, unsigned long))
{  
  unsigned long nodeSpaceX = nodeSpace->xyz1.X;
  unsigned long nodeSpaceY = nodeSpace->xyz1.Y;
  unsigned long nodeSpaceZ = nodeSpace->xyz1.Z;
  
  unsigned long colliderSpaceX1 = colliderSpace->xyz1.X;
  unsigned long colliderSpaceY1 = colliderSpace->xyz1.Y;
  unsigned long colliderSpaceZ1 = colliderSpace->xyz1.Z;
  unsigned long colliderSpaceX2 = colliderSpace->xyz2.X;
  unsigned long colliderSpaceY2 = colliderSpace->xyz2.Y;
  unsigned long colliderSpaceZ2 = colliderSpace->xyz2.Z;

  unsigned long *curStruct = (unsigned long*)trav->traversal;    
  
  unsigned long  zoneW,  zoneH,  zoneD;
  unsigned long depthX, depthY, depthZ;
  
  unsigned long solidNodes = 0;
  unsigned long eventNodes = 0;
  signed long solidSum = 0;
  signed long eventSum = 0;
  
  while (true)
  {  
    if ((curStruct[0] & 0xFFFF) == 0xFFFF)
      break;
    else if ((curStruct[0] & 0xFFFF) == 0)
    {
      zoneW = ((curStruct[0] & 0xFFFF0000) >> 16) << 8; /*curStruct[2] << 8*/  //s4
      zoneH = ((curStruct[1] & 0x0000FFFF) >>  0) << 8; /*curStruct[4] << 8*/  //s5
      zoneD = ((curStruct[1] & 0xFFFF0000) >> 16) << 8; /*curStruct[6] << 8*/  //s6
      
      depthX = (curStruct[2] & 0xFFFF0000) >> 16; /*depthX = curStruct[0xA]*/   //s7
      depthY = (curStruct[3] & 0x0000FFFF) >>  0; /*depthY = curStruct[0xC]*/   //t8
      depthZ = (curStruct[3] & 0xFFFF0000) >> 16; /*depthZ = curStruct[0xE];*/   //t9
        
      //curStruct+=0x10;
      curStruct += 4;
    }
    //otherwise its a collision point
    else      
    {
      //curStruct[0] = VVVVVVVV VVVVVIII, I = node level, V = value of node
        
      //nodeValue = VVVVVVVV VVVVVVV1      //lowest bit forced set
      unsigned short nodeValue = ((curStruct[0] & 0xFFF8) >> 2) | 1; //t1 
      unsigned char  nodeLevel = (curStruct[0] & 7);      //t0
       
      unsigned short nodeType    = nodeValue & 0xE;
      unsigned short nodeSubtype = (nodeValue & 0x3F0) >> 4;
      
      //**** signed values
      //after shifting to break apart the respective values, we need to
      //recover the sign by shifting into the sign bit (<<16); then we
      //recover the original scale by shifting back (>>12) because these 
      //values are stored at a scale of 16
      signed long nodeX = ((signed long)(((curStruct[0] & 0xFFFF0000) >> 16) << 16)) >> 12; 
      signed long nodeY = ((signed long)(((curStruct[1] & 0x0000FFFF) >>  0) << 16)) >> 12; 
      signed long nodeZ = ((signed long)(((curStruct[1] & 0xFFFF0000) >> 16) << 16)) >> 12; 
             
      signed long nodeX1 = nodeSpaceX + nodeX;
      signed long nodeY1 = nodeSpaceY + nodeY;
      signed long nodeZ1 = nodeSpaceZ + nodeZ;
     
      if (colliderSpaceX2 >= nodeX1 &&
          colliderSpaceY2 >= nodeY1 &&
          colliderSpaceZ2 >= nodeZ1)            
      {
        unsigned long nodeW;
        if (depthX < nodeLevel)
          nodeW = zoneW >> depthX;
        else
          nodeW = zoneW >> nodeLevel;
            
        signed long nodeX2 = nodeX1 + nodeW;
          
        unsigned long nodeH;
        if (depthY < nodeLevel)
          nodeH = zoneH >> depthY;
        else
          nodeH = zoneH >> nodeLevel;
            
        signed long nodeY2 = nodeY1 + nodeH;
        
        unsigned long nodeD;
        if (depthZ < nodeLevel)
          nodeD = zoneD >> depthZ;
        else
          nodeD = zoneD >> nodeLevel;
            
        signed long nodeZ2 = nodeZ1 + nodeD;
        
          
        if (nodeX2 >= colliderSpaceX1 &&
            nodeY2 >= colliderSpaceY1 &&
            nodeZ2 >= colliderSpaceZ1)
        {  
          //if node is a death node, or node is an event node (type 0 and subtype nonzero and < 0x27)
          bool isMisc = ((nodeType == 6) || (nodeType == 8) ||
            (nodeSubtype && (nodeSubtype < 0x27)));
              
          //if its a misc type node then process it
          if (isMisc)
            isMisc = (*routine)(obj, nodeValue);
                      
          //if its not misc or by processing determined to not be misc               
          if (!isMisc)
          {
            if (height >= nodeY2)
            {
              if (nodeType == 0)
              {
                solidNodes++;
                solidSum += nodeY2;
              }
              else
              {
                eventNodes++;
                eventSum += nodeY2;
              }
            }
          }
        }
      }
        
      curStruct += 2;
    }
  } 
     
  if (solidNodes)
    output[1] = solidSum / solidNodes;   //var_6C
  else
    output[1] = defY;
    
  if (eventNodes)
    output[2] = eventSum / eventNodes;   //var_68
  else
    output[2] = defY;
}  

bool processNode(object *obj, unsigned long nodeValue)
{
  unsigned long nodeType, nodeFunction;
 
  if (nodeValue == 0)
    nodeType = 0; //a0
  else
    nodeType = ((nodeValue & 0xE) >> 1) + 1;
     
  nodeFunction = ((nodeValue & 0x3F0) >> 4) - 1;  

  switch (nodeFunction)
  {
    case 1:
      obj->process.event    = 0x700;
      obj->process.statusA |= 0x400;
    break;
    case 2:
      obj->process.event    = 0xC00;
      obj->process.statusA |= 0x400;
    break;
    case 3:
      obj->process.event    = 0x2100;
      obj->process.statusA |= 0x400;
    break;
    case 4:
      obj->process.event    = 0x1F00;
      obj->process.statusA |= 0x400;
    break;
    case 5:
      obj->process.event    = 0x1E00;
      obj->process.statusA |= 0x400;
    break;
    case 6:
      obj->process.event    = 0xD00;
      obj->process.statusA |= 0x400;
    break;
    case 7:
    case 8:
    case 9:
    case 10:
      obj->process.event    = 0x1200;
      obj->process.statusA |= 0x2000;
    break;
    case 11:
      obj->process.event    = 0x900;
      obj->process.statusA |= 0x400;
    return 1;
    case 12:
      obj->process.event    = 0x2300;
      obj->process.statusA |= 0x400;
    break;
  }
  
  if (nodeType == 4)  //pit death A
  {
    if (nsd->level.LID == 3 || nsd->level.LID == 7) //cortex power or
    {                                               //toxic waste
      obj->process.event   = 0x2100;                //sludge death
      obj->process.statusA |= 0x400;
      return 0;
    }
    else
    {
      obj->process.event    = 0x900;
      obj->process.statusA |= 0x400;
      return 1;
    }
  }
  else if (nodeType == 3)  //wall node
    return 1;
  else if (nodeType == 5)  //pit death B
    return 1;
  else
    return 0;
}      


signed long findHighestBelowObjectY(object *obj, cvector *trans, cvector *newTrans, traversalInfo *trav)
{
  signed long newY = newTrans->Y; //a0
  signed long oldY =    trans->Y; //v0
  
  signed long minY = -999999999;  //s0
  
  if (spacePairCount != 0)        //v1
  { 
    signed long testY;
    signed long diff = oldY - newY; //v0
    
    if (diff > 0)
      testY = oldY + 0xF424;
    else
      testY = newY + 0xF424;
      
    getSpace(newTrans, &testBoxObj, &trav->colliderSpace);
    
    trav->counter += 0x10;
    
    unsigned long count =     0;
    bool colliding      = false;
    
    spacePair *foundPair = 0;
    object    *foundObj  = 0;
    while (count < spacePairCount)  //bound box count
    {
      spacePair *pair = &spacePairs[count];
        
      signed long Y2 = pair->space.xyz2.Y;
      
      if (((testY >= Y2) || (pair->obj->process.statusB & 0x400000)) && (minY < Y2))
      {
        colliding = false;
        if (Y2 >= trav->colliderSpace.xyz1.Y)
        {
          if (pair->space.xyz1.Y <  trav->colliderSpace.xyz2.Y &&
              pair->space.xyz1.X <  trav->colliderSpace.xyz2.X &&
              pair->space.xyz1.Z <  trav->colliderSpace.xyz2.Z &&
              pair->space.xyz2.X >= trav->colliderSpace.xyz1.X &&
              pair->space.xyz2.Z >= trav->colliderSpace.xyz1.Z)
          {
            colliding = true;
          }
          
          if (colliding)
          {
            foundPair = pair;
            
            if (pair->obj->process.statusB & 0x20000)
            {
              entry *codeEntry          = pair->obj->global;
              unsigned char *codeHeader = codeEntry->itemData[0];
              unsigned long codeType    = getWord(codeHeader, 4, true);
              
              if ( ( ( (obj->process.routineFlagsA & 0x10) == 0) && ( obj->process.displayMode != 5 ) ) 
               || ( codeType != 0x300 )
               || ( (pair->obj->process.statusC & 0x1012) && ( (pair->obj->process.routineFlagsA & 0x10020) == 0 ) ) )
              {
                minY = pair->space.xyz2.Y + 1;
              }
            }
            
            foundObj       = pair->obj;
            trav->counter += 0x1000;
          }
        }
      }
      
      count++;
    }
    
    if (foundObj)
    {
      objectCollide(obj, &trav->colliderSpace, foundObj, &foundPair->space);
    }
  }
  
  return minY;
}

void plotWalls(cvector *trans, object *obj, traversalInfo *trav)
{
  unsigned char *zoneHeader = currentZone->itemData[0];
  unsigned long zoneFlags   = getWord(zoneHeader, 0x2FC, true);
  
  if ((zoneFlags & 0x100000) == 0)  //if the zone has walls?
  {
    //clear the wall cache and wall map
    for (int count = 0x1F; count >= 0; count--)
      wallCache[count] = 0;
     
    for (int count = 0x7F; count >= 0; count--)
      wallMap[count] = 0;
      
    unsigned long val;
    if (obj->process.displayMode == 3 || obj->process.displayMode == 4)
      val = obj->process.statusC & 2;
    else
      val = 2;
        
    //Y + landOffset  or Y + landOffset/4096
    //Y + 12800,      or Y + 50      (approx 4 small tile nodes top y to bot y?)
    //Y + 170240,     or Y + 665       
    plotZoneWalls(trav, &trav->nodeSpace, val, 
                  obj->process.vectors.trans.Y + landOffset, 
                  obj->process.vectors.trans.Y + 0x3200,
                  trans->Y+0x29900, trans->X, trans->Z);
  }
  
  //and this routine is for solid objects left, right, in front
  //or behind crash that should stop him
  plotObjWalls(trans, obj, trav, true);
}

void plotZoneWalls(traversalInfo *trav, cspace *nodeSpace, bool flag, signed long yvalA, signed long yvalB, signed long transY, signed long transX, signed long transZ)
{  
  unsigned long *curStruct = (unsigned long*)trav->traversal;
  
  unsigned long zoneW, zoneH, zoneD;
  unsigned long depthX, depthY, depthZ;
  
  while (true)
  {  
    if ((curStruct[0] & 0xFFFF) == 0xFFFF)
      break;
    else if ((curStruct[0] & 0xFFFF) == 0)
    {
      zoneW = ((curStruct[0] & 0xFFFF0000) >> 16) << 8; /*curStruct[2] << 8*/  //s4
      zoneH = ((curStruct[1] & 0x0000FFFF) >>  0) << 8; /*curStruct[4] << 8*/  //s5
      zoneD = ((curStruct[1] & 0xFFFF0000) >> 16) << 8; /*curStruct[6] << 8*/  //s6
      
      depthX = (curStruct[2] & 0xFFFF0000) >> 16; /*depthX = curStruct[0xA]*/   //s7
      depthY = (curStruct[3] & 0x0000FFFF) >>  0; /*depthY = curStruct[0xC]*/   //t8
      depthZ = (curStruct[3] & 0xFFFF0000) >> 16; /*depthZ = curStruct[0xE];*/   //t9
        
      //curStruct+=0x10;
      curStruct += 4;
    }
      
    //otherwise its a collision point
    else      
    {
      //curStruct[0] = VVVVVVVV VVVVVIII, I = node level, V = value of node
        
      //nodeValue = VVVVVVVV VVVVVVV1      //lowest bit forced set
      unsigned short nodeValue = ((curStruct[0] & 0xFFF8) >> 2) | 1; //t1 
      unsigned char  nodeLevel = (curStruct[0] & 7);      //t0
        
      //**** signed values
      //after shifting to break apart the respective values, we need to
      //recover the sign by shifting into the sign bit (<<16); then we
      //recover the original scale by shifting back (>>12) because these 
      //values are stored at a scale of 16
      signed long nodeX = ((signed long)(((curStruct[0] & 0xFFFF0000) >> 16) << 16)) >> 12; 
      signed long nodeY = ((signed long)(((curStruct[1] & 0x0000FFFF) >>  0) << 16)) >> 12; 
      signed long nodeZ = ((signed long)(((curStruct[1] & 0xFFFF0000) >> 16) << 16)) >> 12; 
                    
      //sampling stores locations relative to the query bounds
      //this makes up for the displacement
      signed long nodeX1 = nodeX + nodeSpace->xyz1.X;
      signed long nodeY1 = nodeY + nodeSpace->xyz1.Y;
      signed long nodeZ1 = nodeZ + nodeSpace->xyz1.Z;      
      
      unsigned long nodeW, nodeH, nodeD;
      if (nodeLevel >= depthX) { nodeW = zoneW >> depthX; }
      else                     { nodeW = zoneW >> nodeLevel; }
      if (nodeLevel >= depthY) { nodeH = zoneH >> depthY; }
      else                     { nodeH = zoneH >> nodeLevel; }
      if (nodeLevel >= depthZ) { nodeD = zoneD >> depthZ; }
      else                     { nodeD = zoneD >> nodeLevel; }
      
      signed long nodeX2 = nodeX1 + nodeW;
      signed long nodeY2 = nodeY1 + nodeH;
      signed long nodeZ2 = nodeZ1 + nodeD;
         
      unsigned char nodeType    = (nodeValue &   0xE) >> 1;         
      unsigned char nodeSubtype = (nodeValue & 0x3F0) >> 4;
          
      //type 0/0: solid wall/scenery node
      //type 1/2: floor node
      //type 2/4: level bound/wall node
      //type 3/6: death/bound or pit node
      //type 4/8: death/or is this pit? node
                    
      //this is sampling nodes and excluding ones that fall outside a unit circle
      //bitmap in order to perform a nearest neighbor search... 
      
      //crash bound box = <-0x9600, 0, -0x9600> - <0x9600, 0x29900, 0x9600>
      
      //yValA = trans->Y + 0xF424;   //this is 3125/8512 above the bottom of crash
      //yValB = trans->Y + 0x3200     //this is 10/133 above the bottom of crash
      //transY = trans->Y + 0x29900   //this is the top of crash

      
      //first condition (involves all wall type nodes && solid scenery):
      
      //        ^O^   <-- clearly above bottom
      //        .+.
      //  |--|  /\     <- we are just below the top of wall   
      //  |  |   |
      //  |  |   |
      //  |  |   |
      //  |  |   |
      //  |  |   |   (at any point along the line we should be stopped by the wall)
      //  |  |   |
      //  ----   |   
      //         |
      //         V     
      
      //second condition (involves all floor type nodes)
      
      //         ^O^ 
      //    |--| .+.
      //     |--|/\  
      //       |--||--||--|   (|--| = floor nodes to equal approximately 1/8th height of crash)    
      //    
      // the floor to our left comes to a point higher than (approx) the center of our body
      // so we instead consider it a wall
                
      if (nodeType != 3 && nodeType != 4)
      {
        if (
           ((nodeType != 1) &&                      //if we are not a floor type node, then a wall type node
             (nodeSubtype == 0 || nodeSubtype >= 0x27 || flag) &&
             (yvalB < nodeY2 && nodeY1 < transY)) //and if 10/133 above bottom of crash is below top of the node, and the bottom of the node is below the top of crash
        || (nodeType == 1 &&                      //or if we 'are' a floor type node
             (yvalA < nodeY2 && nodeY1 < transY)) //and if it is higher than a pt approximately at the center of crash
           )               
        {
          
          signed long diffX1 = (nodeX1 - transX) * 4;  //t6
          if (diffX1 >= 0)
            diffX1 >>= 13;
          else
            diffX1 = (diffX1 + 0x1FFF) >> 13;
              
          signed long diffX2 = (nodeX2 - transX) * 4;  //s1
          if (diffX2 >= 0)
            diffX2 >>= 13;
          else
            diffX2 = (diffX2 + 0x1FFF) >> 13;

          signed long diffZ1 = (nodeZ1 - transZ) * 4;  //s0
          if (diffZ1 >= 0)
            diffZ1 >>= 13;
          else
            diffZ1 = (diffZ1 + 0x1FFF) >> 13;
              
          signed long diffZ2 = (nodeZ2 - transZ) * 4;  //s3
          if (diffZ2 >= 0)
            diffZ2 >>= 13;
          else
            diffZ2 = (diffZ2 + 0x1FFF) >> 13;
               
          signed short boxX1 = diffX1;
          signed short boxX2 = diffX2;
          signed short boxZ1 = diffZ1;
          signed short boxZ2 = diffZ2;
          signed short boxX1i = diffX1;
          signed short boxX2i = diffX2;
          signed short boxZ1i = diffZ1;
          signed short boxZ2i = diffZ2;
                          
          if (-32 < boxZ1 && boxZ1 < 32)
          {
            while (true)
            {
              if (-32 < boxX1i)
              {
                if (boxX1i >= 32 || boxX1i >= boxX2)
                  break;
                else
                  plotWall(boxX1i, boxZ1);
              }
              
              boxX1i += 8;
            }
          }
          
          if (-32 < boxX2 && boxX2 < 32)
          {
            while (true)
            {
              if (-32 < boxZ1i)
              {
                if (boxZ1i >= 32 || boxZ1i >= boxZ2)
                  break;
                else
                  plotWall(boxX2, boxZ1i);
              }
              
              boxZ1i += 8;
            }
          }
                      
          if (-32 < boxZ2 && boxZ2 < 32)
          {
            while (true)
            {
              if (boxX2i < 32)
              {
                if (-32 >= boxX2i || boxX1 >= boxX2i)
                  break;
                else
                  plotWall(boxX2i, boxZ2);
              }
              
              boxX2i -= 8;
            }
          }
          
          if (boxX1 < 32 && -32 < boxX1)
          {
            while (true)
            {
              if (boxZ2i < 32)
              {
                if (-32 >= boxZ2i || boxZ1 >= boxZ2i)
                  break;
                else
                  plotWall(boxX1, boxZ2i);
              }
              
              boxZ2i -= 8;
            }
          }
        }      
      }
      
      curStruct += 2;
    }
  }          
}   

void plotWall(signed char x, signed char z)
{
  unsigned short bit, offset;
  unsigned long circleBits;
  
  if (x < 0)
  {
    offset = ((z + 32) * 2) + 0;
    bit    = 1 << (x + 32);
  }
  else
  {
    offset = ((z + 32) * 2) + 1;        
    bit    = 1 << (x +  0);
  }
  
  if ((wallCache[offset] & bit) == 0)
  {
    wallCache[offset] |= bit;
       
    unsigned short offset;
    unsigned char start, end;
    if (z < 0)
    {
      start = -z;
      end   = 32;
      
      offset = 0;
    }
    else
    {
      start  =      0;
      end    = 32 - z;
      
      offset = z;
    }
      
    if (x < 0)
    {
      if (start < end)
      {
        x = -x;
        do
        {
          circleBits         = circleMap[start++];  //at         
          wallMap[offset++] |= (circleBits << x);
         
        } while (start < end);
      }  
    }
    else
    {
      if (start < end)
      {
        do
        {
          circleBits         = circleMap[start++];
          wallMap[offset++] |= (circleBits >> x);
        } while (start < end);
      }
    }
  }
}
    
void plotWallB(signed char x, signed char z)
{
  unsigned short bit, offset;
  unsigned long circleBits;
  
  if ((cabs(x) < 32) && (cabs(z) < 32))
  {
    if (x < 0)
    {
      offset = ((z + 32) * 2) + 0;
      bit    = 1 << (x + 32);
    }
    else
    {
      offset = ((z + 32) * 2) + 1;        
      bit    = 1 << (x +  0);
    }
    
    if ((wallCache[offset] & bit) == 0)
    {
      wallCache[offset] |= bit;
         
      unsigned short offset;
      unsigned char start, end;
      if (z < 0)
      {
        start = -z;
        end   = 32;
        
        offset = 0;
      }
      else
      {
        start  =      0;
        end    = 32 - z;
        
        offset = z;
      }
        
      if (x < 0)
      {
        if (start < end)
        {
          x = -x;
          do
          {
            circleBits         = circleMap[start++];  //at         
            wallMap[offset++] |= (circleBits << x);
           
          } while (start < end);
        }  
      }
      else
      {
        if (start < end)
        {
          do
          {
            circleBits         = circleMap[start++];
            wallMap[offset++] |= (circleBits >> x);
          } while (start < end);
        }
      }
    }
  }
}

signed long dx1, dx2, dz1, dz2;

//sub_8002D8B8
void plotObjWalls(cvector *trans, object *obj, traversalInfo *trav, bool flag)
{

  unsigned char *zoneHeader = currentZone->itemData[0];
  unsigned long zoneFlags   = getWord(zoneHeader, 0x2FC, true);
  
  bool zoneHasWalls = ((zoneFlags >> 20) ^ 1) & 1;  //var_38
  
  signed long transX = trans->X; //var_48
  signed long transY = trans->Y; //
  signed long transZ = trans->Z; //var_40
  
  if (spacePairCount)
  {
    object *collider = obj->process.collider;  //var_30
    
    cspace objSpace = { { transX + obj->bound.xyz1.X,    //var_B8
                          transY + obj->bound.xyz1.Y,    //var_B4
                          transZ + obj->bound.xyz1.Z },  //var_B0 
                        { transX + obj->bound.xyz2.X,    //var_AC
                          transY + obj->bound.xyz2.Y,    //var_A8
                          transZ + obj->bound.xyz2.Z } };//var_A4
                            
    cspace objSpaceB = { { objSpace.xyz1.X - 0x6400,     //var_A0
                           objSpace.xyz1.Y,              //var_9C
                           objSpace.xyz1.Z - 0x6400 },   //var_98
                         { objSpace.xyz2.X + 0x6400,     //var_94
                           objSpace.xyz2.Y,              //var_90
                           objSpace.xyz2.Z + 0x6400 } }; //var_8C
                              
    trav->counter += 0x100;
    
    signed long count = spacePairCount - 1;
    if (count < 0) { return; }
    
    do
    {
      spacePair *pair = &spacePairs[count];
      object *pairObj  = pair->obj; 
  
      if (pairObj != collider || !flag || objSpaceB.xyz1.Y < pair->space.xyz2.Y)
      {
        if (pair->space.xyz2.Y >= objSpaceB.xyz1.Y && 
            pair->space.xyz1.Y <  objSpaceB.xyz2.Y && 
            pair->space.xyz1.X <  objSpaceB.xyz2.X &&
            pair->space.xyz1.Z <  objSpaceB.xyz2.Z &&
            pair->space.xyz2.X >= objSpaceB.xyz1.X &&
            pair->space.xyz2.Z >= objSpaceB.xyz1.Z)
        { 
          cspace hitSpace = { { pair->space.xyz1.X + pairObj->process._148,     //var_88 
                                pair->space.xyz1.Y,                             //var_84
                                pair->space.xyz1.Z + pairObj->process._148 },   //var_80
                              { pair->space.xyz2.X - pairObj->process._148,     //var_7C
                                pair->space.xyz2.Y,                             //var_78
                                pair->space.xyz2.Z - pairObj->process._148 } }; //var_74
            
          entry *codeEntry          = pairObj->global;
          unsigned char *codeHeader = codeEntry->itemData[0];
          
          unsigned long codeObject = getWord(codeHeader, 0, true);
          unsigned long codeType   = getWord(codeHeader, 4, true);
              
          if (zoneHasWalls && (pairObj->process.statusB & 0x10000))
          {
            if (   ( ( (obj->process.routineFlagsA & 0x10) == 0 ) && ( obj->process.displayMode != 5 ) ) 
                || ( codeType != 0x300 )
                || ( (pairObj->process.statusC & 0x1012) && ( (pairObj->process.routineFlagsA & 0x10020) == 0 ) ) )
            {
              //checks the 2 y boundaries again for some reason...
            
              //PoPIC, thinking maybe this is 'icons'/items in levels
              //that don't have a parent box, they stand alone by themselves
              //ex. aku aku, crash lives in 2d levels
              if (codeObject == 0xB) 
              {
                signed long distX = (pairObj->process.vectors.trans.X - trans->X) >> 8;
                signed long distZ = (pairObj->process.vectors.trans.Z - trans->Z) >> 8;
                
                float distF        = sqrt((distX*distX)+(distZ*distZ));
                unsigned long dist = FLOAT2FIXED(distF) << 8;
                
                if (dist < 0x19000) { continue; }   //i'm not enclosing this in another level of if...
              }
            
              //calculate the region of 2-d space the pair object occupies in the XZ plane 
              //in terms of distance from the source object
              //THEN convert these to uncentralized node plot coordinates
              //this way we can plot the object walls similarly to how we plot the solid  
              //zone terrain walls, in terms of coordinate system
              
              signed long diffX1 = (hitSpace.xyz1.X - transX) * 4;  //s1, s6
              if (diffX1 >= 0)
                diffX1 >>= 13;
              else
                diffX1 = (diffX1 + 0x1FFF) >> 13;
              
              signed long diffX2 = (hitSpace.xyz2.X - transX) * 4;  //s4
              if (diffX2 >= 0)
                diffX2 >>= 13;
              else
                diffX2 = (diffX2 + 0x1FFF) >> 13;

              signed long diffZ1 = (hitSpace.xyz1.Z - transZ) * 4;  //s5, s0
              if (diffZ1 >= 0)
                diffZ1 >>= 13;
              else
                diffZ1 = (diffZ1 + 0x1FFF) >> 13;
              
              signed long diffZ2 = (hitSpace.xyz2.Z - transZ) * 4;  //s3
              if (diffZ2 >= 0)
                diffZ2 >>= 13;
              else
                diffZ2 = (diffZ2 + 0x1FFF) >> 13;
     
              dx1 = diffX1 + 0xF;
              dx2 = diffX2 + 0x10;
              dz1 = diffZ1 + 0x10;
              dz2 = diffZ2 + 0x10;
              
              //instead of plotting the proper portion of the circle as in plotting solid zone terrain walls
              //so as to make the collision smooth (for they have no defined area), we fill the entire area
              //occupied by the region of 2-d space in the XZ plane for the object in the wall map
              
              signed long boxX1 = diffX1 + 0xF;
              signed long boxX2 = diffX2 + 0x10;
              
              
              if (boxX2 > 0 && boxX1 < 32)  //binary does these comparisons in a different order
              {                             //does this make a huge impact?
                unsigned long bits = 0xFFFFFFFF;
                signed long bitsB  = 0x80000000;
                if (boxX2 < 32)
                {
                  unsigned char shiftVal = 32 - boxX2;
                  bits <<= shiftVal;
                }

                if (boxX1 >= 0)
                {
                  unsigned char shiftVal = boxX1;
                  bitsB  >>= shiftVal;
               
                  bits &= ~bitsB;
                }
              
                if (bits)  //
                {
                  signed long boxZ1 = diffZ1 + 0x10;
                  signed long boxZ2 = diffZ2 + 0x10;
                  
                  if (boxZ1 < 0)
                    boxZ1 = 0;
                    
                  if (boxZ2 > 32)
                    boxZ2 = 32;
                                
                  if (boxZ1 < 32)
                  {
                    for (int lp = boxZ1; lp < boxZ2; lp++)
                      wallMap[lp] |= bits;
                  }
                }
              }
              
              //if flag is set then we fill the region like we would multiple walls surrounding the
              //outer bounds of the region
              if (flag)
              {
                signed long wallX1 = diffX1;  //s6
                signed long wallX  = diffX1;  //s1
                signed long wallX2 = diffX2;  //s4
                signed long wallZ1 = diffZ1;  //s5
                signed long wallZ  = diffZ1;  //s0
                signed long wallZ2 = diffZ2;  //s3
                
                while (wallX < wallX2) 
                {
                  plotWallB(wallX, wallZ);
                  wallX += 8;
                }
                wallX = wallX2;
                
                while (wallZ < wallZ2)
                {
                  plotWallB(wallX, wallZ);
                  wallZ += 8;
                }
                wallZ = wallZ2;
                
                while (wallX1 < wallX)
                {
                  plotWallB(wallX, wallZ);
                  wallX -= 8;
                }
                wallX = wallX1;
                
                while (wallZ1 < wallZ)
                {
                  plotWallB(wallX, wallZ);
                  wallZ -= 8;
                }
              }
            }
          }

          if (flag)  //do a collision test while we're at it
          {
            if (pair->space.xyz2.Y >= objSpace.xyz1.Y && 
                pair->space.xyz1.Y <  objSpace.xyz2.Y && 
                pair->space.xyz1.X <  objSpace.xyz2.X &&
                pair->space.xyz1.Z <  objSpace.xyz2.Z &&
                pair->space.xyz2.X >= objSpace.xyz1.X &&
                pair->space.xyz2.Z >= objSpace.xyz1.Z)
            {
              //this allows crash to collide with objects from sides
              //and send appropriate events

              objectCollide(obj, &objSpace, pairObj, &pair->space);
            }
          }
        }
      }
    
      count--;
    } while (count >= 0);
  }
}

//ok here is my attempt at describing the plotting system although so many things
//are still pretty unclear

//generally we want to be stopped by solid terrain/objects in physics engines
//and this involves adjusting the object back to the closest nonsolid location 
//rather than into the solid location itself

//ex. simple for platformers where walls/bounds are based in one dimension, 
//in this case X
// 
//  //for stopping at the right
//  if (player.X + player.W > wall.X) //if the right of the player exceeds the left of the wall
//    player.X = wall.X;              //shouldn't happen, so readjust back to the left of the wall
//  
//  if (player.X < wall.X + wall.W)   //if the left of the player exceeds the right of the wall
//    player.X = wall.X + wall.W;     //shouldn't happen, so readjust back to the left of the wall

//when walls are based in two dimensions, things get trickier since walls aren't straight,
//or not even necessarily linear, so the point of readjustment varies-and further,
//since we can approach the wall in 2 dimensions, we need to be able to readjust for
//both dimensions

//here's how crash does it:

//crash is at C
//X denotes solid terrain or object to left, right, front, or behind crash
//O denotes the point where Crash wants to move at his current velocity

//-----------------------------------------------------------------
//|X|X|X| | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
//-----------------------------------------------------------------
//|X|X|X|X|X| | | | | | | | | |C| | | | | | | | | | | | | | | | | |
//---------------------------/-------------------------------------
//|X|X|X|X|X|X| | | | | | |/  | | | | | | | | | | | | | | | | | | |
//-----------------------/-----------------------------------------
//|X|X|X|X|X|X|X| | | |/| | | | | | | | | | | | | | | | | | | | | | 
//-------------------/---------------------------------------------
//|X|X|X|X|X|X| | |/| | | | | | | | | | | | | | | | | | | | | | | |
//---------------/-------------------------------------------------
//|X|X|X|X|X|X|/| | | | | | | | | | | | | | | | | | | | | | | | | |
//-----------/-----------------------------------------------------
//|X|X|X|X|/|X| | | | | | | | | | | | | | | | | | | | | | | | | | |
//-------/--------------------------------------------------------
//|X|X|/|X| | | | | | | | | | | | | | | | | | | | | | | | | | | | |
//----V------------------------------------------------------------
//|X|O|X| | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
//-----------------------------------------------------------------
//|X|X| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
//-----------------------------------------------------------------
//|X| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
//-----------------------------------------------------------------
//| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
//-----------------------------------------------------------------
//| | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |

//basically we do a scaled plot of all nearby solid terrain or objects 
//that constitute a wall before calculating which point crash would end up 
//at his current velocity, regardless of solidity. if crash ends up at
//a solid point then we iterate through a list of precomputed pairs sorted 
//by euclidian distance:
//
//  (1,0), (1,1), (2,0), (2,1), (2,2), (3,0) ... (16,16)

//to check for in relation to the solid point until a non-solid point
//is found, in effect calculating the 'closest non-solid location'

//it is worth noting that only the pairs (1,0), (1,1), (2,0), (2,1), (2,2), (3,0) 
//are stored in the precomputed list 
//because in the code (after first checking the initial point)
//for all pairs (X,Y) where X != Y the inverse pair (Y,X) is also checked, 
//the (almost) final check sequence:
//
// (0,0), (1,0), (0,1), (1,1), (2,0), (0,2), (2,1), (1,2), (2,2), (3,0), (0,3) ...
//  ... (16,16)
//
//which is the sequence of all possible positive integer pairs sorted by euclidian
//distance, with such a distance less than the maximum of 16sqrt(2)
//
//of course for all pairs we check the 4 mirrored versions of each pair, because
//for each (X,Y) where X > 0 and Y > 0 there are pairs (-X,Y), (X,-Y), (-X,-Y)
//with the same euclidian distance as (X,Y), 
//the final check sequence:
//
// (0,0), (1,0), (-1,0), (0,1), (0,-1), (1,1), (-1,1), (1,-1), (-1,-1), (2,0),
// (-2,0), (0,2), (0,-2), (2,1), (-2,1), (2,-1), (-2,-1), (1,2), (-1,2), (1,-2),
// (-1,2), (-1,-2), (2,2), (-2,2), (2,-2), (-2,2), (3,0), (-3,0), (0,3), (0,-3),
// (-16,-16)

unsigned long stopAtWalls(cvector *trans, unsigned long reqMovePtX, unsigned long reqMovePtZ, unsigned long *movePtX, unsigned long *movePtZ, object *obj, unsigned long ret)
{
  bool secondCheck = false;
  
  do
  {
    //bit 21 of zone flags says we aren't stopped by walls in the zone and the zone has no walls
    unsigned char *zoneHeader = currentZone->itemData[0];
    unsigned long zoneFlags   = getWord(zoneHeader, 0x2FC, true);
      
    //if we're requesting to move to a valid point in the wall map
    if (reqMovePtX < 32 && reqMovePtX < 32)
    {
      //get the appropriate values to test that point for occupancy in the wall map
      unsigned long bits = wallMap[reqMovePtZ];
      unsigned long bit  = 0x80000000 >> reqMovePtX;

      //if the point isn't occupied with a wall (or bit 21)
      if (((bits & bit) == 0) || (zoneFlags & 0x100000))
      {
        //then we can move there
        *(movePtX) = reqMovePtX;
        *(movePtZ) = reqMovePtZ;
      
        //also return this
        return ret+2;
      }
    }
    else if (zoneFlags & 0x100000)
    {
      *(movePtX) = reqMovePtX;
      *(movePtZ) = reqMovePtZ;
    
      return ret+2;
    }
    
    unsigned char *checkDist = sortedDists;
    unsigned long testPtX, testPtZ;
    unsigned long count = 0;
    
    do
    {
      unsigned char offsetX = checkDist[0];
      unsigned char offsetZ = checkDist[1];
      checkDist += 2;
      
      //first check the point at distance (+x, +z) from the requested move point
      testPtX = reqMovePtX + offsetX;  
      testPtZ = reqMovePtZ + offsetZ;  
      
      //if its a valid point in the wall map
      if (testPtX < 32 && testPtZ < 32)
      {
        //get the appropriate values to test that point for occupancy in the wall map
        unsigned long bits = wallMap[testPtZ];
        unsigned long bit  = 0x80000000 >> testPtX;

        //if the point isn't occupied with a wall
        if ((bits & bit) == 0)
        {
          //then we can move there
          *(movePtX) = testPtX;
          *(movePtZ) = testPtZ;
      
          //also return this
          return ret+2;
        }
      }
      
      //then check the point at distance (+x, -z) from the requested move point
      testPtX = reqMovePtX + offsetX;  
      testPtZ = reqMovePtZ - offsetZ;
      
      //if its a valid point in the wall map
      if (testPtX < 32 && testPtZ < 32)
      {
        //get the appropriate values to test that point for occupancy in the wall map
        unsigned long bits = wallMap[testPtZ];
        unsigned long bit  = 0x80000000 >> testPtX;

        //if the point isn't occupied with a wall
        if ((bits & bit) == 0)
        {
          //then we can move there
          *(movePtX) = testPtX;
          *(movePtZ) = testPtZ;
      
          //also return this
          return ret+2;
        }
      }
      
      //then check the point at distance (-x, +z) from the requested move point...
      testPtX = reqMovePtX - offsetX;  
      testPtZ = reqMovePtZ + offsetZ;
      
      //if its a valid point in the wall map
      if (testPtX < 32 && testPtZ < 32)
      {
        //get the appropriate values to test that point for occupancy in the wall map
        unsigned long bits = wallMap[testPtZ];
        unsigned long bit  = 0x80000000 >> testPtX;

        //if the point isn't occupied with a wall
        if ((bits & bit) == 0)
        {
          //then we can move there
          *(movePtX) = testPtX;
          *(movePtZ) = testPtZ;
      
          //also return this
          return ret+2;
        }
      }
            
      //then check the point at distance (-x, -z) from the requested move point
      testPtX = reqMovePtX - offsetX;  
      testPtZ = reqMovePtZ - offsetZ;
      
      //if its a valid point in the wall map
      if (testPtX < 32 && testPtZ < 32)
      {
        //get the appropriate values to test that point for occupancy in the wall map
        unsigned long bits = wallMap[testPtZ];
        unsigned long bit  = 0x80000000 >> testPtX;

        //if the point isn't occupied with a wall
        if ((bits & bit) == 0)
        {
          //then we can move there
          *(movePtX) = testPtX;
          *(movePtZ) = testPtZ;
      
          //also return this
          return ret+2;
        }
      }
      
      //if the distance pair has an inverse
      if (offsetX != offsetZ)
      {
        //check the point at distance (+z, +x) from the requested move point
        testPtX = reqMovePtX + offsetZ;  
        testPtZ = reqMovePtZ + offsetX;  
        
        //if its a valid point in the wall map
        if (testPtX < 32 && testPtZ < 32)
        {
          //get the appropriate values to test that point for occupancy in the wall map
          unsigned long bits = wallMap[testPtZ];
          unsigned long bit  = 0x80000000 >> testPtX;

          //if the point isn't occupied with a wall
          if ((bits & bit) == 0)
          {
            //then we can move there
            *(movePtX) = testPtX;
            *(movePtZ) = testPtZ;
        
            //also return this
            return ret+2;
          }
        }
        
        //then check the point at distance (+z, -x) from the requested move point
        testPtX = reqMovePtX + offsetZ;  
        testPtZ = reqMovePtZ - offsetX;
        
        //if its a valid point in the wall map
        if (testPtX < 32 && testPtZ < 32)
        {
          //get the appropriate values to test that point for occupancy in the wall map
          unsigned long bits = wallMap[testPtZ];
          unsigned long bit  = 0x80000000 >> testPtX;

          //if the point isn't occupied with a wall
          if ((bits & bit) == 0)
          {
            //then we can move there
            *(movePtX) = testPtX;
            *(movePtZ) = testPtZ;
        
            //also return this
            return ret+2;
          }
        }
        
        //then check the point at distance (-z, +x) from the requested move point...
        testPtX = reqMovePtX - offsetZ;  
        testPtZ = reqMovePtZ + offsetX;
        
        //if its a valid point in the wall map
        if (testPtX < 32 && testPtZ < 32)
        {
          //get the appropriate values to test that point for occupancy in the wall map
          unsigned long bits = wallMap[testPtZ];
          unsigned long bit  = 0x80000000 >> testPtX;

          //if the point isn't occupied with a wall
          if ((bits & bit) == 0)
          {
            //then we can move there
            *(movePtX) = testPtX;
            *(movePtZ) = testPtZ;
        
            //also return this
            return ret+2;
          }
        }
              
        //then check the point at distance (-z, -x) from the requested move point
        testPtX = reqMovePtX - offsetZ;  
        testPtZ = reqMovePtZ - offsetX;
        
        //if its a valid point in the wall map
        if (testPtX < 32 && testPtZ < 32)
        {
          //get the appropriate values to test that point for occupancy in the wall map
          unsigned long bits = wallMap[testPtZ];
          unsigned long bit  = 0x80000000 >> testPtX;

          //if the point isn't occupied with a wall
          if ((bits & bit) == 0)
          {
            //then we can move there
            *(movePtX) = testPtX;
            *(movePtZ) = testPtZ;
        
            //also return this
            return ret+2;
          }
        }
      }
      
      count++;
    } while (count < 152);
  
    //the only way the above would have checked all possible points is if the
    //check had been done in relation to the origin i.e. (16,16) so that last 2
    //checks would have been (32,32), (0,0)...
    //ex. in the case of checking from (0,0), only the upper left quadrant could have
    //been checked (since nothing is to the left and/or above (0,0))
    
    secondCheck = false;
    object *collider = obj->process.collider;
    
    //if we aren't checking from (16,16)...
    if (reqMovePtX != 16 || reqMovePtZ != 16)
    { 
      if (collider)
      {
        entry *codeEntry           = collider->global;
        unsigned char *codeHeader  = codeEntry->itemData[0];
        unsigned long colliderType = getWord(codeHeader, 0, true);
        
        //if its not a box
        if (colliderType != 0x22)
        {
          unsigned long index = 0;
          //in the case in which we find the entire plot solid (rarely the case
          //unless for boxes) we abandon checking the unchecked portion of the plot
          while (index < 32)  
          {
            //when we find a line that isn't entirely solid then we know the whole 
            //plot isnt solid
            if (wallMap[index++] != 0xFFFFFFFF)
            {
              //so redo the check a second time at a position ensuring that the rest
              //of the 32x32 points are checked
              reqMovePtX += 16;
              reqMovePtZ += 16;
              
              //not sure how to replicate the binary's flow without an additional loop flag 
              secondCheck = true;
              
              break;
            }
          }
        }
      }
    }
  } while (secondCheck);

  //we reach here if 
  //  1) at least a quarter of the plot has been checked but crash is touching a box
  //  2) the entire plot is solid
  
  if (ret < 0x100)
  {
    object *collider = obj->process.collider;
    unsigned long colliderType;
    
    if (collider)
    {
      entry *codeEntry          = collider->global;
      unsigned char *codeHeader = codeEntry->itemData[0];
      colliderType              = getWord(codeHeader, 0, true);
    }
      
    if (!collider || colliderType != 0x22)
    { 
      //uses reg. t7?
      unsigned long count = replotWalls(0, 0, trans, obj);
    
      if (count != 0)
      {
        replotWalls(1, 1, trans, obj);
        plotObjWalls(trans, obj, &zoneTraversal, 0);
      }
    
      return stopAtWalls(trans, reqMovePtX, reqMovePtZ, movePtX, movePtZ, obj, 0x100);
    }
  }

  //bounce from box/enemy?
  obj->process.statusA |= 0x100000;
   
  //no movement
  *(movePtX) = 16;
  *(movePtZ) = 16;
    
  return 0;
}

//this is so we can clear the plot of non-walls, and replot with strictly walls
unsigned long replotWalls(bool noClear, bool flags, cvector *trans, object *obj)
{
  traversalInfo *trav = &zoneTraversal;
  
  unsigned long *curStruct = (unsigned long*)trav->traversal;
  
  unsigned long zoneW, zoneH, zoneD;
  unsigned long depthX, depthY, depthZ;
  
  unsigned long numNodes = 0;
  unsigned long count = 0;
  
  while (true)
  {  
    if ((curStruct[0] & 0xFFFF) == 0xFFFF)
      break;
    else if ((curStruct[0] & 0xFFFF) == 0)
    {
      zoneW = ((curStruct[0] & 0xFFFF0000) >> 16) << 8; /*curStruct[2] << 8*/  //s4
      zoneH = ((curStruct[1] & 0x0000FFFF) >>  0) << 8; /*curStruct[4] << 8*/  //s5
      zoneD = ((curStruct[1] & 0xFFFF0000) >> 16) << 8; /*curStruct[6] << 8*/  //s6
      
      depthX = (curStruct[2] & 0xFFFF0000) >> 16; /*depthX = curStruct[0xA]*/   //s7
      depthY = (curStruct[3] & 0x0000FFFF) >>  0; /*depthY = curStruct[0xC]*/   //t8
      depthZ = (curStruct[3] & 0xFFFF0000) >> 16; /*depthZ = curStruct[0xE];*/   //t9
        
      //curStruct+=0x10;
      curStruct += 4;
    }
    //otherwise its a collision point
    else      
    {
      //curStruct[0] = VVVVVVVV VVVVVIII, I = node level, V = value of node
        
      //nodeValue = VVVVVVVV VVVVVVV1      //lowest bit forced set
      unsigned short nodeValue = ((curStruct[0] & 0xFFF8) >> 2) | 1; //t1 
      unsigned char  nodeLevel = (curStruct[0] & 7);      //t0
       
      unsigned char nodeType;       
      if (nodeValue != 0) //when would it ever be
        nodeType = ((nodeValue & 0xE) >> 1) + 1;
      else
        nodeType = 0;
          
      if (nodeType != 5 && nodeType != 0 &&
            ((flags == 0 && nodeType == 2) 
          || (flags == 1 && nodeType != 2)
          || (flags != 0 && flags != 1)))
      {
        //**** signed value
        //after shifting to break apart the value, we need to
        //recover the sign by shifting into the sign bit (<<16); then we
        //recover the original scale by shifting back (>>12) because the 
        //value is stored at a scale of 16
        signed long nodeY = ((signed long)(((curStruct[1] & 0x0000FFFF) >>  0) << 16)) >> 12; 
        unsigned long nodeH;    
              
        //note: nodeLevel not set at first entry!
        if (nodeLevel >= depthY) { nodeH = zoneH >> depthY; }
        else                     { nodeH = zoneH >> nodeLevel; }
        
        signed long nodeY1 = nodeY  + trav->nodeSpace.xyz1.Y;
        signed long nodeY2 = nodeY1 + nodeH;
        
        signed long distY1 = nodeY1 - trans->Y;
        signed long distY2 = nodeY2 - trans->Y;

        if ( (flags == 0 && (distY1 < 0x6401 && distY2 > -0x19000) ) ||
             (flags == 1 && (distY2 >= 0)) ||
             (flags == 2) )
        {
          //**** signed values
          //after shifting to break apart the respective values, we need to
          //recover the sign by shifting into the sign bit (<<16); then we
          //recover the original scale by shifting back (>>12) because these 
          //values are stored at a scale of 16
          signed long nodeX = ((signed long)(((curStruct[0] & 0xFFFF0000) >> 16) << 16)) >> 12; 
          signed long nodeZ = ((signed long)(((curStruct[1] & 0xFFFF0000) >> 16) << 16)) >> 12; 
                
          unsigned long nodeW, nodeD;
          if (nodeLevel >= depthX) { nodeW = zoneW >> depthX; }
          else                     { nodeW = zoneW >> nodeLevel; }
          if (nodeLevel >= depthZ) { nodeD = zoneD >> depthZ; }
          else                     { nodeD = zoneD >> nodeLevel; }
    
          signed long nodeX1 = nodeX + trav->nodeSpace.xyz1.X;
          signed long nodeZ1 = nodeZ + trav->nodeSpace.xyz1.Z;  
          
          signed long nodeX2 = nodeX1 + nodeW;
          signed long nodeZ2 = nodeZ1 + nodeD;

          signed long distX1 = nodeX1 - trans->X;
          signed long distX2 = nodeX2 - trans->X;
          signed long distZ1 = nodeZ1 - trans->Z;
          signed long distZ2 = nodeZ2 - trans->Z;

           signed long diffX1 = distX1 * 4; 
          if (diffX1 >= 0)
            diffX1 >>= 13;
          else
            diffX1 = (diffX1 + 0x1FFF) >> 13;
            
          signed long diffX2 = distX2 * 4;
          if (diffX2 >= 0)
            diffX2 >>= 13;
          else
            diffX2 = (diffX2 + 0x1FFF) >> 13;
            
          signed long diffZ1 = distZ1 * 4;
          if (diffZ1 >= 0)
            diffZ1 >>= 13;
          else
            diffZ1 = (diffZ1 + 0x1FFF) >> 13;
            
          signed long diffZ2 = distZ2 * 4;
          if (diffZ2 >= 0)
            diffZ2 >>= 13;
          else
            diffZ2 = (diffZ2 + 0x1FFF) >> 13;
            
          signed long boxX1 = diffX1 + 0xF;
          signed long boxX2 = diffX2 + 0x10;
            
          if (boxX2 > 0 && boxX1 < 32)  //binary does these comparisons in a different order
          {                             //does this make a huge impact?
            unsigned long bits = 0xFFFFFFFF;
            signed long bitsB  = 0x80000000;
            if (boxX2 < 32)
            {
              unsigned char shiftVal = 32 - boxX2;
              bits <<= shiftVal;
            }

            if (boxX1 > 0)
            {
              unsigned char shiftVal = boxX1;
              bitsB  >>= shiftVal;
             
              bits &= ~bitsB;
            }
            
            if (bits)  //
            {
              signed long boxZ1 = diffZ1 + 0x10;
              signed long boxZ2 = diffZ2 + 0x10;
                
              if (boxZ1 < 0)
                boxZ1 = 0;
                  
              if (boxZ2 > 32)
                boxZ2 = 32;
                  
              if (noClear)
              {                
                for (int lp = boxZ1; lp < boxZ2; lp++)
                  wallMap[lp] |= bits;
              }
              else
              {
                for (int lp = boxZ1; lp < boxZ2; lp++)
                  wallMap[lp] &= ~bits;
              }
            }
          }
            
          numNodes++;
        }
      }
      
      curStruct += 2;
      count++;
    }
  }
  
  return numNodes;
}

signed long findCeil(object *obj, cvector *trans, traversalInfo *trav)
{
  getSpace(trans, &testBoxObjTop, &trav->colliderSpace);
  
  signed long lowestY = -999999999;
  object *firstFound = 0;
  
  //the below code uses the space map to find the first colliding object with the
  //specified space and for any additional colliding objects finds the lowest (y value)
  //of all
  if (spacePairCount > 0)
  {
    unsigned long count = 0;
    do
    {
      spacePair *pair = &spacePairs[count];
      
      object *pairObj = pair->obj;
    
      if (pairObj->process.statusB & 0x80000000)   //flag for is solid on the bottom?
      {
        if (pair->space.xyz2.Y >= trav->colliderSpace.xyz1.Y && 
            pair->space.xyz1.Y <  trav->colliderSpace.xyz2.Y &&
            pair->space.xyz1.X <  trav->colliderSpace.xyz2.X &&
            pair->space.xyz1.Z <  trav->colliderSpace.xyz2.Z &&
            pair->space.xyz2.X >= trav->colliderSpace.xyz1.X &&
            pair->space.xyz2.Z >= trav->colliderSpace.xyz1.Z)
        {
          if (lowestY == -999999999)
          {
            lowestY    = pair->space.xyz1.Y;
            firstFound = pairObj;          
          }
          else
          {
            if (lowestY >= pair->space.xyz1.Y)
              lowestY = pair->space.xyz1.Y;
          }
        }
      }
    
      count++;
    } while (count < spacePairCount);
  }
  
  //type 0/1/0: solid scenery node
  //type 1/2/2: floor node
  //type 2/3/4: wall node
  //type 3/4/6: death/bound or pit? node
  //type 4/5/8: death/or is this pit? node

  //bound box for averaging all node heights is a bit smaller x/z wise;
  getSpace(trans, &testBoxCeil, &trav->colliderSpace);

  signed long stopY;
  //here we want the average Y of all colliding nodes of types 2 and 1
  //these types are solid scenery nodes and floor nodes
  stopY = findAvgY(obj, trav, &trav->nodeSpace, &trav->colliderSpace, 2, 1, -999999999);
  
  unsigned char *zoneHeader = obj->zone->itemData[0];
  unsigned long zoneFlags   = getWord(zoneHeader, 0x2FC, true);
  
  //the below code allows zones with no zones above them to be solid at the top
  if (zoneFlags & 0x20000)
  {
    //find the zone that contains the pt 0x2580 units above the player
    cvector above   = { trans->X, trav->colliderSpace.xyz2.Y, trans->Z };
    entry *nextZone = findZone(currentZone, &above);
    
    //if there is no zone there
    if (isErrorCode((unsigned long)nextZone))
    {  
      //then if this point is higher than the zone
      //-restrict the player to the height of the zone: return zone height
      unsigned char *zoneDim = obj->zone->itemData[1];
     
      signed long zoneY = getWord(zoneDim, 4, true) << 8;     
      if (zoneY < above.Y)
        stopY = zoneY;
    }
  }
  
  //if an object has been found to collide with the specified bounding space (player)
  //and the box/player is (at this point) in air/not stopped by solid ground or 
  //the lowest colliding object is lower than the ceiling then the object is being
  //hit from bottom
  if (lowestY != -999999999 && (stopY == -999999999 || lowestY < stopY))
  {
    
    //flag for hit from the bottom?
    firstFound->process.statusA |= 0x80;
    
    //hit from the bottom...
    unsigned long args[1] = { 0x6400 };
    issueEvent(obj, firstFound, 0x1700, 1, args);
    
    return lowestY;
  }
  else
    return stopY;  //otherwise the player touches the ceiling, so return that
}

signed long findAvgY(object *obj, traversalInfo *trav, cspace *nodeSpace, cspace *colliderSpace, unsigned long typeA, unsigned long typeB, signed long defY)
{  
  
  unsigned long nodeTypeA = (typeA - 1) << 1;   //test for these node types
  unsigned long nodeTypeB = (typeB - 1) << 1;
  
  unsigned long *curStruct = (unsigned long*)trav->traversal;
  
  unsigned long zoneW, zoneH, zoneD;
  unsigned long depthX, depthY, depthZ;
  
  signed long sum   = 0;
  signed long count = 0;
  
  do
  {  
    if ((curStruct[0] & 0xFFFF) == 0xFFFF)
      break;
    else if ((curStruct[0] & 0xFFFF) == 0)
    {
      zoneW = ((curStruct[0] & 0xFFFF0000) >> 16) << 8; /*curStruct[2] << 8*/  //s4
      zoneH = ((curStruct[1] & 0x0000FFFF) >>  0) << 8; /*curStruct[4] << 8*/  //s5
      zoneD = ((curStruct[1] & 0xFFFF0000) >> 16) << 8; /*curStruct[6] << 8*/  //s6
      
      depthX = (curStruct[2] & 0xFFFF0000) >> 16; /*depthX = curStruct[0xA]*/   //s7
      depthY = (curStruct[3] & 0x0000FFFF) >>  0; /*depthY = curStruct[0xC]*/   //t8
      depthZ = (curStruct[3] & 0xFFFF0000) >> 16; /*depthZ = curStruct[0xE];*/   //t9
        
      curStruct += 4;
    }
    else
    {
      unsigned short nodeValue = ((curStruct[0] & 0xFFF8) >> 2) | 1; 
      unsigned short nodeType = nodeValue & 0xE;
      
      if (nodeType == nodeTypeA || nodeType == nodeTypeB)
      {
        unsigned char nodeLevel = (curStruct[0] & 7);      
        
        //signed word * 16
        //**** signed values
        //after shifting to break apart the respective values, we need to
        //recover the sign by shifting into the sign bit (<<16); then we
        //recover the original scale by shifting back (>>12) because these 
        //values are stored at a scale of 16
        signed long nodeX = ((signed long)(((curStruct[0] & 0xFFFF0000) >> 16) << 16)) >> 12; 
        signed long nodeY = ((signed long)(((curStruct[1] & 0x0000FFFF) >>  0) << 16)) >> 12; 
        signed long nodeZ = ((signed long)(((curStruct[1] & 0xFFFF0000) >> 16) << 16)) >> 12; 

        signed long nodeX1 = nodeX + nodeSpace->xyz1.X;
        signed long nodeY1 = nodeY + nodeSpace->xyz1.Y;
        signed long nodeZ1 = nodeZ + nodeSpace->xyz1.Z;      
        
        if (colliderSpace->xyz2.X >= nodeX1 &&
            colliderSpace->xyz2.Y >= nodeY1 &&
            colliderSpace->xyz2.Z >= nodeZ1)
        {
          unsigned short nodeW, nodeH, nodeD;
          if (nodeLevel >= depthX) { nodeW = zoneW >> depthX; }
          else                     { nodeW = zoneW >> nodeLevel; }
          if (nodeLevel >= depthY) { nodeH = zoneH >> depthY; }
          else                     { nodeH = zoneH >> nodeLevel; }
          if (nodeLevel >= depthZ) { nodeD = zoneD >> depthZ; }
          else                     { nodeD = zoneD >> nodeLevel; }
          
          signed long nodeX2 = nodeX1 + nodeW;
          signed long nodeY2 = nodeY1 + nodeH;
          signed long nodeZ2 = nodeZ1 + nodeD;
        
          if (nodeX2 >= colliderSpace->xyz1.X &&
              nodeY2 >= colliderSpace->xyz1.Y && 
              nodeZ2 >= colliderSpace->xyz1.Z)
          {              
            count++;
            sum += nodeY1;  //top of node (we use this routine in findCeil)
          }
        }
      }
      
      curStruct += 2;
    }
  } while (true);
  
  if (count != 0)
  {
    signed long avgNodeY = sum / count;
    
    return avgNodeY;
  }
  else
    return defY;
}
             
unsigned long stopAtZone(object *obj, cvector *trans)
{
  entry *neighbor = findZone(currentZone, trans);
  
  //if such a vector's endpt is not contained within the zone
  //or any of its neighbors
  //(i.e. below a zone, we have left the zone)
  if (isErrorCode((unsigned long)neighbor))
  {
    unsigned char *zoneDim = obj->zone->itemData[1];
    signed long zoneY    = getWord(zoneDim, 4, true) << 8;  

    signed long objectTop = obj->bound.xyz1.Y;
    signed long transY  = trans->Y;
    
    //if we are below the zone
    if (transY < (zoneY - objectTop))
    {  
      //assume alive
      bool alive = true;
      
      //if zone is ripper roo then we clearly issue the water death event
      if (nsd->level.LID == 0x17) //if zone is ripper roo
      {
        issueEvent(0, obj, 0x2100, 1, {0}); 
      }
      
      //we need the flags to check if the zone is solid at the bottom
      //or causes death
      unsigned char *zoneHeader = obj->zone->itemData[0];
      unsigned long zoneFlags   = getWord(zoneHeader, 0x2FC, true);
      
      //if zone is not solid at bottom, fall into hole and die
      if ((zoneFlags & 2) && (obj->process.displayMode != 2))
      {    
        unsigned long args[1] = { 0x6400 };
        issueEvent(0, obj, 0x900, 1, args);
        
        alive = false;
      }
      
      //otherwise we treat the bottom of the zone like any solid
      //terrain node in that we stop falling, record fall velocity, etc
      if (alive)
      {
        trans->Y = zoneY - objectTop;
        
        //save old y velocity
        obj->process._134 = obj->process.vectors.miscA.Y;
        
        //clear y velocity/stop falling
        obj->process.vectors.miscA.Y = 0;

        //set flag for stopped by solid ground
        obj->process.statusA |= 1;        
       
        //timestamp
        obj->process._130 = framesElapsed;
      }
    }
  }
  else
    obj->zone = neighbor;  //load that zone
    
  if (obj->zone == 0) { return 0xFFFFFF01; }
  
  //there is an additional check for water levels
  unsigned char *zoneHeader = obj->zone->itemData[0];
  unsigned long zoneFlags   = getWord(zoneHeader, 0x2FC, true);
  
  unsigned long waterVal = getWord(zoneHeader, 0x300, true);
  
  if (zoneFlags & 4)
  {
    if (obj->process.vectors.trans.Y < waterVal)
    {
      //(water deaths seem to be handled differently...)
      //if level is upstream or up the creek
      if (nsd->level.LID == 0xF || nsd->level.LID == 0x18)
      {
        unsigned long args[1] = { 0x27100 };
        issueEvent(0, obj, 0x2100, 1, args);
      }
    }
  }
  
  return 0xFFFFFF01;
}

unsigned long objectCollide(object *collidee, cspace *collideeSpc, object *collider, cspace *colliderSpc)
{
  object *curCollider = collidee->process.collider;
  
  if (curCollider && (curCollider != collider))
  {
    if (collider->process.routineFlagsA & 0x800)
    {
      collider->process.collider = collidee;
      
      return 0xFFFFFFE4;
    }
    else
    {
      unsigned long distCur = apxDist(&collidee->process.vectors.trans, &curCollider->process.vectors.trans);
      unsigned long distNew = apxDist(&collidee->process.vectors.trans, &collider->process.vectors.trans);
      
      //if the collider is further from the collidee than the collidee's current collider
      if (distNew >= distCur)
      {
        //if collision priority flag not set then return 
        if ((curCollider->process.routineFlagsA & 0x800) == 0)
          return 0xFFFFFFE4;   
      }
    }
  }
 
  collider->process.collider = collidee;
  collidee->process.collider = collider;
  
  if (collidee->process._148)
  {
    cspace testSpc = { { collideeSpc->xyz1.X + collidee->process._148,
                         collideeSpc->xyz1.Y,
                         collideeSpc->xyz1.Z + collidee->process._148 },
                       { collideeSpc->xyz2.X - collidee->process._148,
                         collideeSpc->xyz2.Y,
                         collideeSpc->xyz2.Z - collidee->process._148 } };
                           
    unsigned long retval = isColliding(&testSpc, colliderSpc);
    
    if (retval != 0)
      collidee->process.statusA |= 0x1000;
  }
  
  if (collider->process._148)
  {
    cspace testSpc = { { colliderSpc->xyz1.X + collider->process._148,
                         colliderSpc->xyz1.Y,
                         colliderSpc->xyz1.Z + collider->process._148 },
                       { colliderSpc->xyz2.X - collider->process._148,
                         colliderSpc->xyz2.Y,
                         colliderSpc->xyz2.Z - collider->process._148 } };
                           
    unsigned long retval = isColliding(&testSpc, collideeSpc);
  
    if (retval != 0)
      collider->process.statusA |= 0x1000;
  }
  
  return 0xFFFFFF01;
}

unsigned long issueToAll(object *sender, unsigned long event, unsigned long mode, unsigned long count, unsigned long *args)
{
  eventQuery query;
  
  query.sender   = sender;
  query.event    = event;
  query.mode     = mode;
  query.argCount = count;
  query.count    = 0;
  query.args     = args;
  
  for (int group=0; group < 8; group++)
  {
    familyRoutineCBPH(&headObjects[group], (unsigned long (*)(object*,void*))issueIfColliding, (void*)&query);
  }
  
  return GOOL_CODE_SUCCESS;
}

unsigned long issueIfColliding(object *recipient, eventQuery *query)
{
  switch (query->mode)
  {
    case 4:
    {
      entry *codeEntry          = recipient->global;
      unsigned char *codeHeader = codeEntry->itemData[0];
      unsigned long codeType    = getWord(codeHeader, 0, true);
      
      if (codeType != 0x300 && codeType != 0x400)
        return GOOL_CODE_SUCCESS;
    }
    //no break; onto case 1
    case 1:
    {
      object *sender = query->sender;
      cspace senderSpace, recipientSpace;
      
      senderSpace.xyz1.X = recipient->bound.xyz1.X + recipient->process.vectors.trans.X;
      senderSpace.xyz1.Y = recipient->bound.xyz1.Y + recipient->process.vectors.trans.Y;
      senderSpace.xyz1.Z = recipient->bound.xyz1.Z + recipient->process.vectors.trans.Z;
      senderSpace.xyz2.X = recipient->bound.xyz2.X + recipient->process.vectors.trans.X;
      senderSpace.xyz2.Y = recipient->bound.xyz2.Y + recipient->process.vectors.trans.Y;
      senderSpace.xyz2.Z = recipient->bound.xyz2.Z + recipient->process.vectors.trans.Z;
     
      recipientSpace.xyz1.X = sender->bound.xyz1.X + sender->process.vectors.trans.X;
      recipientSpace.xyz1.Y = sender->bound.xyz1.Y + sender->process.vectors.trans.Y;
      recipientSpace.xyz1.Z = sender->bound.xyz1.Z + sender->process.vectors.trans.Z;
      recipientSpace.xyz2.X = sender->bound.xyz2.X + sender->process.vectors.trans.X;
      recipientSpace.xyz2.Y = sender->bound.xyz2.Y + sender->process.vectors.trans.Y;
      recipientSpace.xyz2.Z = sender->bound.xyz2.Z + sender->process.vectors.trans.Z;

      bool colliding = isColliding(&senderSpace, &recipientSpace);
      
      if (!colliding)
        return GOOL_CODE_SUCCESS;
    }
    break;
    
    case 3:
    {
      entry *codeEntry          = recipient->global;
      unsigned char *codeHeader = codeEntry->itemData[0];
      unsigned long codeType    = getWord(codeHeader, 0, true);
      
      if ((codeType != 0x300) && (codeType != 0x400))
        return GOOL_CODE_SUCCESS;
    }
    //no break; onto case 2
    case 2:
    {
      object *sender = query->sender;
      cspace recipientSpace;

      
      recipientSpace.xyz1.X = sender->bound.xyz1.X + sender->process.vectors.trans.X;
      recipientSpace.xyz1.Y = sender->bound.xyz1.Y + sender->process.vectors.trans.Y;
      recipientSpace.xyz1.Z = sender->bound.xyz1.Z + sender->process.vectors.trans.Z;
      recipientSpace.xyz2.X = sender->bound.xyz2.X + sender->process.vectors.trans.X;
      recipientSpace.xyz2.Y = sender->bound.xyz2.Y + sender->process.vectors.trans.Y;
      recipientSpace.xyz2.Z = sender->bound.xyz2.Z + sender->process.vectors.trans.Z;

      bool colliding = isColliding((cpoint*)&recipient->process.vectors.trans, &recipientSpace);
      
      if (!colliding)
        return GOOL_CODE_SUCCESS;  
    }
    break;

    case 5:
    {
      entry *codeEntry          = recipient->global;
      unsigned char *codeHeader = codeEntry->itemData[0];
      unsigned long codeType    = getWord(codeHeader, 0, true);
      
      if ((codeType != 0x300) && (codeType != 0x400))
        return GOOL_CODE_SUCCESS;
      
      object *sender = query->sender;
      cspace senderSpace, recipientSpace;
      
      senderSpace.xyz1.X = recipient->bound.xyz1.X + recipient->process.vectors.trans.X;
      senderSpace.xyz1.Y = recipient->bound.xyz1.Y + recipient->process.vectors.trans.Y;
      senderSpace.xyz1.Z = recipient->bound.xyz1.Z + recipient->process.vectors.trans.Z;
      senderSpace.xyz2.X = recipient->bound.xyz2.X + recipient->process.vectors.trans.X;
      senderSpace.xyz2.Y = recipient->bound.xyz2.Y + recipient->process.vectors.trans.Y;
      senderSpace.xyz2.Z = recipient->bound.xyz2.Z + recipient->process.vectors.trans.Z;
     
      recipientSpace.xyz1.X = sender->process.vectors.trans.X - sender->process.vectors.miscB.X;
      recipientSpace.xyz1.Y = sender->process.vectors.trans.Y - sender->process.vectors.miscB.Y;
      recipientSpace.xyz1.Z = sender->process.vectors.trans.Z - sender->process.vectors.miscB.Z;
      recipientSpace.xyz2.X = sender->process.vectors.trans.X + sender->process.vectors.miscB.X;
      recipientSpace.xyz2.Y = sender->process.vectors.trans.Y + sender->process.vectors.miscB.Y; 
      recipientSpace.xyz2.Z = sender->process.vectors.trans.Z + sender->process.vectors.miscB.Z;

      bool colliding = isColliding(&senderSpace, &recipientSpace);
      
      if (!colliding)
        return GOOL_CODE_SUCCESS;

      if ((query->count >= 3) && ((query->count % 5) != 0))
        return ++query->count;
    }
    break;
  }
 
  query->count++;

  object *sender         = query->sender;
  unsigned long event    = query->event;
  unsigned long argCount = query->argCount;
  unsigned long *args    = query->args;
  
  issueEvent(sender, recipient, event, argCount, args);
 
  return GOOL_CODE_SUCCESS;
}
   
//OCTREE ROUTINE

//array is a structure recording collision information from traversal of the 'collision octree' located in item2 of the current zone entry..
//in a single traversal, a number of collision points could be discovered, identified here as n
//m refers to the number of traversals done so far

//struct
//{
//  struct                 //[0x0000] = information from traversal of multiple zone collision octrees with the specified bounding box 
//  {
//    hword zero;
//    hword zoneW;
//    hword zoneH;
//    hword zoneD;
//    hword zero;
//    hword maxDX;
//    hword maxDY;
//    hword maxDZ;
      
//    struct
//    {
//      hword nodeIndex:13;
//      hword nodeLevel:3;
//      hword locationX;
//      hword locationY;
//      hword locationZ;
//    } colPoints[n];
      
//  } traversalInfo[m];     //max 0x1000 bytes
    
//  word flag;              //[0x1000] = flag determining whether or not to record collision information?
//  word curTravIndex;      //[0x1004] = index of current traversal structure * 8, max of 0x200*8=0x10000 bytes
    
//  struct                  
//  {
//    word X1, Y1, Z1;
//    word X2, Y2, Z2;
//  } boundBox;             //[0x1008] = box which is checked for collision points in traversal of the octree
//}
    
    
//starting at 'arrayVal' as index of the new traversal structure and 'oldCol' as the offset (should reflect each other):
//  finds the collision points from 'zoneDim' octree that lie within bounding box 'newBound', recording at oldCol 
//  returns index of next free space (new arrayVal) for a future traversal

void queryNodes(unsigned char *zoneDim, cspace *nodeSpace, unsigned long *output, unsigned long &outputIndex)
{
  signed long zoneX = getWord(zoneDim, 0, true);
  signed long zoneY = getWord(zoneDim, 4, true);
  signed long zoneZ = getWord(zoneDim, 8, true);
  unsigned long zoneW = getWord(zoneDim, 0xC, true);
  unsigned long zoneH = getWord(zoneDim, 0x10, true);
  unsigned long zoneD = getWord(zoneDim, 0x14, true);
  
  unsigned short zoneRootNode = getHword(zoneDim, 0x1C, true);
  unsigned short zoneMaxNodeW = getHword(zoneDim, 0x1E, true);
  unsigned short zoneMaxNodeH = getHword(zoneDim, 0x20, true);
  unsigned short zoneMaxNodeD = getHword(zoneDim, 0x22, true);
  
  cvolume initDivision = { { 0, 0, 0 } , { (zoneW << 8), (zoneH << 8), (zoneD << 8) } };
  
  cbound nodeBound = { 
                         { (zoneX << 8) - nodeSpace->xyz1.X,            
                           (zoneY << 8) - nodeSpace->xyz1.Y,             
                           (zoneZ << 8) - nodeSpace->xyz1.Z },
                         { nodeSpace->xyz2.X - nodeSpace->xyz1.X, 
                           nodeSpace->xyz2.Y - nodeSpace->xyz1.Y, 
                           nodeSpace->xyz2.Z - nodeSpace->xyz1.Z } 
                       };
  
  
  unsigned long headerIndex = outputIndex*2;
  
  output[headerIndex]   =        (zoneW << 16) |              0;
  output[headerIndex+1] =        (zoneD << 16) |        (zoneH);
  output[headerIndex+2] = (zoneMaxNodeW << 16) |              0;
  output[headerIndex+3] = (zoneMaxNodeD << 16) | (zoneMaxNodeH);
 
  outputIndex += 2;  //we filled 2 pairs of longs
 
  queryNodesR(zoneDim, zoneRootNode, initDivision, nodeBound, 0, output, outputIndex);

  //output[(outputIndex*2)] = 0xFFFF;  
}
                         
void queryNodesR(unsigned char *zoneDim, unsigned short node, cvolume division, cbound nodeBound, int level, unsigned long *output, unsigned long &outputIndex)
{  

  //note: division, or initial partition's components-in the assembly (i.e. zone coordinates and dimensions)
  //are shifted left 8 (probably to avoid precision errors when recursively dividing 
  //partitions in half)
  //this should be taken into account for future conversions of the assembly that add/
  //subtract/compare constant values with the retrieved node locations/dimensions 
  //(stored at the code portion below for nodes w/bit 1 set)
    
  if (node & 1)
  {
    if (outputIndex < 512)
    {
      //store location of node *relative to TEST BOX 
      //**and level for future calculation  of node width/height/depth
      //unsigned long valA = (((node << 2) & 0xFFF8) | level) //at | 
      //                    | ((division.pos.X /*>> 4*/) << 16);         //t5           
      //unsigned long valB = ((division.pos.Y /*>> 4*/) & 0xFFFF) |      //t6
      //                     ((division.pos.Z /*>> 4*/) << 16);          //t7
      unsigned long valA = (((node << 2) & 0xFFF8) | level)   |          //at 
                           ((nodeBound.xyz1.X >> 4) << 16);              //t5           
      unsigned long valB = ((nodeBound.xyz1.Y >> 4) & 0xFFFF) |          //t6
                           ((nodeBound.xyz1.Z >> 4) << 16);              //t7
                                                
      //we need to shift right 4 because nodeBound.xyz1.X Y or Z is
      //at max 20 bits and we can only store 16, so we can ignore the
      //lower 4 rounding to nearest mult of 16
      
      output[(outputIndex*2)]   = valA;
      output[(outputIndex*2)+1] = valB;
      
      outputIndex++;
    }
  }
  else if (node != 0)
  {
    unsigned short maxNodeW = getHword(zoneDim, 0x1E, true);
    unsigned short maxNodeH = getHword(zoneDim, 0x20, true);
    unsigned short maxNodeD = getHword(zoneDim, 0x22, true);
    
    unsigned char *nodeChildren = zoneDim + node;
    
    cvolume subdivision = { { division.pos.X, division.pos.Y, division.pos.Z }, 
                            { division.dim.W, division.dim.H, division.dim.D } };  

    //has left, right, up, down, front, back
    bool hasLR, hasUD, hasFB;
    hasLR = level < maxNodeW;
    hasUD = level < maxNodeH;
    hasFB = level < maxNodeD;
    
    unsigned char flags = (hasLR << 0) | (hasUD << 1) | (hasFB << 2);
     
    //front, up, left
    //front, up, right
    //front, down, left
    //front, down, right
    //back, up, left,
    //back, up, right
    //back, down, left
    //back, down, right
    
    signed long oldX1, oldY1, oldZ1;
    signed long oldX2, oldY2, oldZ2;
    signed long oldW, oldH, oldD;
    signed long oldPX, oldPY, oldPZ;
    
    unsigned short children[8];
    
    switch (flags)
    {
      case 1:
      case 2:
      case 4:
      children[0] = getHword(nodeChildren, 0, true);
      children[1] = getHword(nodeChildren, 2, true);
      
      if (flags == 1)
      {
        oldW  = subdivision.dim.W;
        oldPX = subdivision.pos.X;
        oldX1 = nodeBound.xyz1.X;
        oldX2 = nodeBound.xyz2.X;
        
        subdivision.dim.W /= 2;
        if ((nodeBound.xyz1.X + subdivision.dim.W) >= 0)
        {
          //nodeBound.xyz2.X /= 2;
          queryNodesR(zoneDim, children[0], subdivision, nodeBound, level+1, output, outputIndex);
          //nodeBound.xyz2.X = oldX2;
        }
        if ((nodeBound.xyz1.X + subdivision.dim.W) <= nodeBound.xyz2.X)
        {
          nodeBound.xyz1.X  += subdivision.dim.W;
          //nodeBound.xyz2.X  /= 2;
          subdivision.pos.X += subdivision.dim.W;
          queryNodesR(zoneDim, children[1], subdivision, nodeBound, level+1, output, outputIndex);
          nodeBound.xyz1.X  = oldX1;
          //nodeBound.xyz2.X  = oldX2;
          subdivision.pos.X = oldPX;
        }
        subdivision.dim.W = oldW;
      }
      else if (flags == 2)
      { 
        oldH  = subdivision.dim.H;
        oldPY = subdivision.pos.Y;
        oldY1 = nodeBound.xyz1.Y;
        oldY2 = nodeBound.xyz2.Y;
        
        subdivision.dim.H /= 2;
        if ((nodeBound.xyz1.Y + subdivision.dim.H) >= 0)
        {
          //nodeBound.xyz2.Y /= 2;
          queryNodesR(zoneDim, children[0], subdivision, nodeBound, level+1, output, outputIndex);
          //nodeBound.xyz2.Y = oldY2;
        }
        if ((nodeBound.xyz1.Y + subdivision.dim.H) <= nodeBound.xyz2.Y)
        {
          nodeBound.xyz1.Y  += subdivision.dim.H;
          //nodeBound.xyz2.Y  /= 2;
          subdivision.pos.Y += subdivision.dim.H;
          queryNodesR(zoneDim, children[1], subdivision, nodeBound, level+1, output, outputIndex);
          nodeBound.xyz1.Y  = oldY1;
          //nodeBound.xyz2.Y  = oldY2;
          subdivision.pos.Y = oldPY;
        } 
        subdivision.dim.H = oldH;        
      }
      else
      {
        oldD  = subdivision.dim.D;
        oldPZ = subdivision.pos.Z;
        oldZ1 = nodeBound.xyz1.Z;
        oldZ2 = nodeBound.xyz2.Z;
        
        subdivision.dim.D /= 2;
        if ((nodeBound.xyz1.Z + subdivision.dim.D) >= 0)
        {
          //nodeBound.xyz2.Z /= 2;
          queryNodesR(zoneDim, children[0], subdivision, nodeBound, level+1, output, outputIndex);
          //nodeBound.xyz2.Z = oldZ2;
        }
        if ((nodeBound.xyz1.Z + subdivision.dim.D) <= nodeBound.xyz2.Z)
        {
          nodeBound.xyz1.Z  += subdivision.dim.D;
          //nodeBound.xyz2.Z  /= 2;
          subdivision.pos.Z += subdivision.dim.D;
          queryNodesR(zoneDim, children[1], subdivision, nodeBound, level+1, output, outputIndex);
          nodeBound.xyz1.Z  = oldZ1;
          //nodeBound.xyz2.Z  = oldZ2;
          subdivision.pos.Z = oldPZ;          
        }    
        subdivision.dim.D = oldD;
      }
      
      break;
      
      case 3:
      case 5:
      case 6:
      children[0] = getHword(nodeChildren, 0, true);
      children[1] = getHword(nodeChildren, 2, true);
      children[2] = getHword(nodeChildren, 4, true);
      children[3] = getHword(nodeChildren, 6, true);
      
      if (flags == 3)
      {      
        oldW  = subdivision.dim.W;
        oldH  = subdivision.dim.H;
        oldPX = subdivision.pos.X;
        oldPY = subdivision.pos.Y;
        oldX1 = nodeBound.xyz1.X;
        oldY1 = nodeBound.xyz1.Y;
        oldX2 = nodeBound.xyz2.X;
        oldY2 = nodeBound.xyz2.Y;

        bool left, right, top, bot;
        
        subdivision.dim.W /= 2;
        subdivision.dim.H /= 2;
       
        left = (nodeBound.xyz1.X + subdivision.dim.W) >= 0;
        right = (nodeBound.xyz1.X + subdivision.dim.W) <= nodeBound.xyz2.X;
        top = (nodeBound.xyz1.Y + subdivision.dim.H) >= 0;
        bot = (nodeBound.xyz1.Y + subdivision.dim.H) <= nodeBound.xyz2.Y;

        //nodeBound.xyz2.X /= 2;
        //nodeBound.xyz2.Y /= 2;
        if (left && top)
        {
          queryNodesR(zoneDim, children[0], subdivision, nodeBound, level+1, output, outputIndex);
        }
        if (left && bot)
        {
          nodeBound.xyz1.Y  += subdivision.dim.H;
          subdivision.pos.Y += subdivision.dim.H;
          queryNodesR(zoneDim, children[1], subdivision, nodeBound, level+1, output, outputIndex);
          nodeBound.xyz1.Y  = oldY1;
          subdivision.pos.Y = oldPY;  
        }      
        if (right && top)
        {
          nodeBound.xyz1.X  += subdivision.dim.W;
          subdivision.pos.X += subdivision.dim.W;
          queryNodesR(zoneDim, children[2], subdivision, nodeBound, level+1, output, outputIndex);
          nodeBound.xyz1.X   = oldX1;
          subdivision.pos.X  = oldPX;
        }
        if (right && bot)
        {
          nodeBound.xyz1.Y  += subdivision.dim.H;
          subdivision.pos.Y += subdivision.dim.H;
          nodeBound.xyz1.X  += subdivision.dim.W;
          subdivision.pos.X += subdivision.dim.W;
          queryNodesR(zoneDim, children[3], subdivision, nodeBound, level+1, output, outputIndex);
          nodeBound.xyz1.Y   = oldY1;
          subdivision.pos.Y  = oldPY;
          nodeBound.xyz1.X   = oldX1;
          subdivision.pos.X  = oldPX;
        }
        //nodeBound.xyz2.X = oldX2;
        //nodeBound.xyz2.Y = oldY2;
        
        subdivision.dim.W = oldW;
        subdivision.dim.H = oldH;
      }
      else if (flags == 5)
      {
        oldW  = subdivision.dim.W;
        oldD  = subdivision.dim.D;
        oldPX = subdivision.pos.X;
        oldPZ = subdivision.pos.Z;
        oldX1 = nodeBound.xyz1.X;
        oldZ1 = nodeBound.xyz1.Z;
        oldX2 = nodeBound.xyz2.X;
        oldZ2 = nodeBound.xyz2.Z;
        
        bool left, right, front, back;
       
        subdivision.dim.W /= 2;
        subdivision.dim.D /= 2;
        
        left = (nodeBound.xyz1.X + subdivision.dim.W) >= 0;
        right = (nodeBound.xyz1.X + subdivision.dim.W) <= nodeBound.xyz2.X;
        front = (nodeBound.xyz1.Z + subdivision.dim.D) >= 0;
        back = (nodeBound.xyz1.Z + subdivision.dim.D) <= nodeBound.xyz2.Z;

        //nodeBound.xyz2.X /= 2;
        //nodeBound.xyz2.Z /= 2;
        if (left && front)
        {
          queryNodesR(zoneDim, children[0], subdivision, nodeBound, level+1, output, outputIndex);
        }
        if (left && back)
        {
          nodeBound.xyz1.Z  += subdivision.dim.D;
          subdivision.pos.Z += subdivision.dim.D;
          queryNodesR(zoneDim, children[1], subdivision, nodeBound, level+1, output, outputIndex);
          nodeBound.xyz1.Z   = oldZ1;
          subdivision.pos.Z  = oldPZ;   
        }      
        if (right && front)
        {
          nodeBound.xyz1.X  += subdivision.dim.W;
          subdivision.pos.X += subdivision.dim.W;
          queryNodesR(zoneDim, children[2], subdivision, nodeBound, level+1, output, outputIndex);
          nodeBound.xyz1.X   = oldX1;
          subdivision.pos.X  = oldPX;
        }
        if (right && back)
        {
          nodeBound.xyz1.Z  += subdivision.dim.D;
          subdivision.pos.Z += subdivision.dim.D;
          nodeBound.xyz1.X  += subdivision.dim.W;
          subdivision.pos.X += subdivision.dim.W;
          queryNodesR(zoneDim, children[3], subdivision, nodeBound, level+1, output, outputIndex);
          nodeBound.xyz1.Z   = oldZ1;
          subdivision.pos.Z  = oldPZ;
          nodeBound.xyz1.X   = oldX1;
          subdivision.pos.X  = oldPX;
        } 
        //nodeBound.xyz2.X = oldX2;
        //nodeBound.xyz2.Z = oldZ2;
        
        subdivision.dim.W = oldW;
        subdivision.dim.D = oldD;
      }
      else
      {      
        oldH  = subdivision.dim.H;
        oldD  = subdivision.dim.D;
        oldPY = subdivision.pos.Y;
        oldPZ = subdivision.pos.Z;
        oldY1 = nodeBound.xyz1.Y;
        oldZ1 = nodeBound.xyz1.Z;
        oldY2 = nodeBound.xyz2.Y;
        oldZ2 = nodeBound.xyz2.Z;
        
        bool top, bot, front, back;
       
        subdivision.dim.H /= 2;
        subdivision.dim.D /= 2;
        
        top = (nodeBound.xyz1.Y + subdivision.dim.H) >= 0;
        bot = (nodeBound.xyz1.Y + subdivision.dim.H) <= nodeBound.xyz2.Y;
        front = (nodeBound.xyz1.Z + subdivision.dim.D) >= 0;
        back = (nodeBound.xyz1.Z + subdivision.dim.D) <= nodeBound.xyz2.Z;

        //nodeBound.xyz2.Y /= 2;
        //nodeBound.xyz2.Z /= 2;
        if (front && top)
        {
          queryNodesR(zoneDim, children[0], subdivision, nodeBound, level+1, output, outputIndex);
        }
        if (back && top)
        {
          nodeBound.xyz1.Z  += subdivision.dim.D;
          subdivision.pos.Z += subdivision.dim.D;
          queryNodesR(zoneDim, children[1], subdivision, nodeBound, level+1, output, outputIndex);
          nodeBound.xyz1.Z   = oldZ1;
          subdivision.pos.Z  = oldPZ;   
        }     
        if (front && bot)
        {
          nodeBound.xyz1.Y  += subdivision.dim.H;
          subdivision.pos.Y += subdivision.dim.H;
          queryNodesR(zoneDim, children[2], subdivision, nodeBound, level+1, output, outputIndex);
          nodeBound.xyz1.Y   = oldY1;
          subdivision.pos.Y  = oldPY;
        }
        if (back && bot)
        {
          nodeBound.xyz1.Z  += subdivision.dim.D;
          subdivision.pos.Z += subdivision.dim.D;
          nodeBound.xyz1.Y  += subdivision.dim.H;
          subdivision.pos.Y += subdivision.dim.H;
          queryNodesR(zoneDim, children[3], subdivision, nodeBound, level+1, output, outputIndex);
          nodeBound.xyz1.Z   = oldZ1;
          subdivision.pos.Z  = oldPZ;
          nodeBound.xyz1.Y   = oldY1;
          subdivision.pos.Y  = oldPY;
        }  
        //nodeBound.xyz2.Y = oldY2;
        //nodeBound.xyz2.Z = oldZ2;
        
        subdivision.dim.H = oldH;
        subdivision.dim.D = oldD;        
      }
      break;
      
      case 7:
      children[0] = getHword(nodeChildren, 0, true);
      children[1] = getHword(nodeChildren, 2, true);
      children[2] = getHword(nodeChildren, 4, true);
      children[3] = getHword(nodeChildren, 6, true);
      children[4] = getHword(nodeChildren, 8, true);
      children[5] = getHword(nodeChildren, 0xA, true);
      children[6] = getHword(nodeChildren, 0xC, true);
      children[7] = getHword(nodeChildren, 0xE, true);

      oldW  = subdivision.dim.W;
      oldH  = subdivision.dim.H;
      oldD  = subdivision.dim.D;
      oldPX = subdivision.pos.X;
      oldPY = subdivision.pos.Y;
      oldPZ = subdivision.pos.Z;
      oldX1 = nodeBound.xyz1.X;
      oldY1 = nodeBound.xyz1.Y;
      oldZ1 = nodeBound.xyz1.Z;
      oldX2 = nodeBound.xyz2.X;
      oldY2 = nodeBound.xyz2.Y;
      oldZ2 = nodeBound.xyz2.Z;
        
      bool left, right, top, bot, front, back;
        
      subdivision.dim.W /= 2;
      subdivision.dim.H /= 2;
      subdivision.dim.D /= 2;
      
      left = (nodeBound.xyz1.X + subdivision.dim.W) >= 0;
      right = (nodeBound.xyz1.X + subdivision.dim.W) <= nodeBound.xyz2.X;
      top = (nodeBound.xyz1.Y + subdivision.dim.H) >= 0;
      bot = (nodeBound.xyz1.Y + subdivision.dim.H) <= nodeBound.xyz2.Y;
      front = (nodeBound.xyz1.Z + subdivision.dim.D) >= 0;
      back = (nodeBound.xyz1.Z + subdivision.dim.D) <= nodeBound.xyz2.Z;

      //nodeBound.xyz2.X /= 2;
      //nodeBound.xyz2.Y /= 2;
      //nodeBound.xyz2.Z /= 2;
      if (left && top && front)
      {
        queryNodesR(zoneDim, children[0], subdivision, nodeBound, level+1, output, outputIndex);
      }
      if (left && top && back)
      {
        nodeBound.xyz1.Z  += subdivision.dim.D;
        subdivision.pos.Z += subdivision.dim.D;
        queryNodesR(zoneDim, children[1], subdivision, nodeBound, level+1, output, outputIndex);
        nodeBound.xyz1.Z   = oldZ1;
        subdivision.pos.Z  = oldPZ; 
      }      
      if (left && bot && front)
      {
        nodeBound.xyz1.Y  += subdivision.dim.H;
        subdivision.pos.Y += subdivision.dim.H;
        queryNodesR(zoneDim, children[2], subdivision, nodeBound, level+1, output, outputIndex);
        nodeBound.xyz1.Y   = oldY1;
        subdivision.pos.Y  = oldPY;
      }
      if (left && bot && back)
      {
        nodeBound.xyz1.Z  += subdivision.dim.D;
        subdivision.pos.Z += subdivision.dim.D;
        nodeBound.xyz1.Y  += subdivision.dim.H;
        subdivision.pos.Y += subdivision.dim.H;
        queryNodesR(zoneDim, children[3], subdivision, nodeBound, level+1, output, outputIndex);
        nodeBound.xyz1.Z   = oldZ1;
        subdivision.pos.Z  = oldPZ;
        nodeBound.xyz1.Y   = oldY1;
        subdivision.pos.Y  = oldPY;
      } 
      if (right && top && front)
      {
        nodeBound.xyz1.X  += subdivision.dim.W;
        subdivision.pos.X += subdivision.dim.W;
        queryNodesR(zoneDim, children[4], subdivision, nodeBound, level+1, output, outputIndex);
        nodeBound.xyz1.X   = oldX1;
        subdivision.pos.X  = oldPX;
      }
      if (right && top && back)
      {
        nodeBound.xyz1.Z  += subdivision.dim.D;
        subdivision.pos.Z += subdivision.dim.D;
        nodeBound.xyz1.X  += subdivision.dim.W;
        subdivision.pos.X += subdivision.dim.W;
        queryNodesR(zoneDim, children[5], subdivision, nodeBound, level+1, output, outputIndex);
        nodeBound.xyz1.Z   = oldZ1;
        subdivision.pos.Z  = oldPZ;
        nodeBound.xyz1.X   = oldX1;
        subdivision.pos.X  = oldPX;
      } 
      if (right && bot && front)
      {
        nodeBound.xyz1.Y  += subdivision.dim.H;
        subdivision.pos.Y += subdivision.dim.H;
        nodeBound.xyz1.X  += subdivision.dim.W;
        subdivision.pos.X += subdivision.dim.W;
        queryNodesR(zoneDim, children[6], subdivision, nodeBound, level+1, output, outputIndex);
        nodeBound.xyz1.Y   = oldY1;
        subdivision.pos.Y  = oldPY;
        nodeBound.xyz1.X   = oldX1;
        subdivision.pos.X  = oldPX;
      }    
      if (right && bot && back)
      {
        nodeBound.xyz1.Z  += subdivision.dim.D;
        subdivision.pos.Z += subdivision.dim.D;
        nodeBound.xyz1.Y  += subdivision.dim.H;
        subdivision.pos.Y += subdivision.dim.H;
        nodeBound.xyz1.X  += subdivision.dim.W;
        subdivision.pos.X += subdivision.dim.W;
        queryNodesR(zoneDim, children[7], subdivision, nodeBound, level+1, output, outputIndex);
        nodeBound.xyz1.Z   = oldZ1;
        subdivision.pos.Z  = oldPZ;
        nodeBound.xyz1.Y   = oldY1;
        subdivision.pos.Y  = oldPY;
        nodeBound.xyz1.X   = oldX1;
        subdivision.pos.X  = oldPX;
      }
      
      //nodeBound.xyz2.X = oldX2;
      //nodeBound.xyz2.Y = oldY2;
      //nodeBound.xyz2.Z = oldZ2;
      
      subdivision.dim.W = oldW;
      subdivision.dim.H = oldH;
      subdivision.dim.D = oldD;
        
      break;
    }
  }
}

//HANDLES THE PROCESS ANIMATION STRUCT
//(this is where the object model is displayed)
void renderObject(object *obj)
{
  unsigned long frame = obj->process.aniFrame >> 8;
  unsigned char *anim = obj->process.aniSeq;
  
  unsigned char modelType = anim[0];
  
  //if      (byte == 2) { jmp 1def0; }
  //else if (byte >  3)
  //{
  //  if      (byte == 4) { jmp 1e168; }
  //  else if (byte == 5) { jmp 1e028; }
  //  else
  //    return byte;
  //}
  //else if (byte == 1) { jmp 1e198; }
  //else
  //return byte;

  switch (modelType)
  {
    case 2: 
    { 
      //process[0x108] = 
      //{
      //  byte      mode;
      //  byte         ?;
      //  byte         ?;
      //  byte         ?;
      //  word       EID;   //after EID_PROCESS refers to a value rather than an entry with: ???????? ????OOOO OOOOOOOO F??YYY??
         
      //  //for mode == 2  //           Y=texture page Y ID, F=texture flag, O=palette offset from current table (can go into other tables)       struct
      //  {
      //    word colorinfo;   //?SS?CCCC   S=semi-transparency mode, C=color offset into palette
      //    word textureinfo;
      //  } ctInfo[N];
      //}    
   
      //s3 = (float)abs(process[0x98])/27279;
      
      unsigned long squareSize = abs(obj->process.vectors.scale.X)/27279;
      
      //if (obj->process.statusB & 0x200000 == 0)
      //{
      //  ptr    =    0x57888;  //object rotation matrix
      //  var_50 = *(0x57844);  //global rotation matrix
      // }
      //else
      //{
      //  ptr    =    0x57864;  //use this one instead
      //  var_50 = *(0x577E4);
      //}
      
      //for projection distance, zone visibility depth, some other flag?
      //unsigned char *currentZoneHeader = currentZone->itemData[0];
      //var_4C = *(0x578D0);        
      //var_48 = currentZoneHeader[0x2E8]/256;
      //var_44 = ((obj->process.statusB >> 18) ^ 1) & 1;  //i.e. bit 18 reversed

      //pre rotation/translation function for this primitive type
      //result = sub_8003A144(&process[0x80], ptr, process[0xCC] & 0x200, s3, var_50, var_4C, var_48, var_44);    //something with additinoal rotation
      //if (result == 0) { return 0; }
      
      //with models.c we'll do the translation/scaling for the actual primitive
      //specified in the structure itself
      
      //structwithEID = process[0x108];
      //EID         = structwithEID[4];
      //entry       = EID_PROCESS(EID);
      unsigned long texEID = getWord(anim, 4, true);
      
      //transPolygons = sub_80016E18();        //i.e. *(0x5840C + 0x78);
      //models.c handles the primitive allocation....
      
      //texInfoOffset (val) = (process[0x10C] >> 8) * 8;       //process[0x10C] left half determines which of the color/textureinfo structs to use
      //a.k.a frame
      //we need this for the offset of the texture information for that frame
      unsigned long texInfoOffset = (frame * 8);
      unsigned char *texInfoArray = &anim[8 + texInfoOffset]; 
      
      //v0     = (*(0x578D0) + (*(0x578D0) >> 31)) >> 1;
      //camDist =          process[0x138] + (0x800 - v0);    //process[0x138] = current Z? so this gets distance from *(0x578D0) camZ?       
      //we haven't implemented the Z-sorting yet
      
      //return sub_8003A76C(*(0x5840C) + 0x88, &structwithEID[8 + val], entryorisit, 0xC8 << s3, camDist, transPolygons, 0x51774);  
      //-this just becomes a call to the overloaded addObjectPrim,
      // where arguments convert respectively:
      // 1) there is no need to specify the location for an independent Z-buffer (this is sorted version of actual primitive buffer)
      // 2) i.e. texInfoArray
      // 3) i.e. texEID (not processed; we do the resolution in models.c)
      // 4) i.e. 0xC8 << squareSize
      // 5) we don't do z-sorting (yet) and this value is the determinant for the model priority in the z-sort
      // 6) there is no need to specify the location for primitive storage
      // 7) ...is this trans or global rot? or did i copy wrong? unneeded since we have the global matrices set up
      //    perhaps the different global matrices are different configurations of the modelview/etc matrices
       
      crashSystemSpriteModel(texEID, texInfoArray, 0xC8 << squareSize, &obj->process.vectors.trans);
      
      //creates and transforms a new 'square' with size given by scalar
      //structwithEIDloc indicates a structure that has RGB value, and texture information
      //entryfromstruct contains texture page Y index (y indexes have 4 pages), texture coord flag, and palette offset for CLUT modes 
      //-array is a pointer to a list of default texture page, clut values
      //-transpolygons is a pointer to where we will store the transformed square
      //-drawdispbufloc is a pointer to the draw/display buffer structure where we store a pointer to the new transformed square
      // in a z-sorted list so it can be rendered
      //-proccalc is (object distance from camera?) so polygons can be Z-sorted relative to this distance
      
      //sub_8003A76C(drawdispbufloc, structwithEIDloc, entryfromstruct, scalar, proccalc, transpolygons, array)
      
      //MAYBE used in this context it creates the shadows for an object, since they are all squares in this game,
      //size determined by 0xC8 << s3
    }
    break;
    
    case 5:
    {
      
      //val = abs(process[0x98]);  //a3
      //integral = HIWORD(val * 0x26705EBF);  //the integral portion of the product
      //s2 = (integral >> 12) - (val >> 31);   //val>>31 is highest bit of val
     
      //s2 = process[0x98]/27279;
      unsigned long fragmentSize = abs(obj->process.vectors.scale.X)/27279;

      //gEntry =   *(0x57914);
      //gInfo  = gEntry[0x10];
      //ptr    =    0x57888;
      //var_50 =    0x57844; 
      //var_4C = *(0x578D0);
      //var_48 = gInfo[0x2E8]/256;
      //var_44 = ((process[0xCC] >> 18) ^ 1) & 1;  //i.e. bit 18 reversed
     
      //pre rotation/translation function for this primitive type
      //result = sub_8003A144(&process[0x80], ptr, process[0xCC] & 0x200, s2);
      
      //if (result == 0) { return 0; }
     
      //process[0x108] = 
      //{
      //  byte      mode;
      //  byte         ?;
      //  byte         ?;
      //  byte         ?;
      //  word       EID;   //after EID_PROCESS refers to a value rather than an entry with: ???????? ????OOOO OOOOOOOO F??YYY??
         
      // for mode == 5  //           Y=texture page Y ID, F=texture flag, O=palette offset from current table (can go into other tables)     word groupSize;
      //   word groupSize;
      //   
      //   struct
      //   {
      //     word colorinfo;   //?SS?CCCC   S=semi-transparency mode, C=color offset into palette
      //     word textureinfo;
      //     
      //     struct rect
      //     {
      //       hword y2;
      //       hword x2;
      //       hword y1;
      //       hword x1;
      //     }
      //   } rectInfo[G][groupSize];  //where G=any group
      // }    
     
      //structwithEID =   process[0x108];
   
      //EID           = structwithEID[4];  
      //entry         = EID_PROCESS(EID);  //s6
      unsigned long texEID = getWord(anim, 4, true);

      //numRects      = structwithEID[8];  //number of rects in one group
      //val  =     (process[0x10C] >> 8);
      unsigned long quadCount = getWord(anim, 8, true);
      
      //left half of process[0x10C] refers to a certain group of rectInfos
      //offset        =  (numRects * val) * 16;  //last*val is HIWORD result
      unsigned long fragInfoOffset = (frame * (8 + (quadCount*8)));

      //rectInfo = &structwithEID[0xC + offset];
      unsigned char *texInfoArray  = &anim[fragInfoOffset];
      unsigned char *fragInfoArray = &anim[fragInfoOffset + 8];

      if (quadCount <= 0) { return; } 
     
      //we have to manually rearrange and swap endianness of the quad coordinates
      csquad quads[0x100];
      for (int lp=0; lp<quadCount; lp++)
      {
        quads[lp].Y2 = getHword(fragInfoArray, (0+(lp*4))*2, true);
        quads[lp].X2 = getHword(fragInfoArray, (1+(lp*4))*2, true);
        quads[lp].Y1 = getHword(fragInfoArray, (2+(lp*4))*2, true);
        quads[lp].X1 = getHword(fragInfoArray, (3+(lp*4))*2, true);
      }
      
      //count = 0;
      //do
      //{
      //  var_40 = rectInfo[8]   << s2;   //re
      //  var_3C = rectInfo[0xA] << s2;
      //  var_34 = rectInfo[0xC] << s2;
      //  var_30 = rectInfo[0xE] << s2;
      //
      //  var_50 = *(0x5840C) + 0x88;
      //
      //  //creates/transforms a rectangle with coords specified by var_40 (stack ptr)
      //  //with texture and color info specified by entry
      //  sub_80018B98(&rectInfo, process[0x138], entry, &var_40, var_50);    //cop2
      //  rectInfo += 0x10;
      //
      //  count++;
      //} while (count < numRects)    //for all 16 byte structures in this group
      
      crashSystemFragmentModel(texEID, 
                              quadCount, quads, texInfoArray, 
                              fragmentSize, &obj->process.vectors.trans); 
                              
      break;
    }
    case 4:
    {
      //var_50 = process[0xCC] & 0x400;
      //return sub_8001E7D8(process, process[0x108], process[0x10C]>>8, *(0x5840C) + 0x88);
    }
    break;
  
    case 1:
    {
      unsigned long svtxEID   = getWord(anim, 4, true);
      entry *svtx             = crashSystemPage(svtxEID);

      //itemIndex = ((process[0x10C]>>8) * 4);
      //itemN     =   entry[0x10 + itemIndex];
      //if (entry.type == 0x14)                              //T20, variation of animation?
      //sub_80018A40(itemN, *(0x5840C) + 0x88, process);
      //else                                                 //T1
      //sub_80018964(itemN, *(0x5840C) + 0x88, process);   
   
      crashSystemObjectModel(svtx, frame, 
                   &obj->process.vectors.trans, 
                   &obj->process.vectors.rot, 
                   &obj->process.vectors.scale,
                   &obj->colors.lightmatrix,
                   &obj->colors.colormatrix,
                   &obj->colors.color,
                   &obj->colors.intensity);
    
      //object shading flag
      //if (process[0xCC] & 0x100000 == 0) { return 0; }

      if (obj->process.statusB & 0x100000)
      {
        //if (gp[0x2B8] == process)
        //{
        //graphicsEntry = process[0x28];
        //item1   = graphicsEntry[0x10];
        if (crash == obj)
        {
          entry *zoneEntry          = obj->zone;
          unsigned char *zoneHeader = zoneEntry->itemData[0];
        
          //processOffset = process +  0x30;
          //itemOffset    = item1   + 0x348;
     
          //if (((a2 | a3) & 3) != 0)
          //copyInterleave(processOffset, itemOffset, 12, sizeof(word), 3); //copy 12 words, but dont copy the3rdbyt 
          //else
          //copy(processOffset, itemOffset, 12, sizeof(word));
          copyColors(obj, zoneHeader, 0x348);
        }
        else
        {
          //graphicsEntry = process[0x28];
          entry *zoneEntry    = obj->zone;
          if (!zoneEntry)
            zoneEntry = currentZone;
          unsigned char *zoneHeader = zoneEntry->itemData[0];

          //processOffset = process +  0x30;
          //itemOffset    = item1   + 0x318;
     
          //if (((a2 | a3) & 3) != 0)
          //copyInterleave(processOffset, itemOffset, 12, sizeof(word), 3); //copy 12 words, but dont copy the3rdbyt 
          //else
          //copy(processOffset, itemOffset, 12, sizeof(word));
          copyColors(obj, zoneHeader, 0x318);
        }
      }
    }
    
    break;
  }
  
  return;
}
  
/*sub_80018964(svtxFrame, polygonBufferB, process)
{
  //load the entry for the TGEO model in this SVTX frame
  tgeoEID = svtxFrame[0x4];              
  entry   = EID_PROCESS(tgeoEID); 
  item1   = entry[0x10];      

  //set new translation, rotation, light and color matrices before rendering
  result  = sub_800180CC(item, item1, process, 1, 0);  //last arg is var_30 = 0, arg_10 in sub
 
  //if we could not translate then return error
  if (result == 0) { return 0; }
  
  //determine where to place the new model that will be rendered and 
  //its respective link in the z-buffer
  polygonBuffer = sub_80016E18();
  projval       = (process[0x138] + 0x800) - (((*(0x578D0) >> 31) + *(0x578D0)) >> 1);

  //now render the object model
  item1 = entry[0x10];
  item2 = entry[0x14];
  return sub_8003460C(svtxFrame, polygonBufferB, item2, item1, process[0x98], projval, polygonBuffer, 0x51774, 0);
}*/
    
cspace nodeQuery[0x1000];
unsigned long nodeQueryType[0x1000];
unsigned long nodeQueryLen = 0;

void readyNodeQuery(traversalInfo *trav, cspace *nodeSpace)
{  
  nodeQueryLen = 0;
  
  unsigned long *curStruct = (unsigned long*)trav->traversal;
  
  unsigned long zoneW, zoneH, zoneD;
  unsigned long depthX, depthY, depthZ;
  
  unsigned long sum   = 0;
  unsigned long count = 0;
  
  do
  {  
    if ((curStruct[0] & 0xFFFF) == 0xFFFF)
      break;
    else if ((curStruct[0] & 0xFFFF) == 0)
    {
      zoneW = ((curStruct[0] & 0xFFFF0000) >> 16) << 8; /*curStruct[2] << 8*/  //s4
      zoneH = ((curStruct[1] & 0x0000FFFF) >>  0) << 8; /*curStruct[4] << 8*/  //s5
      zoneD = ((curStruct[1] & 0xFFFF0000) >> 16) << 8; /*curStruct[6] << 8*/  //s6
      
      depthX = (curStruct[2] & 0xFFFF0000) >> 16; /*depthX = curStruct[0xA]*/   //s7
      depthY = (curStruct[3] & 0x0000FFFF) >>  0; /*depthY = curStruct[0xC]*/   //t8
      depthZ = (curStruct[3] & 0xFFFF0000) >> 16; /*depthZ = curStruct[0xE];*/   //t9
        
      curStruct += 4;
    }
    else
    {
      unsigned short nodeValue = ((curStruct[0] & 0xFFF8) >> 2) | 1; 
      unsigned short nodeType = nodeValue & 0xE;

      unsigned char nodeLevel = (curStruct[0] & 7);      
      
      //signed word * 16
      //**** signed values
      //after shifting to break apart the respective values, we need to
      //recover the sign by shifting into the sign bit (<<16); then we
      //recover the original scale by shifting back (>>12) because these 
      //values are stored at a scale of 16
      signed long nodeX = ((signed long)(((curStruct[0] & 0xFFFF0000) >> 16) << 16)) >> 12; 
      signed long nodeY = ((signed long)(((curStruct[1] & 0x0000FFFF) >>  0) << 16)) >> 12; 
      signed long nodeZ = ((signed long)(((curStruct[1] & 0xFFFF0000) >> 16) << 16)) >> 12; 

      signed long nodeX1 = nodeX + nodeSpace->xyz1.X;
      signed long nodeY1 = nodeY + nodeSpace->xyz1.Y;
      signed long nodeZ1 = nodeZ + nodeSpace->xyz1.Z;      
      
      unsigned long nodeW, nodeH, nodeD;
      if (nodeLevel >= depthX) { nodeW = zoneW >> depthX; }
      else                     { nodeW = zoneW >> nodeLevel; }
      if (nodeLevel >= depthY) { nodeH = zoneH >> depthY; }
      else                     { nodeH = zoneH >> nodeLevel; }
      if (nodeLevel >= depthZ) { nodeD = zoneD >> depthZ; }
      else                     { nodeD = zoneD >> nodeLevel; }
      
      signed long nodeX2 = nodeX1 + nodeW;
      signed long nodeY2 = nodeY1 + nodeH;
      signed long nodeZ2 = nodeZ1 + nodeD;

      nodeQueryType[nodeQueryLen] = nodeValue;
      
      cspace *newSpace = &nodeQuery[nodeQueryLen++];
      newSpace->xyz1.X = nodeX1;
      newSpace->xyz1.Y = nodeY1;
      newSpace->xyz1.Z = nodeZ1;
      newSpace->xyz2.X = nodeX2;
      newSpace->xyz2.Y = nodeY2;
      newSpace->xyz2.Z = nodeZ2;
      
      curStruct += 2;
    }
  } while (true);
}
        