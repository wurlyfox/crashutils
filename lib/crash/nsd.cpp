#include "nsd.h"
#include "nsf.h"

#include "eid.h"
#include "r3000a.h"

void NSD::loadNSD(const char *file)
{
  NSDfile = fopen(file, "rb");
  if (NSDfile==NULL) { exit (1); }

  // obtain file size:
  fseek(NSDfile, 0, SEEK_END);
  buffersize = ftell(NSDfile);
  rewind(NSDfile);

  //allocate memory to contain the whole file:
  buffer = (unsigned char*)malloc(buffersize);
  if (buffer == NULL) { exit (2); }

  // copy the file into the buffer:
  int result = fread(buffer,1,buffersize,NSDfile);
  if (result != buffersize) { exit (3); }
}

void NSD::unloadNSD()
{
  free(buffer);
  fclose(NSDfile);
}

void NSD::readEntryLists()
{
  int prevIndex = 0;
  for (int count = 0; count < MAX_ENTRYLISTS; count++)
  {
    
    listIndices[count] = getWord(buffer, count*sizeof(long), true);
    entryLists[count] = &listEntries[listIndices[count]];	
  }
}

unsigned char* NSD::readListEntries(NSF *hasLdat)
{
  numListEntries  = getWord(buffer, COUNT_OFFSET, true);  //number of list entries

  // quick trick for differentiating prototype NSDs from final NSDs:
  // in the proto, at this offset starts the listentries/entry hash tables
  // so the first word would be a CID
  // in the final, at this offset is the EID of the initial zone (thus beginning
  // the level header)
  // assuming max CID is 0x401, i.e. we can have no more than 256 chunks, and that
  // EIDs are normally large numbers, then we can do the following comparison
  
  bool isProto = (getWord(buffer, LISTENTRIES_OFFSET_PROTO, true) < 0x401);
  
  unsigned char *listEntryBuffer;
  if (!isProto)
    listEntryBuffer = &buffer[LISTENTRIES_OFFSET];
  else
    listEntryBuffer = &buffer[LISTENTRIES_OFFSET_PROTO];
    
  for (int count = 0; count < numListEntries; count++)
  {
    listEntries[count].CID = getWord(listEntryBuffer, (count*sizeof(long)*2), true);  
    listEntries[count].EID = getWord(listEntryBuffer, ((count*sizeof(long)*2) + sizeof(long)), true); 
      
    getEIDstring(listEntryEIDstrings[count], listEntries[count].EID);
  }
  
  if (isProto)
  {
    if (hasLdat->entryCount[6] > 0)
    {
      entry *ldat = hasLdat->entries[6][0];
      unsigned char *levelHeader = ldat->itemData[0];
    
      return levelHeader;
    }
  }
      
  return &buffer[LISTENTRIES_OFFSET + (numListEntries*sizeof(long)*2)];
}

void NSD::readLevelEIDs(unsigned char *levelEIDbuffer)
{
  for (int count = 0; count < MAX_LEVELEIDS; count++)
  {
    levelEIDs[count] = getWord(levelEIDbuffer, (count*sizeof(long)), true);
    getEIDstring(levelEIDstrings[count], levelEIDs[count]);
  }
}

unsigned char *NSD::readLevelHeader(unsigned char *levelEIDbuffer)
{
  level.magic        = getWord(levelEIDbuffer, LEVHEADER_MAGIC_OFFSET, true);
  level.LID          = getWord(levelEIDbuffer, LEVHEADER_LID_OFFSET, true);
  level.firstZone    = getWord(levelEIDbuffer, LEVHEADER_FIRSTZONE_OFFSET, true);
  level.firstSect    = getWord(levelEIDbuffer, LEVHEADER_FIRSTSECT_OFFSET, true);
  level.unknown      = getWord(levelEIDbuffer, LEVHEADER_UNKNOWN_OFFSET, true);
  
  ldat = getWord(buffer, LDAT_EID_OFFSET, true);
  
  return levelEIDbuffer + CODEENTRYLIST_OFFSET;
}

unsigned long NSD::lookupCID(unsigned long EID)
{
  if (EID == EID_NONE) { return 0; }
  
  int listIndex = ((EID >> 15) & 0xFF);
  
  listEntry *entryList = entryLists[listIndex];
  int count = 0;
  
  while (EID != entryList[count].EID)
  {
    if (count++ > MAX_LISTENTRIES)
	    return -1;
  }
  
  return entryList[count].CID;
}

unsigned long NSD::globalEID(int index)
{
  return levelEIDs[index];
}

signed char NSD::lookupLevelEIDIndex(unsigned long EID)
{
  int count = 0;
  while (EID != levelEIDs[count] && count < MAX_LEVELEIDS)
    count++;
     
  if (levelEIDs[count] != 0x6396374F) { return count; }
  else { return -1; }
}
   
//--------------------------

void NSD::init(const char *file, NSF *hasLdat)
{
  readNSD(file, hasLdat);
}

void NSD::readNSD(const char *file, NSF *hasLdat)
{
  loadNSD(file);

  levelEIDbuffer = readListEntries(hasLdat);
  levelEIDbuffer = readLevelHeader(levelEIDbuffer);
  readLevelEIDs(levelEIDbuffer);
  readEntryLists();
  
  unloadNSD();
}

//--------------------------

