#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "nsf.h"
#include "r3000a.h"

void NSF::loadNSF(const char *file)
{
  NSFfile = fopen(file, "rb");
  if (NSFfile==NULL) { exit (1); }

  // obtain file size:
  fseek(NSFfile, 0, SEEK_END);
  buffersize = ftell(NSFfile);
  rewind(NSFfile);

  //allocate memory to contain the whole file:
  buffer = (unsigned char*)malloc(buffersize);
  if (buffer == NULL) { exit (2); }

  // copy the file into the buffer:
  int result = fread(buffer,1,buffersize,NSFfile);
  if (result != buffersize) { exit (3); }
}

void NSF::unloadNSF()
{
  free(buffer);
  fclose(NSFfile);
}

unsigned char *NSF::readChunkData()
{
  long bufferPos = 0;

  countCompressed   = 0;
  countUncompressed = 0;
  countChunks       = 0;

  while (bufferPos < buffersize)
  {
    unsigned short magic = getHword(buffer, bufferPos, true);
  
    //chunkData[countChunks] = (unsigned char*)malloc(CHUNKSIZE);

    if (magic == MAGIC_CHUNK)
    {
      memcpy(chunkData[countChunks], &buffer[bufferPos], CHUNKSIZE);
      bufferPos += CHUNKSIZE;

      countUncompressed++;    
    }
    if (magic == MAGIC_COMPRESSED)
    {
      chunks_compressed[countCompressed].magic  = getHword(buffer, bufferPos+CHUNK_COMPRESSED_MAGIC, true);
      chunks_compressed[countCompressed].pad    = 0;
      chunks_compressed[countCompressed].length = getWord(buffer, bufferPos+CHUNK_COMPRESSED_LENGTH, true);
      chunks_compressed[countCompressed].skip   = getWord(buffer, bufferPos+CHUNK_COMPRESSED_SKIP, true);

      bufferPos += CHUNK_COMPRESSED_HEADER;
      bufferPos += decompressChunk(&chunkData[countChunks][0], &buffer[bufferPos], chunks_compressed[countCompressed]);
    
      countCompressed++;
    }

    countChunks++;
  }
}

long decompressChunk(unsigned char *output, const unsigned char *input, chunkcmprHeader header)
{
  unsigned short length = header.length;
  unsigned char span = 0;  
  unsigned char prefix = 0; 
  unsigned short seek = 0;

  long offsetIn  = 0;
  long offsetOut = 0;

  while (offsetOut < length)
  {
    prefix = input[offsetIn++];
    if ((prefix & 0x80) != 0)
    {
      unsigned char data = input[offsetIn++];

      span = data & 7; 
      seek = ((prefix & 0x7F) << 5) | (data >> 3);
        
      if (span == 7)
        span = 0x40;
      else
        span += 3;
      
      for (int lp = 0; lp < span; lp++)
        output[offsetOut + lp] = output[(offsetOut - seek) + lp];

      offsetOut +=   span;
    }
    else
    {
      memcpy(&output[offsetOut], &input[offsetIn], prefix);
      offsetIn  += prefix;
      offsetOut += prefix;
    }
  }
  
  int remainder = CHUNKSIZE - header.length;
  offsetIn += header.skip;
  memcpy(&output[offsetOut], &input[offsetIn], remainder);

  return offsetIn + remainder;
}

int cpOffset = 0;
bool cpEd = false;


int NSF::lookupChunkIndex(unsigned long CID)
{
  unsigned long desiredIndex = CID >> 1;
  
  while (chunks[desiredIndex + cpOffset].CID != CID) { cpOffset++; }
  return desiredIndex+cpOffset;
}

chunk *NSF::lookupChunk(unsigned long CID)
{
  //if (!cpEd)
  //{

  int chunkIndex = lookupChunkIndex(CID);
  return &chunks[chunkIndex];  //for now, since all chunks loaded into memory
}

int NSF::lookupEntryIndex(unsigned long EID, unsigned long CID)
{
  chunk *entryChunk = lookupChunk(CID);
  for (int count=0; count < entryChunk->entryCount; count++)
  {
    if (entryChunk->entries[count].EID == EID)
	  return count;
  }
  
  return -1;
}

entry *NSF::lookupEntry(unsigned long EID, unsigned long CID)
{
  chunk *entryChunk = lookupChunk(CID);
  for (int count=0; count < entryChunk->entryCount; count++)
  {
    if (entryChunk->entries[count].EID == EID)
	  return &entryChunk->entries[count];
  }
  
  return (entry*)0;  
}


int NSF::lookupEntryIndex(unsigned long EID, int chunkIndex)
{
  chunk *entryChunk = &chunks[chunkIndex];
  
  for (int count=0; count < entryChunk->entryCount; count++)
  {
    if (entryChunk->entries[count].EID == EID)
	  return count;
  }
  
  return -1;
}

entry *NSF::lookupEntry(unsigned long EID, int chunkIndex)
{
  chunk *entryChunk = &chunks[chunkIndex];
  for (int count=0; count < entryChunk->entryCount; count++)
  {
    if (entryChunk->entries[count].EID == EID)
	  return &entryChunk->entries[count];
  }
  
  return (entry*)0;
}

int NSF::lookupTextureChunkIndex(unsigned long EID)
{
  for (int count=0; count < textureCount; count++)
  {
    if (textureChunks[count]->CID == EID)
	  return count;
  }
  
  return -1;
}

//-------
void NSF::init(const char *file)
{
  readChunks(file);
  buildLists();
}

void NSF::readChunks(const char *file)
{
  loadNSF(file);
  readChunkData();
  unloadNSF();

  for (int count=0; count < countChunks; count++)
  {
    int type = readChunk(chunks[count], chunkData[count]);
    
    if (type == 1)
    {
      textureChunks[textureCount] = &chunks[count];
      textureCount++;
    }
  } 
} 

void NSF::buildLists()
{
  for (int count=0; count < MAX_TYPES; count++)
    entryCount[count] = 0;
    
  for (int count=0; count < countChunks; count++)
  {
    int type = chunks[count].type;
    
    if (type == 0)
    {
      for (int count2=0; count2 < chunks[count].entryCount; count2++)
      {
        entry *curEntry = &chunks[count].entries[count2];
        if (!curEntry)
          break;
        entries[curEntry->type][entryCount[curEntry->type]++] = curEntry;
      }
    }
  } 
}