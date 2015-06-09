#include "entrydrop.h"

entryDrop::entryDrop(int listID, int listType, int x, int y, int w, int h, HWND parent) :
drop(listID, x, y, w, h, parent)
{
  type = listType;
}

void entryDrop::occupy()
{
  switch (type)
  {
    case ENTRYDROP_GOOL:
    {
      if (nsd)
      {
        for (int execIndex=0; execIndex<0x40; execIndex++)
        {
          if (lookupAddEntry(nsd->levelEIDs[execIndex]))
            itemMap[execIndex] = itemCount;
          else
            itemMap[execIndex] = 0;
        }
      }
    }
  }
}   

int entryDrop::lookupAddEntry(unsigned long EID)
{
  char eidString[6];
  getEIDstring(eidString, EID);
  
  if (EID != EID_NONE)
  {
    unsigned long parentCID = nsd->lookupCID(EID);
    
    if (parentCID != -1)
    {
      chunk *parentChunk      = nsf->lookupChunk(parentCID);
      entry *newEntry         = lookupEntry(EID, parentChunk);
      
      addItem(eidString, (void*)newEntry);
    }
    else
    {
      //notify user elsewhere that gool executables list refers
      //to an exec that doesnt exist in the nsf
    }
  }
  else
    return 0;
    
  return 1;
} 

void entryDrop::selectEntry(int index)
{
  if (type == ENTRYDROP_GOOL)
  {
    if (int sel = itemMap[index])
      selectItem(sel-1);
  }
}