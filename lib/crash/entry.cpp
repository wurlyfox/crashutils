#include "entry.h"

#include "r3000a.h"

void readEntry(entry &outEntry, unsigned char *entryData)
{
  outEntry.magic     = getWord(entryData,     ENTRY_MAGIC, true);
  outEntry.EID       = getWord(entryData,       ENTRY_EID, true);
  outEntry.type      = getWord(entryData,      ENTRY_TYPE, true);
  outEntry.itemCount = getWord(entryData, ENTRY_ITEMCOUNT, true);
  
  unsigned long prevOffset = 0;
  for (int count = 0; count <= outEntry.itemCount; count++)
  {
    unsigned long offset      = getWord(entryData, ENTRY_ITEMOFFSETS+(count*4), true);
    
	  if (count != outEntry.itemCount)
	    outEntry.itemData[count]  = &entryData[offset];
	
  	if (count != 0)
	    outEntry.itemSize[count-1] = offset - prevOffset;
	  
	  prevOffset = offset;
  }
}