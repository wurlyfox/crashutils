#include <iostream>
using namespace std;

#include "goolengine.h"
#include "objectengine.h"    //for definition of the extern cmove struct used by the controller routine
#include "zoneengine.h"      //for instruction that uses findZone
#include "matrix.h"
#include "control.h"         //for controller struct definitions so we can read the controls

#include "../crash_system.h"         //so the gool interpreter can suspend

#include "../gooldisasm/gooldisasm.h"      //for now 

//#include "../gl/gl.h"        //so we can calculate screen coordinates w/gool instruction 0x85

extern NSD *nsd;

object *player;
object *objects;

object *crash;

object initObject;
object headObjects[8];

extern entry *currentZone;
extern unsigned char *oldZoneHeader;

signed long lastConst = 0;    //gp[0x88] in c1
signed long constBuffer[2];   //gp[0x84] in c1

unsigned long globals[0x100];

unsigned short levelStates[COUNT_GAMEOBJECTS];
unsigned long states[COUNT_LEVELOBJECTS];

object *var_60E00;
unsigned long framesElapsed;
unsigned long drawCount = 0;

object *prevBox;
unsigned char *prevBoxEntity;

extern unsigned long globalVelocity;

extern cmove moveStates[16];
extern ctrl controls[2];

int getObjectIndex(object *obj) { return obj-objects; }
 
unsigned long initObjects()
{
  crash = 0;                 //clear Will process

  //pointers at 0x60DB0, 0x60DB4 respectively in crash 1
  player  = (object*)malloc(sizeof(object) + 0x100); 
  objects = (object*)malloc(COUNT_OBJECTS*sizeof(object));
  
  if (objects == 0 || player == 0) { return 0xFFFFFFF1; } 

  var_60E00 = 0;
  framesElapsed = 0;

  //0x60DF8 in crash 1
  initObject.type = 2;
  initObject.children = objects;      
  
  //
  for (int obj = 0; obj < COUNT_OBJECTS; obj++)
  {
    objects[obj].type = 0;
    
    objects[obj].process.parent = &initObject;
    objects[obj].process.children = 0;
    if (obj < (COUNT_OBJECTS-1)) { objects[obj].process.sibling = &objects[obj+1]; }
    else                         { objects[obj].process.sibling = 0; }
  }
    
  for (int obj = 0; obj < 8; obj++)
  {
    headObjects[obj].type = 2;
    headObjects[obj].children = 0;
  }

  player->type = 0;
  player->process.parent = 0;
  player->process.sibling = 0;
  player->process.children = 0;

  return 0xFFFFFF01;
}

inline object *findPrevChild(object *child, object *parent)
{
  object *firstChild = getChildren(parent);
  object *curChild   =          firstChild;
         
  if (curChild->process.sibling == child) { return curChild; }
  
  object *nextChild;
  do
  {
    curChild = curChild->process.sibling;

    if (!curChild) { return 0; }
  
    nextChild = curChild->process.sibling;
    if (!nextChild) { return 0; }

  } while (nextChild != child);
  
  return curChild;
}

inline void addChild(object *child, object *newParent)
{
  child->process.parent  = newParent;
  child->process.sibling = getChildren(newParent);
  setChildren(newParent, child);
}

inline void adoptChild(object *child, object *newParent)
{
  if (hasParent(child))
  {
    object *parent         =    child->process.parent;
    object *parentChildren =      getChildren(parent);
	
	  #define parentfirstChild parentChildren
	
    if (parentfirstChild == child)
    {
      setChildren(parent, child->process.sibling);  
    }
    else
    {
      if (parentChildren)
      {
        object *prev = findPrevChild(child, parent);

        if (prev) 
          prev->process.sibling = child->process.sibling; 
      }
    }
  }
  
  addChild(child, newParent); 
}

unsigned long loadStates(unsigned short levelID)
{
  for (int index = 0; levelStates[index] != 0; index++) 
  {
    unsigned long levelState = levelStates[index];
    unsigned long stateLevID = levelState >> 9;
  
    if (levelID == stateLevID)
    {
      unsigned long stateIndex = levelState & 0x1FF;
      
      states[stateIndex] |= 8;
    }
  }
}

//**FROM HIGHEST/OLDEST GENERATION TO LOWEST/YOUNGEST GENERATION                                                  
//run a routine that takes an argument for a process's entire family 
//terminate any process in the family that makes routine return error code
//0xFFFFFFE7
//[returns the result of calling the routine for the specified process 
//which happens recursively for each process in the family]
//(happens only for processes that are non-null i.e. not type 0)
//sub_8001B648(obj, routine, arg)
unsigned long familyRoutinePBC(object *obj, unsigned long (*routine)(object*,bool), bool arg)
{
  unsigned long result = (*routine)(obj, arg);
  
  if (result == 0xFFFFFFE7)
  { 
    terminateObject(obj, 0);
    
    return 0xFFFFFFE7;
  }
  
  object *children = getChildren(obj);
  if (children)
  {
    object *firstChild = children;
    object *nextChild  = firstChild;
    do
    {
      object *curChild = nextChild;
      if (curChild->type == 0) { break; }
      nextChild        = curChild->process.sibling;
      
      familyRoutinePBC(curChild, routine, arg);
    } while (nextChild);
  }
  
  return result;
}

//**FROM LOWEST/YOUNGEST GENERATION TO HIGHEST/OLDEST GENERATION                                                  
//run a routine that takes an argument for a process's entire family 
//additionally, run 1CDD0 (w/o flag) for all family members that make the routine return 0xFFFFFFE7   
//which happens recursively for each process in the family]                                                  //[returns the result of calling the routine for the specified process      

unsigned long familyRoutineCBP(object *obj, unsigned long (*routine)(object*,void*), void* arg) 
{
  object *children = getChildren(obj);
  if (children)         
  {
    object *firstChild =   children;
    object *nextChild  = firstChild;
   
    do
    {
      object *curChild  =                 nextChild;
      nextChild         = curChild->process.sibling;

      unsigned long result = familyRoutineCBP(curChild, routine, arg); 
                                                          
      if (result == 0xFFFFFFE7)
      {
        terminateObject(obj, 0);  // terminate root rather than child!
      }
    } while (nextChild);
  }
      
  return (*routine)(obj, arg);
}


object *findObject(object *obj, object*(*routine)(object*,unsigned long), unsigned long arg)      //runs a thru a processes entire family till it finds a process that isnt of type 2 and satisfys the specified routine
{
  if (!obj) { return 0; }
  
  object *children;
  if (obj->type != 2)
  {
    object *result = (*routine)(obj, arg);

    if (result) { return result; }
    
    children = getChildren(obj);
  }
  else
    children = obj->children;

  if (!children) { return 0; }

  object *child = children;
  object *result = 0;
  do
  {      
    result = findObject(obj, routine, arg);   
    child  = child->process.sibling;
    
  } while (!result && child);

  return result;
}

object *find_routineFlags(object *obj, unsigned long flags)
{
  if (obj->process.routineFlagsA & flags)
    return obj;
  else
    return 0;
}

//sub_8001B84C(process, routine, arg)
unsigned long familyRoutinePBCH(object *obj, unsigned long (*routine)(object*,bool), bool arg)
{
  object *children = getChildren(obj);

  if (!children) { return GOOL_CODE_SUCCESS; }

  object *firstChild =   children;
  object *nextChild  = firstChild;

  do
  {
    object *curChild = nextChild;
    nextChild = curChild->process.sibling;

    unsigned long result = (*routine)(curChild, arg);

    if (result == 0xFFFFFFE7)
    {
      terminateObject(curChild, 0);
    }
    else
    {
      object *grandchildren = getChildren(curChild); /*curChild->process.children;*/
     
      if (grandchildren)
      {
        object *firstGrandchild =   grandchildren;
        object *nextGrandchild  = firstGrandchild;      
    
        do
        {
          object *curGrandchild = nextGrandchild;
          nextGrandchild = curGrandchild->process.sibling;

          if (curGrandchild->type != 0)
          {
            familyRoutinePBC(curGrandchild, routine, arg);
          }
        } while (nextGrandchild);
      }
    }
  } while (nextChild);

  return GOOL_CODE_SUCCESS;
}

//sub_8001B92C(process, routine, arg)
unsigned long familyRoutineCBPH(object *obj, unsigned long (*routine)(object*,void*), void* arg)   //this is accomplishing what 1B6F0 does for a family of a process, but only with the first 2 generations, but then for 
{                                                                                                //the 2nd generation, 1B6F0 is called, thus kind of making this subroutine redundant-HOWEVER, altogether it                                                              //would accomplish this -without- the recursion for the first 2 gens, therefore we can avoid overflowing the stack
  object *children = getChildren(obj);

  if (!children) { return GOOL_CODE_SUCCESS; }

  object *firstChild =   children;
  object *nextChild  = firstChild;
  
  do
  {
    object *curChild = nextChild;
    nextChild = curChild->process.sibling;  // s5           

    object *grandchildren = getChildren(curChild); //a0 /*curChild->process.children;*/   
 
    if (grandchildren)          
    {
      object *firstGrandchild = grandchildren;   //a0
      object *nextGrandchild  = firstGrandchild;
      
      do
      {
        object *curGrandchild  = nextGrandchild;
        nextGrandchild = curGrandchild->process.sibling;  //s1

        unsigned long result = familyRoutineCBP(curGrandchild, routine, arg);                                                                                

        if (result == 0xFFFFFFE7)
        {
          terminateObject(curChild, 0);
        }
      
      } while (nextGrandchild); 
    }
    
    unsigned long result = (*routine)(obj, arg);

    if (result == 0xFFFFFFE7)
    {
      terminateObject(obj, 0);
    }

  } while (nextChild);

  return GOOL_CODE_SUCCESS;
}

object *hasPID(object *obj, unsigned long PID)
{
  if (obj->process.PIDflags == PID)
    return obj;
  else
    return 0;
}

unsigned long absdistance(unsigned long c1, unsigned long c2)
{
  signed long distance = c1 - c2;
  if (distance < 0)
    distance = c2 - c1;
    
  return distance;
}

object *spawnObject(entry *entityEntry, int entityIndex)
{
  if (entityEntry == 0) { return (object*)0xFFFFFFF2; }

  unsigned char *entityItem;
  if (entityEntry->type == 7) //if the entry who has some items as entitys is a zone
  {
    unsigned char *zoneHeader = entityEntry->itemData[0];
    unsigned char zoneCollisionSkip  = getWord(zoneHeader, 0x204, true);
    unsigned char zoneSectionCount   = getWord(zoneHeader, 0x208, true);
    
    unsigned short entityItemIndex = (zoneCollisionSkip + zoneSectionCount) + entityIndex; 
    entityItem = entityEntry->itemData[entityItemIndex];
  }                      
  else if (entityEntry->type == 17) //if type 0x11 (in game map data?)
  {
    entityItem = entityEntry->itemData[2+entityIndex];
  }   
  else
  {
    entityItem = entityEntry->itemData[entityIndex];
  }
  
  unsigned short bitfield = getHword(entityItem, 0x4, true);
  unsigned short procType = getHword(entityItem, 0x6, true);
  unsigned short PID = getHword(entityItem, 0x8, true);
  unsigned short pathCount = getHword(entityItem, 0xA, true);
  unsigned short argA = getHword(entityItem, 0xC, true);
  unsigned short argB = getHword(entityItem, 0xE, true);
  unsigned short argC = getHword(entityItem, 0x10, true);
  unsigned char type = entityItem[0x12];
  unsigned char subtype = entityItem[0x13];
  unsigned short initX = getHword(entityItem, 0x14, true);
  unsigned short initY = getHword(entityItem, 0x16, true);
  unsigned short initZ = getHword(entityItem, 0x18, true);
  
  bool isCrash = (
                  procType == 3 
               && (type == 0                      || 
                   (PID > 0 && PID < 5)           ||
                   (type == 0x2C && subtype == 0) ||
                   (type == 0x30))
                 );
                 
  //if the entity item is crash and crash already exists then do not spawn
  if (isCrash)
    if (crash) { return (object*)0xFFFFFFF2; }

  //use this variable to indicate whether the previous box is within 10,10,10 of the
  //current, set to the previous box object/process if true, set to 0 if false
  object *prevBoxObj;
  
  //if the entity item is a box
  if (type == 0x22)
  {    
    //if there was a box spawned right before this one, grab previous box object
    if (prevBoxEntity)
    {
      unsigned short prevBoxX = getHword(prevBoxEntity, 0x14, true);
      unsigned short prevBoxY = getHword(prevBoxEntity, 0x16, true);
      unsigned short prevBoxZ = getHword(prevBoxEntity, 0x18, true);
    
      if (
          absdistance(initX, prevBoxX) < 10 &&
          absdistance(initZ, prevBoxZ) < 10 &&
          absdistance(initY, prevBoxY) < 10
         )
      {
        prevBoxObj = prevBox;
      }
      else
        goto clrbox;
    }
    else
    {
      clrbox:
      var_61A60  = 0x19000;
      prevBox    = 0;
      prevBoxObj = 0;
    }
    
    prevBoxEntity = entityItem;
  }

  //grab state of the object/process that occupies the entity item's desired PID slot
  unsigned long procState = states[PID];
  
  if ((procState & 1) || (procState & 2))
  {
    if (type == 0x22)
      var_61A60 += 0x19000;
     
    return (object*)0xFFFFFFF2;
  }

  object *parent;
  object *child;
  unsigned long listProcessIndex;
  if (isCrash)
  {
    crash = player;    //*(0x60DB0) blank process with its own designated page
    child = crash;
    listProcessIndex = 6;
  }
  else
  { 
    object *freeObjectsParent = &initObject;
    object *freeObjects       = getChildren(freeObjectsParent);

    object *victims       = 0;
    object *curVictim     = 0;
    object *flaggedVictim = 0;
    
    do
    {
      //if there are free objects, or the force flag is set 
      //(or no parent has been specified) then grab the first free object
      //if not then we need to find a process to terminate so that we can
      //make a free object in its place, then loop back to here and use
      //this same code to grab that free object
      if (freeObjects)  //if initial process has no children (or referred to a first child that was removed)
      {
        object *freeObject = freeObjects;
        child = freeObject;  //grab the first free object
        listProcessIndex = 3;
        break;
      }
      else
      {
        object *objectListD = &headObjects[3];
        
        if (objectListD->type == 2)              //note that index 1 is the first in the 'list', index 0 is init process        
          victims = objectListD->children;       
        else
        {
          flaggedVictim = find_routineFlags(objectListD, 0x80000);  //returns process in first arg if 'process[0x120]' bit 4 is set
                                                              //else returns 0
          if (flaggedVictim)
            victims = getChildren(objectListD);   
        }
 
        if (victims/*flaggedVictim && curVictim*/)
        {
          curVictim = victims;
          do
          {
            flaggedVictim  = findObject(curVictim, find_routineFlags, 0x80000);  //runs a thru a processes entire family till it finds a process that isnt of type 2 and                                                                                  //satisfys the specified routine
            curVictim      = curVictim->process.sibling;  //keep following links
             
          } while (curVictim && (flaggedVictim == 0));                 //while they exist & a process is not found within the current processes family
        } 

        if (flaggedVictim)                 //if a process was found
        {
          terminateObject(flaggedVictim);  //run sub_8001CDD0 on the process with the flag      
        }
      }
    } while (flaggedVictim);
  }

  //if a free object/child could not be found or created
  //then error for no free objects could be allocated (all objects needed)
  if (child == 0) { return (object*)0xFFFFFFEA; }

  //determine the parent to assign to based on whether the object was determined to
  //be crash   = headObject 6
  //or reg obj = headObject 3
  parent = &headObjects[listProcessIndex];
  
  //allocate the new child to the specified parent
  adoptChild(child, parent); 

  child->subtype = procType;   //field 0x4
  
  unsigned long result = initProcess(child, type, subtype, 0, 0);
 
  object *freeObjects = &initObject;
  if (isErrorCode(result))
    adoptChild(child, freeObjects);

  states[PID]            |=        1;
  child->process.PIDflags = PID << 8;
  
  if (entityEntry)                     //if the entry from the args is valid
  {
    if (entityEntry->type == 0x11)     //if a (map data entry?)
      child->zone   = currentZone;     //we use the global entry with graphicsinfo as item1
    else
      child->zone   = entityEntry;     //0x28: if init arg_a1 not 0xD, 4, or 5, is set to backlink's [0x28] at init if backlink's mode is 1
  }

  if (subtype == 0)
  {  
    unsigned char *zoneHeader = entityEntry->itemData[0];

    //processOffset = child   +  0x30;
    //itemOffset    = item1   + 0x348;
   
    //if (((a2 | a3) & 3) != 0)
    //  copyInterleave(processOffset, itemOffset, 12, sizeof(word), 3); //copy 12 words, but dont copy the 3rd byte
    //else
    //  copy(processOffset, itemOffset, 12, sizeof(word));
    copyColors(child, zoneHeader, 0x348);
    
  }
  else
  {
    unsigned char *zoneHeader = entityEntry->itemData[0];

    //processOffset = child   +  0x30;
    //itemOffset    = item1   + 0x318;

    //if (((a2 | a3) & 3) != 0)
    //  copyInterleave(processOffset, itemOffset, 12, sizeof(word), 3); //copy 12 words, but dont copy the 3rd byte
    //else
    //  copy(processOffset, itemOffset, 12, sizeof(word));
    copyColors(child, zoneHeader, 0x318);
  }

  child->process.entity       = entityItem;
  child->process.pathCountEtc = pathCount << 8;

  if ((bitfield & 1) == 0)
  {
    child->process.vectors.rot.Y = argA;   //from halfwords to word
    child->process.vectors.rot.X = argB;
    child->process.vectors.rot.Z = argC;
  }

  //TODO: FIX VECTOR MISCC AS A CANGLE
  child->process.vectors.miscC.X = argA << 8;   //these are just scaled versions of the values above
  child->process.vectors.miscC.Y = argB << 8;
  child->process.vectors.miscC.Z = argC << 8;

  objectPath(child, 0, &child->process.vectors.trans);
  
  //NO SAVESTATE YET EITHER
  //if (child == crash)
  //{
  //  if (*(0x56714) == -1)
  //  {
  //    sub_80026460(child, 0x57974, 1);   
  //  }
  //}

  //
  entry *codeEntry          =          child->global;
  unsigned char *codeHeader = codeEntry->itemData[0];

  type = getWord(codeHeader, 0, true);
  unsigned long codeSubtype = getWord(codeHeader, 0x4, true);
  
  object *enemies = &headObjects[4];
  if (codeSubtype == 0x300) 
    adoptChild(child, enemies);

  //if entity's code entry refers to type box
  if (type == 0x22)
  {
    //case if this box has been created next to a previous box
    if (prevBoxObj)
    {
      child->process.vectors.boxLink.prev = prevBox;   //this was the old *(0x61A5C) value gotten at beginning of sub
      child->process.vectors.boxLink.next =  0;
      prevBox->process.vectors.boxLink.next = child;
    }
    else
    {
      child->process.vectors.boxLink.prev = 0;
      child->process.vectors.boxLink.next = 0;
    }

    child->process.vectors.trans.X += (0x19000 - var_61A60);

    prevBox = child;   //then it is set right here...

    unsigned char *zoneHeader = child->zone->itemData[0];
    
    unsigned long zoneBits = getWord(zoneHeader, 0x2DC, true);
    if ((zoneBits & 4) == 0)  //bit 3 is tex loading priority???
    {
      unsigned char XorZbit = (((child->process.vectors.trans.Z/16) ^ child->process.vectors.trans.X) & 7);
      child->process.aniCounter = XorZbit;
 
      if (XorZbit)
        child->process.statusB |= 0x10000000;
    }
  }

  return child;
}

object *addObject(object *parent, unsigned long levelListIndex, unsigned long subIDIndex, unsigned long count, unsigned long *args, bool flag)
{
  object *child;
  
  if (levelListIndex == 0 && subIDIndex == 0)
  {
    crash = player;    //*(0x60DB0) blank process with its own designated page
    child = crash;
  }
  else
  { 
    object *freeObjectsParent = &initObject;
    object *freeObjects       = getChildren(freeObjectsParent);

    object *victims       = 0;
    object *curVictim     = 0;
    object *flaggedVictim = 0;
    
    do
    {
      //if there are free objects, or the force flag is set 
      //(or no parent has been specified) then grab the first free object
      //if not then we need to find a process to terminate so that we can
      //make a free object in its place, then loop back to here and use
      //this same code to grab that free object
      if (freeObjects || !flag || !parent)  //if initial process has no children (or referred to a first child that was removed)
      {
        object *freeObject = freeObjects;
        child = freeObject;  //grab the first free object
        break;
      }
      else
      {
        object *objectListD = &headObjects[3];
        
        if (objectListD->type == 2)              //note that index 1 is the first in the 'list', index 0 is init process        
          victims = objectListD->children;       
        else
        {
          flaggedVictim = find_routineFlags(objectListD, 0x80000);  //returns process in first arg if 'process[0x120]' bit 4 is set
                                                              //else returns 0
          if (flaggedVictim)
            victims = getChildren(objectListD);   
        }
 
        if (victims/*flaggedVictim && curVictim*/)
        {
          curVictim = victims;
          do
          {
            flaggedVictim  = findObject(curVictim, find_routineFlags, 0x80000);  //runs a thru a processes entire family till it finds a process that isnt of type 2 and                                                                                  //satisfys the specified routine
            curVictim      = curVictim->process.sibling;  //keep following links
             
          } while (curVictim && (flaggedVictim == 0));                 //while they exist & a process is not found within the current processes family
        } 

        if (flaggedVictim)                 //if a process was found
        {
          terminateObject(flaggedVictim);  //run sub_8001CDD0 on the process with the flag      
        }
      }
    } while (flaggedVictim);
  }

  //if a free object/child could not be found or created
  //then error for no free objects could be allocated (all objects needed)
  if (child == 0) { return (object*)0xFFFFFFEA; }

  //allocate the new child to the specified parent
  adoptChild(child, parent); 

  child->subtype = 3;  //a code process/non spawned by entity

  //process, levelListIndex, subIDIndex, fill, fillVal
  initProcess(child, levelListIndex, subIDIndex, count, args);

  /*
  graphicsEntry = parent[0x28];
  if (graphicsEntry)
  {  
    item1 = graphicsEntry[0x10];

    processOffset = parent  +  0x30;
    itemOffset    = item1   + 0x318;
   
    if (((a2 | a3) & 3) != 0)
      copyInterleave(processOffset, itemOffset, 12, sizeof(word), 3); //copy 12 words, but dont copy the 3rd byte
    else
      copy(processOffset, itemOffset, 12, sizeof(word));
  }
  else
  {
    item1 = *(0x57914)[0x10];
    
    if (gp[0x2B8] == parent)
      itemOffset    = item1   + 0x348;
    else
      itemOffset    = item1   + 0x318;

    processOffset   = parent  +  0x30;

    if (((a2 | a3) & 3) != 0)
      copyInterleave(processOffset, itemOffset, 12, sizeof(word), 3); //copy 12 words, but dont copy the 3rd byte
    else
      copy(processOffset, itemOffset, 12, sizeof(word));
  }
  */
  
  return child;
}

bool terminateObject(object *obj)
{
  terminateObject(obj, true);
}

bool terminateObject(object *obj, bool termEvent) 
{                            
  if (!obj) { return GOOL_CODE_SUCCESS; }

  if (obj == crash /*&& *(0x5C53C).level != 0x19*/) { return GOOL_CODE_SUCCESS; }

  if (termEvent)
    issueEvent(0, obj, 0x1A00, 0, 0);

  if (hasChildren(obj))
  {
    object *firstChild = obj->process.children;
    
    object *curChild   = firstChild;
    object *nextChild;
    
    do
    {
      nextChild = curChild->process.sibling;
      terminateObject(curChild, termEvent);
   
      curChild = nextChild;
    } while (curChild);
  }

  //AUDIO NOT EMULATED JUST YET
  //sub_80030260(process);

  states[obj->process.PIDflags >> 8] &= 0xFFFFFFFE;  //clear bit 1 of the word in the 0x5FF58 array determined by the 0xD8 field of parent

  obj->type = 0;
  object *freeObjects = &initObject;
  
  if (obj == crash)
  {
    object *oldChildren = getChildren(freeObjects);
    adoptChild(obj, freeObjects);
    setChildren(freeObjects, oldChildren);

    player->process.parent   = 0;
    player->process.children = 0;
    player->process.sibling  = 0;
      
    crash = 0;
  }
  else
  {
    adoptChild(obj, freeObjects);
  }

  return GOOL_CODE_SUCCESS;
}

unsigned long terminateZoneObject(object *obj, entry *zone)
{
  if (obj->type == 0) { return GOOL_CODE_SUCCESS; }
  
  if (obj->zone == zone)
  {
    entry *prevZone = obj->zone;

    if (((obj->process.statusB       & 0x1000000) == 0) ||
        ((obj->process.routineFlagsA &   0x40000) == 0))
    {
      issueEvent(0, obj, 0x1A00, 0, 0);
      
      // if the object is still in its/a zone after issuing the term event
      // or the old zone header == non-existent == entire level is being terminated 
      if ((obj->zone == prevZone) || ((unsigned long)oldZoneHeader == -1))
        terminateObject(obj, false);
    }
  }
  
  return GOOL_CODE_SUCCESS;
}
      
unsigned long issueEvent(object *sender, object *recipient, unsigned long tag, unsigned long count, unsigned long *args)
{                                     
  if (!recipient)
  {
    if (sender) { sender->process.miscFlag = false; }   //cleared when no process
    return 0xFFFFFF01;
  }

  if (sender) { sender->process.miscFlag = true; }   //and set when is process   

  recipient->process.invoker = sender;
  
  unsigned long result = 0xFFFFFFE6;   
  unsigned short eventID;
  
  //if there is an event service routine for the current state
  if (recipient->process.subFuncB)  
  {                          
    push(recipient, tag);      
    push(recipient, args);
    
    //subtract 8 for the tag and args here since they are not part of the
    //previous frame
    unsigned short frameFP = getFP(recipient);
    unsigned short frameSP = getSP(recipient) - 8;

    recipient->process.fp = recipient->process.sp;
	
    push(recipient, 0xFFFF);
    push(recipient, recipient->process.pc);
    push(recipient, frameTag(frameFP, frameSP));
	  
    #ifdef DEBUG_GOOL
    if (getWord(recipient->global->itemData[0], 0, true) == DEBUG_GOOL_OBJTYPE)
    {
      char temp[0x200];
      sprintf(temp, "issuing event...");
      cout << temp << endl;
      
      printObject(recipient, 0);
    }
    #endif
          
    recipient->process.pc = recipient->process.subFuncB;
	
    event eventResult;
    unsigned long eventFlags = 8;
    result = interpret(recipient, eventFlags, eventResult); //var_30??
    
    if (result != 0xFFFFFFE6) 
    {
      if (isErrorCode(result)) { return result; }
     
      eventID = eventResult.id;
    }

    if (sender)
      sender->process.miscFlag = eventResult.issue;
  }
  
  //if the event service routine failed to find a routine/event ID or there
  //is no event service routine
  if (isErrorCode(result)) 
  {
    unsigned char *codeHeader = recipient->global->itemData[0];
    unsigned long subIDIndex = getWord(codeHeader, 0x10, true);
    unsigned long initStack  = getWord(codeHeader, 0xC, true);
	
    unsigned long eventIDIndex = tag >> 8;
	 
    if (eventIDIndex < subIDIndex) 
    {
      unsigned char *codeSubIDs = recipient->global->itemData[3]; 
      eventID = getHword(codeSubIDs, eventIDIndex*2, true);
    }
    else
      eventID = 0xFF;
    
    if (sender) 
    {
      if (eventID != 0xFF) { sender->process.miscFlag = true; }
      else                 { sender->process.miscFlag = false; }
    }

    //if the corresponding event maps to an absolute code address rather than a state
    if (eventID & 0x8000)
    {                      
      recipient->process.event = tag;
	  
      for (int c=0; c<count; c++)
        push(recipient, args[c]);

      //subtract word per arg from sp as the args are not part of the frame
      unsigned long frameFP = getFP(recipient);
      unsigned long frameSP = getSP(recipient) - (count*4);
	  
      recipient->process.fp = recipient->process.sp;
	  
      push(recipient, 0xFFFF);
      push(recipient, recipient->process.pc);
      push(recipient, frameTag(frameFP, frameSP));

      unsigned char *codeSource = recipient->global->itemData[1];
	  
      unsigned long offset  = (eventID & 0x7FFF) * 4;
      recipient->process.pc = (unsigned long*)&codeSource[offset];
	  
      #ifdef DEBUG_GOOL
      if (getWord(recipient->global->itemData[0], 0, true) == DEBUG_GOOL_OBJTYPE)
      {
        char temp[0x200];
        sprintf(temp, "issuing event...");
        cout << temp << endl;
        
        printObject(recipient, 0);
      }
      #endif
    
	    unsigned long flags = 0x3;
	    event sub;
      return interpret(recipient, flags, sub);
    }
  }
 
  if (eventID != 0xFF)
  {
    //if -EVENT = CRUSHED BY ?
    //            CRUSHED BY ROLLER
    //            CRUSHED BY BOULDER
      
    unsigned long statusCbits;	
    if (tag == 0x1800 || 
        tag == 0x1900 || 
        tag == 0x2500)
      statusCbits = recipient->process.statusC & 0xFFFFFFFD;  //grab the value with cleared bits 1 & 2 (so that the only other bit could be set is 13?)
    else
      statusCbits = recipient->process.statusC;
   
    unsigned char *codeSubs = recipient->global->itemData[4];
    unsigned char *event = codeSubs + (eventID * 0x10);
    
    unsigned long header = getWord(event, 0, true);
    
    unsigned long testBits;
    if (recipient == crash && 
        (recipient->process.displayMode >= 3 && 
         recipient->process.displayMode <= 5))
    {
      testBits = header & (statusCbits | 0x1002);
    }  
    else
      testBits = header & statusCbits;
    
    if (testBits)                      
    {
      if ((eventID != 0xFF) && sender)
        sender->process.miscFlag = 0; 
    }
    else
    {
      recipient->process.event = tag;           
      
      if (tag == 0x1800 || tag == 0x1900) //if crushed by ? or crushed by boulder
        recipient->process.statusA |= 0x10000;   //set bit 17 for these tags

      executeSub(recipient, eventID, count, args);  //execute the frame for the event handler s3
    }
  }
  
  return 0xFFFFF01;
}

void copyColors(object *dst, unsigned char *src, unsigned long offset)
{
  dst->colors.lightmatrix.V1.X = getHword(src, offset+0x0, true);
  dst->colors.lightmatrix.V1.Y = getHword(src, offset+0x2, true);
  dst->colors.lightmatrix.V1.Z = getHword(src, offset+0x4, true);
  dst->colors.lightmatrix.V2.X = getHword(src, offset+0x6, true);
  dst->colors.lightmatrix.V2.Y = getHword(src, offset+0x8, true);
  dst->colors.lightmatrix.V2.Z = getHword(src, offset+0xA, true);
  dst->colors.lightmatrix.V3.X = getHword(src, offset+0xC, true);
  dst->colors.lightmatrix.V3.Y = getHword(src, offset+0xE, true);
  dst->colors.lightmatrix.V3.Z = getHword(src, offset+0x10, true);
  dst->colors.color.R          = getHword(src, offset+0x12, true);
  dst->colors.color.G          = getHword(src, offset+0x14, true);
  dst->colors.color.B          = getHword(src, offset+0x16, true);
  dst->colors.colormatrix.V1.R = getHword(src, offset+0x18, true);
  dst->colors.colormatrix.V1.G = getHword(src, offset+0x1A, true);
  dst->colors.colormatrix.V1.B = getHword(src, offset+0x1C, true);
  dst->colors.colormatrix.V2.R = getHword(src, offset+0x1E, true);
  dst->colors.colormatrix.V2.G = getHword(src, offset+0x20, true);
  dst->colors.colormatrix.V2.B = getHword(src, offset+0x22, true);
  dst->colors.colormatrix.V3.R = getHword(src, offset+0x24, true);
  dst->colors.colormatrix.V3.G = getHword(src, offset+0x26, true);
  dst->colors.colormatrix.V3.B = getHword(src, offset+0x28, true);
  dst->colors.intensity.R      = getHword(src, offset+0x2A, true);
  dst->colors.intensity.G      = getHword(src, offset+0x2C, true);
  dst->colors.intensity.B      = getHword(src, offset+0x2E, true);
}

void copyVectors(object *src, object *dst)
{
  copyVector(&dst->process.vectors.trans, &src->process.vectors.trans);
  copyEuler(&dst->process.vectors.rot, &src->process.vectors.rot);
  copyVector(&dst->process.vectors.scale, &src->process.vectors.scale);
  copyVector(&dst->process.vectors.miscA, &src->process.vectors.miscA);
  copyEuler(&dst->process.vectors.miscB, &src->process.vectors.miscB);
  copyVector(&dst->process.vectors.miscC, &src->process.vectors.miscC);  
}

void copyEuler(cangle *src, cangle *dst)
{
  dst->Y = src->Y;
  dst->X = src->X;
  dst->Z = src->Z;
}

void copyVector(cvector *src, cvector *dst)
{
  dst->X = src->X;
  dst->Y = src->Y;
  dst->Z = src->Z;
}

void copyVector(csvector *src, csvector *dst)
{
  dst->X = src->X;
  dst->Y = src->Y;
  dst->Z = src->Z;
}

void addVector(cvector *src, cvector *dst)
{
  dst->X += src->X;
  dst->Y += src->Y;
  dst->Z += src->Z;
}

void addVector(csvector *src, csvector *dst)
{
  dst->X += src->X;
  dst->Y += src->Y;
  dst->Z += src->Z;
}

void subVector(cvector *src, cvector *dst)
{
  dst->X -= src->X;
  dst->Y -= src->Y;
  dst->Z -= src->Z;
}

void subVector(csvector *src, csvector *dst)
{
  dst->X -= src->X;
  dst->Y -= src->Y;
  dst->Z -= src->Z;
}

void addVector(cvector *srcA, cvector *srcB, cvector *dst)
{
  dst->X = srcA->X + srcB->X;
  dst->Y = srcA->Y + srcB->Y;
  dst->Z = srcA->Z + srcB->Z;
}

void addVector(csvector *srcA, csvector *srcB, csvector *dst)
{
  dst->X = srcA->X + srcB->X;
  dst->Y = srcA->Y + srcB->Y;
  dst->Z = srcA->Z + srcB->Z;
}

void subVector(cvector *srcA, cvector *srcB, cvector *dst)
{
  dst->X = srcA->X - srcB->X;
  dst->Y = srcA->Y - srcB->Y;
  dst->Z = srcA->Z - srcB->Z;
}

void subVector(csvector *srcA, csvector *srcB, csvector *dst)
{
  dst->X = srcA->X - srcB->X;
  dst->Y = srcA->Y - srcB->Y;
  dst->Z = srcA->Z - srcB->Z;
}

unsigned long initProcess(object *obj, unsigned long levelListIndex, unsigned long subIDIndex, unsigned long count, unsigned long *args)
{
  //process[0x158] = 0xFFFF;
  obj->process.memory[0] = 0xFFFF;
  
  obj->process.PIDflags       = 0;  //determines location in bitlist

  obj->process.entity       = 0;
  obj->process.pathProgress = 0;
  obj->process.pathCountEtc = 0;
  
  //set type to 1
  obj->type = 1;
  
  obj->process.vectors.miscA.X = 0;
  obj->process.vectors.miscA.Y = 0;
  obj->process.vectors.miscA.Z = 0;
  obj->process.vectors.miscB.X = 0;
  obj->process.vectors.miscB.Y = 0;
  obj->process.vectors.miscB.Z = 0;
  obj->process.vectors.miscC.X = 0;
  obj->process.vectors.miscC.Y = 0;
  obj->process.vectors.miscC.Z = 0;
  
  obj->process.speed = 0;
  
  obj->process.aniFrame = 0;
  
  obj->process.F8 = 0;
  
  obj->process.vectors.miscB.Y = 0; //not sure why they do this, probably a typo
 
  obj->process.statusA = 0;
  obj->process.statusB = 0;
  obj->process.statusC = 0;
  
  obj->process.displayMode = 0;
  obj->process._138 = 0;
  obj->process._130 = 0;
  obj->process._148 = 0;
  
  obj->process._14C = 0xFFFFFFFE;
  
  //inherit zone, rotation, and scale from parent object if type 1
  unsigned long parentType = obj->process.parent->type;
  if (parentType == 1)
  {
    obj->zone = obj->process.parent->zone;
    copyVectors(obj, obj->process.parent);
  }
  else
  {
    obj->zone = 0;
	
    obj->process.vectors.rot.X = 0;
    obj->process.vectors.rot.Y = 0;
    obj->process.vectors.rot.Z = 0;
    
    obj->process.vectors.scale.X = 0x1000;
    obj->process.vectors.scale.Y = 0x1000;
    obj->process.vectors.scale.Z = 0x1000;
  }

  if (levelListIndex == 4 | 5 | 0x1D )  { obj->zone = 0; }
  if (levelListIndex == 0)              { obj->process.camZoom = 0; }
  
  if (obj->subtype != 3) { return 0xFFFFFFF2; }  //if not type 3
  
  unsigned long EID = nsd->levelEIDs[levelListIndex];
  
  if (EID == 0x6396347F)      { return 0xFFFFFFF2; }  //if invalid code eid
  if (levelListIndex >= 0x40) { return 0xFFFFFFF2; }  //or refers to outside of the 64 level code eids

  entry *initCode = crashSystemPage(EID);
	  
  obj->global = initCode;
  
  //if (initEntry > 0xFFFFFF01)  { return 0xFFFFFFF2; }  //if an EID refers to invalid entry
  if (initCode == 0) { return 0xFFFFFFF2; }  //TODO: make a better lookup system so the errors can be handled here
  
  //first link set to link back to itself
  obj->process.self = obj;
  
  obj->process.link[0] = 0;
  obj->process.collider = 0;
  obj->process.invoker  = 0;
  
  //set initial state
  obj->process.initSubIndex = subIDIndex;
  
  //reset animation sequence
  obj->process.aniSeq = 0;
  
  //reset head block
  obj->process.F0 = 0;
  
  //this field always refers to crash
  obj->process.link[1] = player; //refers to crash
 
  entry *codeEntry = obj->global;
  unsigned char *codeHeader = codeEntry->itemData[0];
  
  //this is the offset at which states are separated from event handlers
  unsigned long subs = getWord(codeHeader, 0x10, true);  
  unsigned long initSubIndex = obj->process.initSubIndex;
  unsigned long subOffset = initSubIndex + subs;
  unsigned char *codeSubIDs = codeEntry->itemData[3];
  unsigned short subID = getHword(codeSubIDs, subOffset*2, true);
  
  unsigned long retval;
  if (subID != 0xFF)
  {
    retval = executeSub(obj, subID, count, args);

    if (retval < 0xFFFFFF02) { return 0xFFFFFF01; }
    else                     { return 0xFFFFFFF2; }
  }
  else
  {
    return 0xFFFFFFE5;
  }
}     

unsigned long executeSub(object *obj, unsigned long subid, unsigned long count, unsigned long *args)
{
  unsigned long F0val = obj->process.F0;

  if (subid == 0xFF)                     { return 0xFFFFFFF2; }                   //can't be 0xFF;
  if (obj->process.statusB & 0x10000000) { return 0xFFFFFFF2; }   //not sure where this is used, but returns the bit if its set
  
  obj->routine = subid;
  
  unsigned char *codeRoutines = obj->global->itemData[4];
  unsigned char *codeRoutine  = &codeRoutines[subid * 0x10];
  unsigned long initStatusC = getWord(codeRoutine, 4, true);
  obj->process.statusC = initStatusC;

  unsigned char *codeData  = obj->global->itemData[2];
  unsigned short EIDOffset = getHword(codeRoutine, 0x8, true) * 4;
  unsigned long EID        = getWord(codeData, EIDOffset, true);
  
  entry *externCode = crashSystemPage(EID);
  obj->external = externCode;                       
  
  unsigned char *codeSource = obj->global->itemData[1];
  
  //calculate new absolute pc location from relative offset
  //this is also the location of the current code block
  unsigned short newPCOffset = getHword(codeRoutine, 0xE, true);
  unsigned long *newPC;
  if (newPCOffset != 0x3FFF) { newPC = (unsigned long*)&codeSource[newPCOffset*4]; }
  else                       { newPC = 0; }
  obj->process.pc = newPC;
			 
  //calculate new absolute event block location from relative offset
  unsigned short newEBOffset = getHword(codeRoutine, 0xA, true);
  unsigned long *newEB;
  if (newEBOffset != 0x3FFF) { newEB = (unsigned long*)&codeSource[newEBOffset*4]; }
  else                       { newEB = 0; }
  obj->process.subFuncB = newEB;
  
  //calculate new absolute trans block location from relative offset
  unsigned short newTBOffset = getHword(codeRoutine, 0xC, true);
  unsigned long *newTB;
  if (newTBOffset != 0x3FFF) { newTB = (unsigned long*)&codeSource[newTBOffset*4]; }
  else                       { newTB = 0; }
  obj->process.subFuncA = newTB;
  
  //reset head block location
  obj->process.F0 = 0;
  
  //reset fp location                     
  obj->process.fp = 0;
  
  unsigned char *codeHeader = obj->global->itemData[0];
  unsigned long initStack = getWord(codeHeader, 0xC, true);
  
  //reset sp to initial location
  obj->process.sp = ((unsigned long*)&obj->process + initStack);

  //set bits 6 and 18 of this; bit 18 is set during state transitions
  obj->process.statusA |= 0x20020;
  obj->process.routineFlagsA = getWord(codeRoutine, 0, true);
  
  //push args  
  for (int c=0; c<count; c++)
    push(obj, args[c]);

  //create initial stack frame
  newFrame(obj, count, 0xFFFF); 
  
  //do head block interpretation
  unsigned long result;
  if (F0val != 0)
  {
    newFrame(obj, 0, 0xFFFF);

    obj->process.pc = (unsigned long*)F0val;     

    unsigned long flags = 0x13;
    event sub;
   
    result = interpret(obj, flags, sub);
    
    if (isErrorCode(result)) { return result; }
  }
  else
    push(obj, 0);

  //record new timestamp for this state change
  obj->process.initStamp = framesElapsed; 
      
  //if the object was unable to interpret its trans block in this frame
  //that can be made up for here
  if (obj->process.subFuncA != 0) 
  {  
    if (obj == var_60E00)      //if unable to interpret trans block in this frame
    {	
      newFrame(obj, 0, 0xFFFF); 
  
      unsigned long *subA = obj->process.subFuncA;    
	    unsigned long flags = 0x3;
      obj->process.pc = subA;
            
	    event sub;	  
      result = interpret(obj, flags, sub);      
   
      if (!isErrorCode(result)) { return result; }
    }
  }
  
  return 0xFFFFFF01;
}



unsigned long newFrame(object *obj, unsigned long offset, unsigned long header)
{

  unsigned long frameFP = getFP(obj);
  unsigned long frameSP = getSP(obj) - (offset*4);

  obj->process.fp = obj->process.sp;    //0xE4 will now be 0xDC -before- it is incremented 0xC     

  push(obj, header);                   //-header-, ---PC---, stak|ret
  push(obj, obj->process.pc);
  push(obj, frameTag(frameFP, frameSP));

  //printf("new frame created\n");
  //printObject(obj, 0);
            
  return 0xFFFFFF01;
}

bool testControls(unsigned long instruction, unsigned long port)
{
  //sub_8001FDC4(arg_a0 = EDDDD CCBBAAAA AAAAAAAA  //- 21 bits, arg_a1):
  unsigned long testButtons = (instruction & 0xFFF);
  unsigned long testMode    = (instruction >> 12) & 3;
  unsigned long dirTestMode = (instruction >> 14) & 3;
  unsigned long dirTest     = (instruction >> 16) & 0xF;   
  bool notPressedInd        = (instruction >> 20) & 1;
  
  bool cond;

  if (testMode == 0) 
    cond = true;
  else if (testMode == 1)  
  {
    unsigned long buttons = controls[port].tapState & 0xFFF; //grab 'has just been pressed' state (1 or 1 at instant of either occurence)
    cond = buttons & testButtons;                            //test if the specified buttons have just been pressed
    if (!cond) { return notPressedInd; }                     //if none of the buttons have just been pressed
  }
  else if (testMode == 2) 
  {
    unsigned long buttons = controls[port].heldState & 0xFFF; //grab 'IS currently held down (1) or not (0)' state
    cond = buttons & testButtons;                             //
    if (!cond) { return notPressedInd; }                      //if none of the buttons (referred to by abits) are currently held down
  } 
  else if (testMode == 3) 
  { 
    unsigned long buttons     = controls[port].tapState;
    unsigned long buttonsPrev = controls[port].prevTapState;  //index C
    
    cond = testButtons & (buttons | buttonsPrev);             //determine if the button has just been pressed in 
    if (!cond) { return notPressedInd; }                      //either the current or the previous read
  } 
  else if (testMode == 4)
  {
    unsigned long buttons     = controls[port].heldState;     //determine if the button is being held down in
    unsigned long buttonsPrev = controls[port].prevHeldState; //either the current or previous read

    cond = testButtons & (buttons | buttonsPrev);
    if (!cond) { return notPressedInd; }
  }    

  if (dirTestMode == 0)
  {
    if (notPressedInd) { return !cond; }   
    else               { return  cond; }
  }
  else if (dirTestMode == 1)
  {
    unsigned long buttons = controls[port].tapState;
    
    if (dirTest == 9)
      cond = buttons & 0x1000;
    else if (dirTest == 10)
      cond = buttons & 0x2000;
    else if (dirTest == 11)
      cond = buttons & 0x4000;
    else if (dirTest == 12)
      cond = buttons & 0x8000;
    else
    {
      unsigned long dirHeld   = (controls[port].heldState & 0xF000) >> 12;
      unsigned long direction = moveStates[dirHeld].mode;
     
      cond = false;
      if (dirTest == direction) { cond = true; }       //if the value corresponding to the pressed/held state is the same as dbits, return true (or false if ebit set)
      else
      {
        unsigned long prevDirHeld   = (controls[port].prevHeldState & 0xF000) >> 12;
        unsigned long prevDirection = moveStates[prevDirHeld].mode;

        cond = dirTest ^ prevDirection;                   
      }
    }
  }
  else if (dirTestMode == 2)
  {
    unsigned long buttons = controls[port].heldState;
    
    if (dirTest == 9)
      cond = buttons & 0x1000;
    else if (dirTest == 10)
      cond = buttons & 0x2000;
    else if (dirTest == 11)
      cond = buttons & 0x4000;
    else if (dirTest == 12)
      cond = buttons & 0x8000;
    else
    {
      unsigned long dirHeld   = (controls[port].heldState & 0xF000) >> 12;
      unsigned long direction = moveStates[dirHeld].mode;
    
      cond = ((dirTest ^ direction) == 0);                   
    }
  }
  else if (dirTestMode == 3)
  {
    unsigned long dirHeld   = (controls[port].heldState & 0xF000) >> 12;
    unsigned long direction = moveStates[dirHeld].mode;
   
    cond = false;
    if (dirTest == direction) { cond = true; }
    else
    {
      unsigned long prevDirHeld   = (controls[port].prevHeldState & 0xF000) >> 12;
      unsigned long prevDirection = moveStates[prevDirHeld].mode;

      if (dirTest == prevDirection) { cond = true; }
    }
  }
 
  if (notPressedInd) { return !cond; }
  else               { return cond;  }
}

const int bitsPop = 0xE1F;

unsigned long interpret(object *obj, unsigned long &arg1, event &arg2)
{
  unsigned long argBuf;  //var_58
  unsigned long var_5C;  //var_5C
  object *recipient;     //var_60
  //..
  unsigned long args[8]; //var_B0
  
  
  long instruction;
  unsigned char opcode;
  do
  {
    instruction = *(obj->process.pc++);
    opcode = instruction >> 24;
	
    //srcval = (instr >> 12) & 0xFFF = left
    //dstval = (instr >>  0) & 0xFFF = right
    
    unsigned long left, right;
    signed long sleft, sright;
    unsigned long *src, *dst;
    bool endianS, endianD;
    
    
	  #ifdef DEBUG_GOOL
    if (getWord(obj->global->itemData[0], 0, true) == DEBUG_GOOL_OBJTYPE)
    {
      goolDisasm gd;
      unsigned long offset = 3;
      unsigned long length = 7;
      char lines[length][80];
      
      unsigned long codeStart = (unsigned long)obj->global->itemData[1];
      unsigned char *codeCur = (unsigned char *)(obj->process.pc - offset);
      
      if ((unsigned long)codeCur < codeStart)
        codeCur = obj->global->itemData[1];
        
      gd.setSource(codeCur, length*(sizeof(long))); 
      gd.disassemble(lines);
      
      char temp[0x200];
      sprintf(temp, "current code window: ");
      cout << temp << endl;
      
      unsigned long codeLocation = (unsigned long)codeCur - (unsigned long)obj->global->itemData[1];
      for (int lp=0; lp<length; lp++)
      {
        if (lp == (offset - 1))
        {
          //printf("***");
          cout << "***";
        }
       
        sprintf(temp, "   %x:  %s", codeLocation+(lp*4), lines[lp]);
        cout << temp;
      }
    }
    #endif
    
    switch (opcode)
    {
      case 0:
      
      left = translate(obj, srcVal(instruction));
      right = translate(obj, dstVal(instruction));
      
      push(obj, right+left);
      break;
      
      case 1:
      
      left = translate(obj, srcVal(instruction));
      right = translate(obj, dstVal(instruction));
      
      push(obj, right-left);
      break;
      
      case 2:
      
      left = translate(obj, srcVal(instruction));
      right = translate(obj, dstVal(instruction));
      
      push(obj, right*left);
      break;
      
      case 3:
      
      left = translate(obj, srcVal(instruction));
      right = translate(obj, dstVal(instruction));
      
      push(obj, right/left);
      break;
      
      case 4:
      
      left = translate(obj, srcVal(instruction));
      right = translate(obj, dstVal(instruction));
      
      push(obj, ((left^right)==0));
      break;
      
      case 5:
      
      left = translate(obj, srcVal(instruction));
      right = translate(obj, dstVal(instruction));
      
      if (right == 0) { push(obj, 0); }
      else            { push(obj, (left > 0)); }
      break;
      
      case 6:
      
      left = translate(obj, srcVal(instruction));
      right = translate(obj, dstVal(instruction));
      
      push(obj, (left !=0 || right!=0));
      break;
      
      case 7:
      
      left = translate(obj, srcVal(instruction));
      right = translate(obj, dstVal(instruction));
      
      push(obj, left&right);
      break;
      
      case 8:
      
      left = translate(obj, srcVal(instruction));
      right = translate(obj, dstVal(instruction));
      
      push(obj, left|right);
      break;
      
      case 9: 
      
      sleft = translate(obj, srcVal(instruction));
      sright = translate(obj, dstVal(instruction));
      
      push(obj, sleft>sright);
      break;
      
      case 0xA:
      
      sleft = translate(obj, srcVal(instruction));
      sright = translate(obj, dstVal(instruction));
      
      push(obj, sleft>=sright);
      break;
      
      case 0xB:
      
      sleft = translate(obj, srcVal(instruction));
      sright = translate(obj, dstVal(instruction));
      
      push(obj, sleft<sright);
      break;
      
      case 0xC:
      
      sleft = translate(obj, srcVal(instruction));
      sright = translate(obj, dstVal(instruction));
      
      push(obj, sleft<=sright);
      break;
      
      case 0xD:
      
      left = translate(obj, srcVal(instruction));   //>>12
      right = translate(obj, dstVal(instruction));  //n/a
      
      push(obj, right % left);
      break;
      
      case 0xE:
      
      left = translate(obj, srcVal(instruction));
      right = translate(obj, dstVal(instruction));
      
      push(obj, left ^ right);
      break;
      
      case 0xF:
      
      left = translate(obj, srcVal(instruction));
      right = translate(obj, dstVal(instruction));
      
      push(obj, (((left & right) ^ left)==0));
      break;
      
      case 0x10:
      
      left = translate(obj, srcVal(instruction));
      right = translate(obj, dstVal(instruction));
      
      if (left != right) { push(obj, right + (rand() % (left-right))); }
      else               { push(obj, right); }
      break;
      
      case 0x11:
      
      left = translate(obj, srcVal(instruction));
      dst = translateP(obj, dstVal(instruction), endianD);
      
      if (dst) { *(dst) = left; }
      
      break;
      
      case 0x12:
      
      left = translate(obj, srcVal(instruction));
      dst = translateP(obj, dstVal(instruction), endianD);
      *(dst) = (left == 0);
      
      break;
      
      case 0x13:
      
      signed long p1, p2, rval;
      if (srcVal(instruction) == 0xBF0)
      {
        p1 = pop(obj);
        p2 = pop(obj);
      }
      else
      {
        p1 = 0x100;
        p2 = translate(obj, srcVal(instruction));
      }

      dst = translate(obj, dstVal(instruction), endianD);
     
      signed long progress;
      unsigned long absprog;
      if (dst)
      {
        rval = *(dst);       //counter proc[0x114] say is at 0x1000..
        
        if (p2 >= 0)                  //max value -is- 0x1022
        {
          if ((rval + p1) < p2)           //if 0x1022 < 0x1022, is not
            progress = rval + p1;
          else
            progress = (p1*2) - rval;  //so 0x44 - 0x1000

          absprog = abs(progress);
        }
        else
        {
          if (p2 >= (rval - p1))
            progress = -p2 - (p1*2);
          else
            progress = rval - p1;
            
          absprog = -abs(progress);
        }

        // need to re-translate as a stack push if dst is the stack; 
        // this is the only(?) case of an 'inout' type operand 
        dst = translateP(obj, dstVal(instruction), endianD);
        
        *(dst) = (unsigned long)progress;
        push(obj, absprog);
      }
      break;
      
      case 0x14:
      
      src = translate(obj, srcVal(instruction), endianS);
      dst = translateP(obj, dstVal(instruction), endianD);
      *(dst) = (unsigned long)src;
      
      break;
      
      case 0x15:
      
      sleft = (signed long)translate(obj, srcVal(instruction));
      right = translate(obj, dstVal(instruction));
      
      if (sleft < 0) { push(obj, right >> -sleft); }
      else           { push(obj, right <<  sleft); }
      
      break;
      
      case 0x16:
      case 0x26:
      
      src = translate(obj, srcVal(instruction), endianS);
      dst = translate(obj, dstVal(instruction), endianD);
      
      //push -pointers-
      if (opcode == 0x26)
      {
        if (dst)
        {
          push(obj, (unsigned long)dst);
          
          if (src)
            push(obj, (unsigned long)src);
        }
      }
      //push -values-
      else
      {
        if (dst)
        {
          if (src)
            argBuf = *(src);
          
          push(obj, *(dst));
          
          if (src)
            push(obj, argBuf);
        }
      }
      
      break;
      
      case 0x17:
      {
        src = translate(obj, srcVal(instruction), endianS);
        dst = translateP(obj, dstVal(instruction), endianD);
        
        if (dst)
          *(dst) = ~*(src);
      }
      break;      
        
      //src = 
      case 0x18:
      {      
        unsigned char *codeSource = obj->global->itemData[1];
        unsigned long offset = (instruction & 0x3FFF) * 4;
        
        long mem = (instruction >> 14) & 0x3F; 
        if (mem == 0x1F)
          push(obj, &codeSource[offset]);
        else
          ((unsigned long*)&obj->process)[mem] = (unsigned long)&codeSource[offset];
      }
      break;
      
      case 0x19:
      
      sleft = (signed long)translate(obj, srcVal(instruction));
      dst  = translateP(obj, dstVal(instruction), endianD);
      
      if (sleft < 0) { *(dst) = -sleft; }
      else           { *(dst) =  sleft; }
      
      break;
      
      case 0x1A:
      {
        bool result = testControls(instruction & 0x1FFFFF, 0);
        push(obj, result);
      }
      break;
      
      case 0x1B:
      {
        left = translate(obj, srcVal(instruction));
        right = translate(obj, dstVal(instruction));
        
        unsigned long scaled, total;
        if (globalVelocity >= 0x67)
          scaled = left * 0x66;
        else
          scaled = left * globalVelocity;

        if (scaled < 0)
          scaled += 0x3FF; 

        total = (scaled >> 10) + right; 
        
        push(obj, total);
      }
      break;
      
      case 0x1C:
      {
        unsigned long *addr = translate(obj, dstVal(instruction), endianD);
        
        unsigned long subFunction    = (instruction >> 20) & 0xF;
        signed long subSubFunction   = (instruction << 12) >> 27;
        unsigned long linkIndex      = (instruction >> 12) & 0x7;
        
        switch (subFunction)
        {
          //get argument passed to event service routine, passed as a
          //stray pointer rather than the actual arguments themselves
          case 0:
          {
            if (addr)
            {
              unsigned long *args = (unsigned long*)*(addr);
              
              unsigned long argIndex = subSubFunction;
              
              if (*args)
                push(obj, args[argIndex]);
              else
                push(obj, 0);
            }
          }
          break;
          
          case 1:
          case 6:
          {
            object *link = (object*)*((unsigned long*)&obj->process + linkIndex);

            cvector *trans;
            cvector *linkTrans;
            
            if (subFunction == 6) { trans = (cvector*)*(addr); }
            else                  { trans = &obj->process.vectors.trans; }
            
            linkTrans = &link->process.vectors.trans;
            
            cvector _trans;
            cvector _linkTrans;
            if (subSubFunction & 2)
            {
              _trans.X = trans->X;
              _trans.Y =        0;
              _trans.Z = trans->Z;
              
              trans = &_trans;
              
              _linkTrans.X = linkTrans->X;
              _linkTrans.Y =            0;
              _linkTrans.Z = linkTrans->Z;
              
              linkTrans = &_linkTrans;
            }
            
            if (subSubFunction & 1)
            {
              push(obj, eucDist(trans, linkTrans));
            }
            else
            {
              push(obj, apxDist(trans, linkTrans));
            }
          }
          break;
          
          case 2:
          {
            object *link = (object*)*((unsigned long*)&obj->process + linkIndex);
          
            cvector *inTrans   = (cvector*)addr;
            cvector *linkTrans = &link->process.vectors.trans;
            
            if (obj->process.statusB & 0x00200200)
            {
              push(obj, angleXY(inTrans, linkTrans));
            }
            else
            {
              push(obj, angleXZ(inTrans, linkTrans));
            }
          }
          break;
          
          //read from a links' memory
          case 3:
          {
            right = *(addr);
            
            object *link = (object*)*((unsigned long*)&obj->process + linkIndex);

            unsigned long mem  = (right >> 8);
            unsigned long data = *((unsigned long*)&link->process + mem);
            
            push(obj, data);
          }
          break;
          
          //write to a links' memory
          case 4:
          {
            right = *(addr);
            
            object *link = (object*)*((unsigned long*)&obj->process + linkIndex);
            
            unsigned long mem  = (right >> 8);
            unsigned long data = pop(obj);
            
            *((unsigned long*)&link->process + mem) = data;
          }
          break;
            
          //calculates angle in XZ plane from process translation vector to one of its link's translation vectors
          //then finds angular distance (an angle)between this and process[0x90] angle
          case 5:
          {
            object *link = (object*)*((unsigned long*)&obj->process + linkIndex);
            
            cvector *trans     =  &obj->process.vectors.trans;
            cvector *linkTrans = &link->process.vectors.trans;
            
            unsigned long result = angleXZ(trans, linkTrans);
            
            signed long diff = result - (obj->process.vectors.rot.X & 0xFFF);
            if (diff <      0) { diff = -diff; }
            if (diff >= 0x800) { diff = diff & 0xFFF; }
            
            push(obj, diff);
          }
          break;
          
          case 7:
          {
            if (addr && *(addr))
            {
              unsigned long PIDflags = *(addr);
              unsigned long PID = PIDflags >> 8;
              
              object *found;

              if (states[PID] & 1)
              {
                for (int count = 3; count < 5; count++)
                {
                  object *headObject = &headObjects[count];
                  
                  object *children = getChildren(headObject);
                  
                  if (headObject->type != 2 &&
                      !(found = hasPID(headObject, PIDflags)))
                  {
                    object *child = children;
                    found = 0;
                    
                    while (child && !found)
                    {
                      found = findObject(child, hasPID, PIDflags);
                      child = child->process.sibling;
                    }
                  }
                }
                
                push(obj, found);
              }
              else
              {
                push(obj, 0);
              }
            }
          }
          break;
          
          case 8:
          {
            unsigned long PIDflags = *(addr);
            
            if (PIDflags)
            {
              unsigned long PID = PIDflags >> 8;
              if (linkIndex) //is this right? used as a flag in this context?
              {
                states[PID] &= 0xFFFFFFFD;
              }
              else
              {
                states[PID] |= 2;
              }
            }
          }
          break;
          
          case 9:
          {
            cvector *vector = (cvector*)*(addr);
            
            entry *newZone;
            if (vector)
              newZone = findZone(currentZone, vector);
            else
              newZone = currentZone;
            
            if ((unsigned long)newZone < 0xFFFFFF02)
            {
              object *link = (object*)*((unsigned long*)&obj->process + linkIndex);
              
              link->zone = newZone;
            }
          }
          break;
          
          case 10:
          {
            unsigned long PIDflags = *(addr);
            
            if (PIDflags)
            {
              unsigned long PID = PIDflags >> 8; 
              switch (subSubFunction)
              {
                case 0: states[PID] &= 0xFFFFFFFB; break; //clear bit 3
                case 1: states[PID] |=          4; break; //set bit 3
                case 2: states[PID] &= 0xFFFFFFF7; break; //clear bit 4
                case 3: states[PID] |=          8; break; //set bit 4
                case 4: /*TODO*/ break;
                case 5: /*TODO*/ break;
                case 6: break;
                case 7: break;
                case 8: states[PID] &= 0xFFFFFFFE; break; //clear bit 1
                case 9: states[PID] |=          1; break; //set bit 1
              }
            }
          }
          break;
              
          //this is a multipurpose function that:
          //for one, allows boxes to set their display flag to true based
          //on checking some object's (not necessarily this) global pid 
          //status bits returned by subsub 2 (status bit 3)
          //subsub 1: returns false on specified global pid status bit 2 set
          //subsub 2: returns true  on specified global pid status bit 3 set
          //subsub 3: returns true  on specified global pid status bit 4 set
          case 11:
          {
          
            right = *(addr);
            
            //the priorities are different in the assembly, prob based
            //on likeliness of usage
            if (subSubFunction == 1)
            {
              if (right == 0)
                push(obj, 0);
              else
              {
                unsigned long PID = right >> 8;
                bool test = (((states[PID] >> 1) ^ 1) & 1);
                push(obj, test);
              }
            } 
            else if (subSubFunction == 2) 
            {
              if (right == 0)
                push(obj, 0);
              else
              {
                unsigned long PID = right >> 8;
                bool test = states[PID] & 4;
                push(obj, test);
              }
            }
            else if (subSubFunction == 3)
            {
              if (right == 0)
                push(obj, 0);
              else
              {
                unsigned long PID = right >> 8;
                bool test = states[PID] & 8;
                push(obj, test);
              }
            }
          }
          break;
          
          case 12:
          {
            switch(subSubFunction)
            {
              case 0:
              //saveState(obj, levelState, false);
              break;
              case 1:
              //reloadLevel(levelState);
              break;
              case 2:
              {
                unsigned long listIndex = *(addr) >> 8;
                object *parent = &headObjects[listIndex];
                
                adoptChild(obj, parent);
              }
              break;
              case 3:
              //unused
              break;
              case 4:
              //changes zone?
              break;
              case 5:
              //gp[0x268] = 0xFFFFFD56
              break;
              case 6:
              //spu routine
              break;
              case 7:
              {
                if (currentZone)
                {
                  unsigned char *curZoneHeader = currentZone->itemData[0];
                  unsigned long neighborCount = getWord(curZoneHeader, 0x210, true);
                  
                  if (neighborCount > 0)
                  {
                    for (int index = 0; index < neighborCount; index++)
                    {
                      unsigned long neighborEID = getWord(curZoneHeader, 0x214+(4*index), true);
                      entry *neighborZone = crashSystemPage(neighborEID);
                      
                      for (int group = 0; group < 8; group++)
                      {
                        unsigned long (*routine)(object*, void*) = (unsigned long (*)(object*, void*))terminateZoneObject;
                        familyRoutineCBPH(&headObjects[group], routine, neighborZone);
                      }
                    }
                  }
                }
                break;
              }
              case 8:
              {
                object *link = (object*)*((unsigned long*)&obj->process + linkIndex);

                cvector *inTrans   = (cvector*)addr;
                cvector *linkTrans = &link->process.vectors.trans;
                
                signed long distY  = linkTrans->Y - inTrans->Y;
                signed long distXZ = eucDistXZ(linkTrans, inTrans);

                //we dont have to scale down for atan2
                signed long arctan = atan2(distY, distXZ);
               
                //we do have to convert back to crash angles if we are using math's atan2
                signed long ang = CTCA(arctan) & 0xFFF;
                
                push(obj, ang);
              }
              break;
              //TODO: still need cases 9, 10, & 11
            }
          }
          break;
          //TODO: case 13
          
          case 14:
          {
            object *link = (object*)*((unsigned long*)&obj->process + linkIndex);

            cspace linkSpace;
            getSpace(&link->process.vectors.trans, &link->bound, &linkSpace);

            cpoint *testPoint = (cpoint*)addr;
            bool result = isColliding(testPoint, &linkSpace);

            push(obj, result);            
          }
          break;
          
          case 15:
          {
            //TODO: memory card/inventory save stuff
          }
          break;
        }     
      }
      break;        
      
      case 0x1D:
      {
        left = translate(obj, srcVal(instruction));
        sright = (signed long)translate(obj, dstVal(instruction));
        
        double angle = (double)(sright << 11)/left;
        double phaseangle = angle - 0x400;
        double radians = (phaseangle * (PI/0x800));
        //if (0xFFFF < right)
        double sine = sin(radians);
        signed long fixed = FLOAT2FIXED(sine);
        unsigned long fixedPos = fixed + 0x1000;
        
        unsigned long scale = (fixedPos * left) >> 13;
        
        //double angle = (((double)right/sleft) * PI) - (PI/2);
        
        push(obj, scale);
        
        //increasing sine wave: period = left/2
        //                      amplitude increases at slope of 1
        //                      right = distance into sine
      }
      break;
      
      case 0x1E:
      {
        left = translate(obj, srcVal(instruction));
        right = translate(obj, dstVal(instruction));
        
        unsigned long result = (left + drawCount) % right;
        
        push(obj, result);
      }
      break;
      
      case 0x1F:
      {
        right = translate(obj, dstVal(instruction));
        unsigned long index = right >> 8;
        
        push(obj, globals[index]);
      }
      break;
      
      case 0x20:
      {
        left = translate(obj, srcVal(instruction));
        right = translate(obj, dstVal(instruction));
        unsigned long index = right >> 8;
        
        globals[index] = left;
      }
      break;
      
      case 0x21:
      {
        left = translate(obj, srcVal(instruction));
        right = translate(obj, dstVal(instruction));
        
        long ang = leastAngDist(right, left);
        
        push(obj, ang);
      }
      break;
      
      case 0x23:
      {
        unsigned long linkIndex = (instruction >> 12) & 7;
        unsigned long colorIndex = (instruction >> 15) & 0x3F;
        
        object *link = (object*)*((unsigned long*)&obj->process + linkIndex);
        unsigned short color = *((unsigned short*)&obj->colors + colorIndex);
        
        push(obj, color);
      }
      break;
      
      case 0x24:
      {
        right = translate(obj, dstVal(instruction));
        
        unsigned long linkIndex = (instruction >> 12) & 7;
        unsigned long colorIndex = (instruction >> 15) & 0x3F;

        object *link = (object*)*((unsigned long*)&obj->process + linkIndex);
        unsigned short color = right; 
        
        *((unsigned short*)&obj->colors + colorIndex) = color;
      }
      break;
        
      case 0x22:
      case 0x25:
      {
        signed long speed;
        if (srcVal(instruction) == 0xBF0)
        {
          speed = pop(obj);
          left = pop(obj);
        }
        else
        {
          speed = pop(obj);
          left = translate(obj, srcVal(instruction));
        }
        
        if (right = translate(obj, dstVal(instruction)))
        {
          signed long val;
          if (opcode == 0x22)
            val = approach(right, left, speed);
          else if (opcode == 0x25)
            val = objectRotate(right, left, speed, 0);
            
          push(obj, val);
        }
        else
          push(obj, 0);
      }
      
      case 0x27:
      {
        left = translate(obj, srcVal(instruction));
        dst  = translateP(obj, dstVal(instruction), endianD);
        
        unsigned long offset = left/64;
        unsigned char *item5 = obj->global->itemData[5];
        *(dst) = (unsigned long)&(item5[offset]);
      }
      break;
      
      
      case 0x82:
      {
      unsigned char condType = (instruction >> 20) & 3;
      unsigned long cond;
    
      if (condType == 0) //no condition = jump
        cond = 1;
      else if (condType == 1)
      {
        long mem = (instruction >> 14) & 0x3F;
      
        unsigned long val;
        if (mem == 0x1F)
          val = pop(obj);
        else
          val = *((unsigned long*)&obj->process + mem);
      
        cond = val;
      }
      else if (condType == 2)
      {
        long mem = (instruction >> 14) & 0x3F;
      
        unsigned long val;
        if (mem == 0x1F)
          val = pop(obj);
        else
          val = *((unsigned long*)&obj->process + mem);
      
        cond = (val == 0);
      }
      else
        cond = 0;
      
      unsigned char branchType = (instruction >> 22) & 3;
      if (cond != 0 && branchType != 3)
      {
        if (branchType == 0) //relative jump
        {
          signed short relAddr = (signed short)((instruction & 0x3FF) << 6) >> 6;
          obj->process.pc += relAddr;
        
          unsigned char stackSize = (instruction >> 10) & 0xF;
          obj->process.sp -= stackSize;
        }
        else if (branchType == 1) 
        {
          unsigned char *subs = obj->global->itemData[4];
       
          unsigned short varCodeSubIndex = (instruction & 0x3FFF);
          unsigned char* varCodeSub      = subs + (varCodeSubIndex * 16);
          unsigned long varCodeSubHeader = getWord(varCodeSub, 0, true);

          unsigned long displayMode = obj->process.displayMode;
        
          unsigned long statusC = obj->process.statusC;
          unsigned long flagsExecExtern;
		  
          if (displayMode >= 2 && displayMode <= 4)
            flagsExecExtern = (statusC | 0x1002) & varCodeSubHeader;
          else
            flagsExecExtern = (statusC) & varCodeSubHeader;

            
          if (flagsExecExtern == 0)
          {
            #ifdef DEBUG_GOOL
            if (getWord(obj->global->itemData[0], 0, true) == DEBUG_GOOL_OBJTYPE)
            {
              //printf("issuing SUBROUTINE branch instruction \n");
              cout << "Changing state to " << varCodeSubIndex << "..." << endl;
            }
            #endif
            
            unsigned long retval = executeSub(obj, varCodeSubIndex, 0, 0);
              
            if (retval >= 0xFFFFFF02 || ((arg1 & 2) == 0))
              pop(obj);
            else
              return retval;                    //the process is preempted
          }
        }
        if (branchType == 2)
        {
          #ifdef DEBUG_GOOL
          if (getWord(obj->global->itemData[0], 0, true) == DEBUG_GOOL_OBJTYPE)
          {
            //printf("ISSUING RTE INSTRUCTION:\n");          
            //printf("before switch: \n");
            cout << "ISSUING RTE INSTRUTION:" << endl;
            cout << "before switch: " << endl;
            
            printObject(obj, arg1);
          }
          #endif
          
          //var_D8 = var_70;
          unsigned long execState = arg1;
          
          //process[0xDC] = process[0xE4] + 8;  //set back to return frame
          obj->process.sp = obj->process.fp + 2;
          
          //lval = *(process[0xDC]) >> 16;      //grab tag there
          //rval = *(process[0xDC]) & 0xFFFF;
          unsigned long retFrameTag = peek(obj);
          unsigned short retFrameSP = retFrameTag >> 16;
          unsigned short retFrameFP = retFrameTag & 0xFFFF;
          //pop(obj);
          
          unsigned long retStatus;
          // not SP?
          if (retFrameFP != 0)                      //if not the initial frame
          {
            retStatus = 0xFFFFFF01;
            
            obj->process.sp = obj->process.fp + 1;
            
            unsigned long srcret_c = (unsigned long)(obj->process.pc);
            unsigned long dstret_c = (unsigned long)(peek(obj));
            unsigned long srcret = srcret_c - (unsigned long)obj->global->itemData[1];
            unsigned long dstret = dstret_c - (unsigned long)obj->global->itemData[1];
            
            obj->process.pc = (unsigned long*)peek(obj);            //return to the code for this frame
            pop(obj);
            
            //obj->process.sp = obj->process.fp + 0;
            unsigned short prevState = peek(obj);
            
            //var_D8 = (var_70 & 0xFFFF0000) | *(process[0xDC]);   //lhu   hmm...
            execState = (arg1 & 0xFFFF0000) | (prevState & 0xFFFF);
          
            
            //v0 = &process[0x60 + rval];  //but then r of tag determines index into links
            //process[0xDC] = v0;       //which we put at stack (but not PUSH)
            obj->process.sp = (unsigned long*)((unsigned char *)&obj->process + (retFrameSP));
            obj->process.fp = (unsigned long*)((unsigned char *)&obj->process + (retFrameFP));
            
            #ifdef DEBUG_GOOL
            if (getWord(obj->global->itemData[0], 0, true) == DEBUG_GOOL_OBJTYPE)
            {
              //printf("after switch: \n");
              cout << "after switch: " << endl;
              printObject(obj, execState);
              
              char temp[0x200];
              sprintf(temp, "code location: %x", obj->global->itemData[1]);
              cout << temp << endl;
              
              sprintf(temp, "rte code location: %x dest code location: %x\n", srcret_c, dstret_c);
              cout << temp << endl;
              
              sprintf(temp, "rte location: %x dest location: %x\n", srcret, dstret);
              cout << temp << endl;
            }     
            #endif
          }
          else
          {
            retStatus = 0xFFFFFFE7;
          }

          if ((arg1 & 8) == 0)    //bit 4 not set        //22e48
          {  
            if (((arg1 & 1) == 0) && (retStatus < 0xFFFFFF02))  //bit 1 not set and not initial frame
            {
                //22e94 w/ v0 = 0xFFFF
              if ((execState & 0xFFFF) != 0xFFFF)
                arg1 = execState;  
            }
            else
            {
              //bit 4 not set, bit 1 set, not initial frame, and bit 5 not set
              if (((arg1 & 0x10) != 0) && (retStatus < 0xFFFFFF02)) 
                push(obj, 0);                //bit 4&5 not set & not initial frame, push 0
            
              return retStatus;             //the process execution finally ends
            }
          }   
          else if ((arg1 & 0x20) == 0)    //bit 4 set and bit 6 not set
            return 0xFFFFFFE6;             //the process execution finally ends with ERROR
          else                             
          {                                 //bit 4 set and bit 6 set
            arg2.id = 0xFF;                 //return null event  
            return 0xFFFFFF01;             //the process execution finally ends with SUCCESS
          }
        }  
      }
      }
      break;
 
      case 0x83:
      {
        unsigned long aniFrame = instruction & 0x7F;
        unsigned long aniSeq   = (instruction >> 7) & 0x1FF;
        unsigned long wait     = (instruction >> 16) & 0x3F;
        unsigned long flip     = (instruction >> 22) & 0x3;
        
        entry *codeEntry        = obj->global;
        unsigned char *animItem = codeEntry->itemData[5];
        
        unsigned char *animation = &animItem[aniSeq * 4];

        obj->process.aniFrame = aniFrame << 8;
        obj->process.aniSeq   = animation;

        //push timestamp and number of frames to wait before resume execution
        push(obj, (wait << 24) | framesElapsed);
        
        if (flip == 0)
          obj->process.vectors.scale.X = -abs(obj->process.vectors.scale.X);
        else if (flip == 1)
          obj->process.vectors.scale.X = abs(obj->process.vectors.scale.X);
        else if (flip == 2)
          obj->process.vectors.scale.X = -obj->process.vectors.scale.X;
        else if (flip == 3) {}
         
        if (obj->process.statusB & 0x18)
        {
          cvector temp = { 0x1000, 0x1000, 0x1000 };
          bool outOfRange = isOutOfRange(
                                         obj, 
                                         &obj->process.vectors.trans, 
                                         &temp/*&crash->process.vectors.trans*/,
                                         0x7D000, 0xAF000, 0x7D000
                                        );
          
          if (!outOfRange || (obj->process.statusB & 0x80000000))
            objectBound(obj, &obj->process.vectors.scale);
        }
        
        if (arg1 & 4) { return GOOL_CODE_SUCCESS; }
      }
      break;
      
      case 0x84:
      {
        unsigned long aniFrame = translate(obj, dstVal(instruction));
        unsigned long wait     = (instruction >> 16) & 0x3F;
        unsigned long flip     = (instruction >> 22) & 0x3;
        
        obj->process.aniFrame = aniFrame;
        
        push(obj, (wait << 24) | framesElapsed);
        
        if (flip == 0)
          obj->process.vectors.scale.X = -abs(obj->process.vectors.scale.X);
        else if (flip == 1)
          obj->process.vectors.scale.X = abs(obj->process.vectors.scale.X);
        else if (flip == 2)
          obj->process.vectors.scale.X = -obj->process.vectors.scale.X;
        else if (flip == 3) {}
        
        objectBound(obj, &obj->process.vectors.scale);
        
        if (arg1 & 4) { return GOOL_CODE_SUCCESS; }
      }
      break;  
        
      case 0x85:
      {
      unsigned char subFunction = (instruction >> 18) & 7;
      
      switch(subFunction)
      {
        case 0:
        {
          dst = translate(obj, dstVal(instruction), endianD);
          if (dst && obj->process.entity)
          {
            unsigned long progressV = *(dst);
            
            unsigned char inIndex = (instruction >> 12) & 7;
            cvector *trans = &((cvector*)&obj->process.vectors)[inIndex];
            
            cvector new_trans;
            new_trans.X = trans->X;
            new_trans.Y = trans->Y;
            new_trans.Z = trans->Z;
            objectPath(obj, progressV, &new_trans);
            trans->X = new_trans.X;
            trans->Y = new_trans.Y;
            trans->Z = new_trans.Z;
          }
        }
        break;        
        
        case 1:
        {
          //should be dst
          src = translate(obj, dstVal(instruction), endianS);
          
          unsigned char indexIn  = (instruction >> 12) & 7;
          unsigned char indexOut = (instruction >> 15) & 7;
          cvector *in  = &((cvector*)&obj->process.vectors)[indexIn];
          cvector *out = &((cvector*)&obj->process.vectors)[indexOut];
          
          //screenCoordinates((vector*)in, (vector*)out);
          
          if (src)
          {
            unsigned long zval = out->Z >> 8;
            
            if (zval != 0)
            {
              cvector *srcVector = (cvector*)src;
              
              srcVector->X = (srcVector->X * 280) / zval;
              srcVector->Y = (srcVector->Y * 280) / zval;
              srcVector->Z = (srcVector->Z * 280) / zval;
            }
          }
        }
        break;
        
        case 2:
        {
          unsigned long dstIndex = (instruction >> 12) & 7;
          cvector *dstVector = &((cvector*)&obj->process.vectors)[dstIndex];
          
          unsigned long scale = translate(obj, dstVal(instruction));
          
          //we need to do the conversions first since we are using math.h sin
          unsigned long appXangC = (obj->process.vectors.miscB.X & 0xFFF);
          unsigned long appXang  = CTGA(appXangC);
          unsigned long appX     = CTCA(sin(appXang));
         
          dstVector->X = ((appX/16) * scale) >> 8;
          
          if (obj->process.statusB & 0x00200200)
          {
            appXangC           = (obj->process.vectors.miscB.X & 0xFFF);
            appXang            = CTGA(appXangC);
            unsigned long appZ = CTCA(cos(appXang));
            
            dstVector->Z = ((appZ/16) * scale) >> 8;
          }
          else
          {
            appXangC           = (obj->process.vectors.miscB.X & 0xFFF);
            appXang            = CTGA(appXangC);
            unsigned long appY = CTCA(cos(appXang));
            
            dstVector->Y = ((appY/16) * scale) >> 8;  
          }
          
          obj->process.speed = scale;
        }
        break;
        
        case 3:
        {
        }
        break;
           
        case 4:
        case 5:
        {
          unsigned char inIndex = (instruction >> 12) & 7;
          unsigned char outIndex = (instruction >> 15) & 7;
          cvector *transIn = &((cvector*)&obj->process.vectors)[inIndex];
          cvector *transOut = &((cvector*)&obj->process.vectors)[outIndex];
          
          //should be dst
          cvector srcVector;
          srcVector.Z = translate(obj, dstVal(instruction));
          srcVector.Y = pop(obj);
          srcVector.X = pop(obj);
          
          if (subFunction == 4)
            transform(&srcVector, transIn, &obj->process.vectors.rot, &obj->process.vectors.scale, transOut);
          else
            transform(&srcVector, transIn, &obj->process.vectors.miscB, 0, transOut);
        }
        break; 

        case 6:
        {
          //should be dst
          left = translate(obj, dstVal(instruction));
          
          unsigned long linkIndex  = (instruction >> 21) & 7;
          unsigned long indexTrans = (instruction >> 12) & 7;
          
          cvector *destTrans = &((cvector*)&obj->process.vectors)[indexTrans];
          
          object *link = (object*)*((unsigned long*)&obj->process + linkIndex);
          
          if (link->process.aniSeq)
          {
            if (link->process.aniSeq[0] == 1)  //if animation involves svtx model
            {
              unsigned long svtxEID   = getWord(link->process.aniSeq, 4, true);
              entry *svtx             = crashSystemPage(svtxEID);
							
              unsigned long frame      = link->process.aniFrame >> 8;
              unsigned char *frameItem = svtx->itemData[frame];
              
              unsigned long tgeoEID   = getWord(frameItem, 4, true);
              entry *tgeo             = crashSystemPage(tgeoEID);
              
              unsigned long vertexIndex = left >> 8;
              
              unsigned char *vertexData = &frameItem[0x38 + (vertexIndex*6)];
              
              unsigned long modelXOffset = getWord(frameItem, 0x8, true);
              unsigned long modelYOffset = getWord(frameItem, 0xC, true);
              unsigned long modelZOffset = getWord(frameItem, 0x10, true);
              
              unsigned char vertexX = vertexData[5];
              unsigned char vertexY = vertexData[4];
              unsigned char vertexZ = vertexData[3];
              
              unsigned long vertexXFinal = (vertexX - 0x80) + modelXOffset;
              unsigned long vertexYFinal = (vertexY - 0x80) + modelYOffset;
              unsigned long vertexZFinal = (vertexZ - 0x80) + modelZOffset;
              
              cvector vertex;
              vertex.X = vertexXFinal << 10;
              vertex.Y = vertexYFinal << 10;
              vertex.Z = vertexZFinal << 10;
              
              unsigned char *tgeoItem1 = tgeo->itemData[0];
              unsigned long modelXScale = getWord(tgeoItem1, 0x4, true);
              unsigned long modelYScale = getWord(tgeoItem1, 0x8, true);
              unsigned long modelZScale = getWord(tgeoItem1, 0xC, true);
              
              unsigned long objXScale = link->process.vectors.scale.X;
              unsigned long objYScale = link->process.vectors.scale.Y;
              unsigned long objZScale = link->process.vectors.scale.Z;
              
              cvector scale;
              scale.X = (modelXScale * objXScale) >> 12;
              scale.Y = (modelYScale * objYScale) >> 12;
              scale.Z = (modelZScale * objZScale) >> 12;
              
              transform(&vertex, 
                        &link->process.vectors.trans, 
                        &link->process.vectors.rot,
                        &scale,
                        destTrans);
            }
          }      
        }
        break;
          
        case 7:
        {
          //TODO: sound matrix routine
        }
        break;
      }
      }
      break;
      
      case 0x86:
      {
        //________ BBBB____ __AAAAAA AAAAAAAA

        //loc_80022F60:
        unsigned long jumpLoc    = (instruction & 0x3FFF);
        unsigned long offset = (instruction >> 20) & 0xF;

        //oldfp = process[0xE4] - (process + 0x60);  //a1
        //oldsp = process[0xDC] - (process + 0x60);
        unsigned long frameFP = getFP(obj);
        unsigned long frameSP = getSP(obj) - (offset*4);
        
        //process[0xE4] = process[0xDC]; 
        obj->process.fp = obj->process.sp;
        
        //additional = bbits * 4; 
        //tag = (oldfp << 16 | oldsp);
        //process.push(var_70);
        push(obj, arg1);
        push(obj, obj->process.pc);
        push(obj, frameTag(frameFP, frameSP));

        unsigned long srcjal_c = (unsigned long)(obj->process.pc);
                
        //codeLocation = abits;
        if (jumpLoc != 0x3FFF)   //i.e. not all abits set
        {
          entry *codeEntry          = obj->global;
          unsigned char *codeSource = codeEntry->itemData[1]; 
          obj->process.pc           = (unsigned long*)&codeSource[jumpLoc*4];
        }
        else
          obj->process.pc           = 0;
        
        unsigned long dstjal_c = (unsigned long)(obj->process.pc);
        unsigned long srcjal = srcjal_c - (unsigned long)obj->global->itemData[1];
        unsigned long dstjal = dstjal_c - (unsigned long)obj->global->itemData[1];
            
        arg1 &= 0xFFFFFFF6;  //clear bits 1 & 4

        #ifdef DEBUG_GOOL
        if (getWord(obj->global->itemData[0], 0, true) == DEBUG_GOOL_OBJTYPE)
        {
          //printf("issuing jump and link instruction...\n");
          cout << "issuing jump and link instruction..." << endl;
          
          //this jump will not be terminated by RTEs...
          printObject(obj, arg1);
          
          char temp[0x200];
          sprintf(temp, "code location: %x", obj->global->itemData[1]);
          cout << temp << endl;
          sprintf(temp, "jump and link code src location: %x jump and link code dst location: %x", srcjal_c, dstjal_c);
          cout << temp << endl;
          sprintf(temp, "jump and link src location: %x jump and link dst location: %x", srcjal, dstjal); 
          cout << temp << endl;
        }
        #endif 
      }
      break;
    
      case 0x87:
      {
        unsigned long rindex = (instruction >> 21) & 0x7;
        recipient = (object*)*((unsigned long*)&obj->process + rindex);
      }
      case 0x8F:
      case 0x90:
      {
        dst = translate(obj, dstVal(instruction), endianD);
        unsigned long event = *(dst);
        
        obj->process.statusA &= 0xFFFDFFFF; //clear bit 18
        
        //bits 13-18 are memory location of value tested for existence
        long mem = (instruction >> 12) & 0x3F;
        
        bool cond;
        if (mem == 0x1F)
          cond = pop(obj);
        else
          cond = *((unsigned long*)&obj->process + mem);
      
        unsigned long argcount = (instruction >> 18) & 0x7; //min 1 max 8 args
        unsigned long mode     = (instruction >> 21) & 0x7; //mode for determining recipients when != 0x87 
      
        if (!dst || !cond || (opcode != 0x8F && !recipient))
          obj->process.miscFlag = 0;  //todo: rename to appropriate
        else
        {
          if (argcount)
          {
            unsigned long count = 0;
            do
            {
              signed long stackoffset = count - argcount;
              
              args[count++] = *(obj->process.sp + stackoffset);
              
            } while (count < argcount);
          }
                    
          if (opcode == 0x8F)
            issueToAll(obj, event, mode, argcount, args);
          else if (opcode == 0x87)
            issueEvent(obj, recipient, event, argcount, args);
          else if (opcode == 0x90)
          {
          //sub_800251B8(obj, recipient, event, dbits, argcount, args, count?);
          }
        }
        
        if ((obj->process.statusA & 0x20000) == 0)
        {
          for (int pback=0; pback<argcount; pback++)
            pop(obj);
        }
        else 
        {
          if ((arg1 & 2) == 0) { pop(obj); }
          else
            return 0xFFFFFF01;
        }
      }
      break;
      
      case 0x88:
      case 0x89:
      {
        if ((arg1 & 8) == 0) { return GOOL_CODE_SUCCESS; }
        
        unsigned char condType = (instruction >> 20) & 3;
        unsigned char retType  = (instruction >> 22) & 3;
        
        unsigned long cond;
        if (condType == 0) //no condition = jump
          cond = 1;
        else if (condType == 1)
        {
          long mem = (instruction >> 14) & 0x3F;
        
          unsigned long val;
          if (mem == 0x1F)
            val = pop(obj);
          else
            val = *((unsigned long*)&obj->process + mem);
        
          cond = val;
        }
        else if (condType == 2)
        {
          long mem = (instruction >> 14) & 0x3F;
        
          unsigned long val;
          if (mem == 0x1F)
            val = pop(obj);
          else
            val = *((unsigned long*)&obj->process + mem);
        
          cond = (val == 0);
        }
        else
          cond = 0;
          
        if (cond)  //else 22C7C
        {
          arg1 |= GOOL_XSTAT_RETURNEVENT;
          arg2.issue = (opcode == 0x89);
       
          //we store SP | FP
          //they store FP | SP
          //we should store like them to avoid confusion...
          
          if (retType == 0 || retType == 2)
          {          
            obj->process.sp = &obj->process.fp[2];
            
            unsigned long retFrameTag = peek(obj);         
            unsigned short retFrameSP = retFrameTag >> 16;
            unsigned short retFrameFP = retFrameTag & 0xFFFF;     
            
            unsigned long retStatus;
            if (retFrameFP != 0)                      //if not the initial frame
            {
              obj->process.sp = &obj->process.fp[1];
           
              obj->process.pc = (unsigned long*)peek(obj);            //return to the code for this frame
              pop(obj);
              
              obj->process.fp = (unsigned long*)((unsigned char *)&obj->process + (retFrameFP));
              obj->process.sp = (unsigned long*)((unsigned char *)&obj->process + (retFrameSP));
            
              retStatus = 0xFFFFFF01;
            }
            else
              retStatus = 0xFFFFFFE7;
            
            if (!isErrorCode(retStatus)) { arg2.id = 0xFF; }
            
            return retStatus;
          }
          else if (retType == 1) 
          {
            obj->process.sp = &obj->process.fp[2];
            
            unsigned long retFrameTag = peek(obj);         
            unsigned short retFrameSP = retFrameTag >> 16;
            unsigned short retFrameFP = retFrameTag & 0xFFFF;     
            
            unsigned long retStatus;
            if (retFrameFP != 0)                      //if not the initial frame
            {
              obj->process.sp = &obj->process.fp[1];
           
              obj->process.pc = (unsigned long*)peek(obj);            //return to the code for this frame
              pop(obj);
              
              obj->process.fp = (unsigned long*)((unsigned char *)&obj->process + (retFrameFP));
              obj->process.sp = (unsigned long*)((unsigned char *)&obj->process + (retFrameSP));
            
              retStatus = 0xFFFFFF01;
            }
            else
              retStatus = 0xFFFFFFE7;            
            
            if (!isErrorCode(retStatus)) 
            { 
              unsigned long sub = instruction & 0x3FFF;
              arg2.id = sub; 
            }
              
            return retStatus;
          }
          else if (retType == 3) { /*next instruction;*/ }
        }
        else 
        {
          if (retType == 0) 
          {
            signed short relAddr = (signed short)((instruction & 0x3FF) << 6) >> 6;
            obj->process.pc += relAddr;
        
            unsigned char stackSize = (instruction >> 10) & 0xF;
            obj->process.sp -= stackSize;
          }
        }
      }
      break;
      
      case 0x8A:
      case 0x91:
      {
        unsigned long spawnCount = (instruction & 0x3F);
        unsigned long argCount   = (instruction >> 20) & 0xF;
        
        unsigned long popCount = argCount;
        
        if (spawnCount == 0)
        {
          popCount++;
          spawnCount = *(obj->process.sp - popCount);
        }
          
        if (spawnCount > 0)
        {  
          bool flag = (opcode == 0x91);
         
          unsigned long count = 0;         
          do
          {
            unsigned long objType    = (instruction >> 12) & 0xFF;
            unsigned long objSubType = (instruction >>  6) & 0x3F;
            
            unsigned long *args = (obj->process.sp - argCount);
            object *child = addObject(obj, objType, objSubType, argCount, args, flag);
          
            if (isErrorCode((unsigned long)child)) { obj->process.miscChild = 0; }
            else                                   { obj->process.miscChild = child; }
            
            child->process.link[0] = obj;  
             
            count++;
          } while (count < spawnCount);
        }

        //then pop the words back
        obj->process.sp -= popCount;
      }
      break;
      
      
      //this instruction involves the entry/paging system which is clearly not yet implemented
      //until then we must simulate, and since only the success of this instruction would
      //hinder the execution of the overall gool binary we simulate its success and ignore its
      //effect (entries are all pre-loaded so we need not to page them in and out)
      case 0x8B:
      {
        left = translate(obj, srcVal(instruction));
        right = translate(obj, dstVal(instruction));

        switch (right)
        { 
          case 1:
          //obj->process.miscEntry = sub_80015118(left, 0, 1);
          obj->process.miscEntry = (entry*)1;
          break;
          case 2:
          //obj->process.miscEntry = sub_80015458(left, 1);
          obj->process.miscEntry = (entry*)1;
          break;
          case 3:
          //push(obj, sub_80015458(left, 0));
          push(obj, 1);
          break;
          case 4:
          //process.push(sub_800156D4(left));
          push(obj, 100);
          break;
          case 5:
          //args   = &(obj->process.sp - left)
          //result = sub_8001579C(args);
          obj->process.sp -= left;
          //push(obj, result);
          push(obj, left);
          break;
          case 6:
          //obj->process.miscEntry = sub_80015118(left, 1, 1);
          obj->process.miscEntry = (entry*)1;
          break;
        }
      }
      break;

      //we need to pop stack for the sound opcodes for now to avoid
      //stack overflow for code that pushes data intended for use by
      //these opcodes
      case 0x8C:
      {
        src = translate(obj, srcVal(instruction), endianS);
        dst = translate(obj, dstVal(instruction), endianD);
      }
      break;
      
      case 0x8D:
      {
        dst = translate(obj, dstVal(instruction), endianD);
        
        // optional implicit stack operand to account for
        unsigned long index = instruction >> 12;
        if (index == 0x1F)
          pop(obj);
        
      }
      break;
    }

    #ifdef DEBUG_GOOL
    if (getWord(obj->global->itemData[0], 0, true) == DEBUG_GOOL_OBJTYPE)
    {    
      //printf("\n");
      cout << endl;
      printObject(obj, arg1);
    }
    #endif
      
  } while (true);
}


inline unsigned long *translate(object *obj, unsigned short addr, bool &endian)
{
  endian = false;
  
  if ((addr & 0xFFF) == bitsPop)
    return --obj->process.sp;
  else if ((addr & 0xE00) == 0xE00)
  {
    unsigned short loc = addr & 0x1FF;
    return ((unsigned long*)&obj->process + loc);
  }
  else
    return transA(obj, addr, endian);
}
    
inline unsigned long translate(object *obj, unsigned short addr)
{
  long retval;
  
  if ((addr & 0xFFF) == bitsPop)
    retval = pop(obj);  
  else if ((addr & 0xE00) == 0xE00)
  {
    unsigned short loc = addr & 0x1FF;
    retval = *((unsigned long*)&obj->process + loc);
  }
  else
    retval = transA(obj, addr);

  return retval;	
}

inline unsigned long *translateP(object *obj, unsigned short addr, bool &endian)
{
  endian = false;
  
  if ((addr & 0xFFF) == bitsPop)
  {
    unsigned long *cur = obj->process.sp;
    obj->process.sp++;
  
    return cur;
  }
  else if ((addr & 0xE00) == 0xE00)
  {
    unsigned short loc = addr & 0x1FF;
    return ((unsigned long*)&obj->process + loc);
  }
  else
    return transA(obj, addr, endian);
}

inline unsigned long translateP(object *obj, unsigned short addr)
{
  long retval;
  
  if ((addr & 0xFFF) == bitsPop)
  {
    retval = *(obj->process.sp);
    obj->process.sp++;
  }    
  else if ((addr & 0xE00) == 0xE00)
  {
    unsigned short loc = addr & 0x1FF;
    retval = *((unsigned long*)&obj->process + loc);
  }
  else
    retval = transA(obj, addr);

  return retval;	
}

unsigned long transA(object *obj, unsigned short addr)
{
  long retval;
   
  if ((addr & 0x800) == 0)
  {
    unsigned short dat = addr & 0x3FF;  
    if ((addr & 0x400) == 0)
      retval = getWord(obj->global->itemData[2], dat*4, true);
    else
      retval = getWord(obj->external->itemData[2], dat*4, true);
  }
  else if ((addr & 0x400) == 0)
  {
    if ((addr & 0x200) == 0)
    {
      signed long curConst = (addr & 0x1FF) << 23; //shift up sign bit
      curConst >>= 15;
      
      if (lastConst == 0)
      {
        constBuffer[0] = curConst;
        retval = constBuffer[0];
      }
      else
      {
        constBuffer[1] = curConst;
        retval = constBuffer[1];
      }
      lastConst = !lastConst;
    }
    else if ((addr & 0x100) == 0)
    {
      signed long curConst = (addr & 0xFF) << 24; //shift up sign bit
      curConst >>= 20;
      
      if (lastConst == 0)
      {
        constBuffer[0] = curConst;
        retval = constBuffer[0];
      }
      else
      {
        constBuffer[1] = curConst;
        retval = constBuffer[1];
      }
      lastConst = !lastConst;
    }
    else if ((addr & 0x80) == 0)
    {
      signed char _addr = (addr & 0x3F);
      unsigned char sign = (addr & 0x40) >> 6;
      if (sign == 1)
        _addr = -(0x40 - _addr);
        
      retval = obj->process.fp[_addr];
    }
    else if (addr == 0xBE0)
    {            
      retval = 0;
    }
    else if (addr == 0xBF0)
    {            
      retval = 1;   
    }
    else
    {            
      //error       
    }
  }
  else
  {
    unsigned short linkIndex = ((addr >> 4) & 0x1C)>>2;
    unsigned short linkMem   = (addr & 0x3F);

    object *link = (object*)*((unsigned long*)&obj->process + linkIndex);
    
    if (link)
      retval       = *((unsigned long*)&link->process + linkMem);
    else
      retval       = 0;
  }
  
  return retval;
}

unsigned long *transA(object *obj, unsigned short addr, bool &endian)
{
  unsigned long *retval;
   
  if ((addr & 0x0800) == 0)
  {
    unsigned short dat = addr & 0x3FF;
    if ((addr & 0x400) == 0)
      retval = (unsigned long*)obj->global->itemData[2] + (dat);
    else
      retval = (unsigned long*)obj->external->itemData[2] + (dat);
      
    endian = true;
  }
  else if ((addr & 0x400) == 0)
  {
    if ((addr & 0x200) == 0)
    {
      signed long curConst = (addr & 0x1FF) << 23; //shift up sign bit
      curConst >>= 15;
      
      if (lastConst == 0)
      {
        constBuffer[0] = curConst;
        retval = (unsigned long*)&constBuffer[0];
      }
      else
      {
        constBuffer[1] = curConst;
        retval = (unsigned long*)&constBuffer[1];
      }
      
	    lastConst = !lastConst;
    }
    else if ((addr & 0x100) == 0)
    {
      signed long curConst = (addr & 0xFF) << 24; //shift up sign bit
      curConst >>= 20;
      
      if (lastConst == 0)
      {
        constBuffer[0] = curConst;
        retval = (unsigned long*)&constBuffer[0];
      }
      else
      {
        constBuffer[1] = curConst;
        retval = (unsigned long*)&constBuffer[1];
      }
      
	    lastConst = !lastConst;
    }
    else if ((addr & 0x80) == 0)
    {
      signed char _addr = (addr & 0x3F);
      unsigned char sign = (addr & 0x40) >> 6;
      if (sign == 1)
        _addr = -(0x40 - _addr);
         
      retval = &obj->process.fp[_addr];
    }
    else if (addr == 0xBE0)
    {            
      retval = 0;
    }
    else if (addr == 0xBF0)
    {            
      retval = (unsigned long*)1;   
    }
    else
    {            
      //error       
    }
  }
  else
  {
    unsigned short linkIndex = ((addr >> 4) & 0x1C)>>2;
    unsigned short linkMem   = (addr & 0x3F);

    object *link = (object*)*((unsigned long*)&obj->process + linkIndex);
    
    if (link)
      retval       = (unsigned long*)&link->process + linkMem;
    else
      retval       = 0;
  }
  
  return retval;
}
  
//unsigned long result = interpret(obj, 0x3, 0);

void printObject(object *obj, unsigned long state)
{
  unsigned char *codeHeader = obj->global->itemData[0];
  unsigned long initStack = getWord(codeHeader, 0xC, true) * sizeof(long);
  unsigned long memsize = getSP(obj);
  unsigned long framestart = getFP(obj);

  char temp[0x200];
  sprintf(temp, "current state: %x", state);
  cout << temp << endl;
  sprintf(temp, "current used memory: %x", memsize);
  cout << temp << endl;
  sprintf(temp, "stack range: %x | %x", framestart, memsize);
  cout << temp << endl;
  
  sprintf(temp, "memory/stack contents: ");
  cout << temp << endl;
  for (int i=initStack; i<memsize; i++)
  {
    if (((i-initStack) % 16) == 0)
    {
      sprintf(temp, "%03x:", i);
      cout << endl << temp;
    }
    
    if (i == framestart)
    {
      sprintf(temp, "(%02x", *((unsigned char*)&obj->process + i));
      cout << temp;
    }
    else if (i == memsize-1)
    {
      sprintf(temp, " %02x)", *((unsigned char*)&obj->process + i));
      cout << temp << endl;
    }
    else
    {
      sprintf(temp, " %02x", *((unsigned char*)&obj->process + i));
      cout << temp;
    }
  }
}


    
    