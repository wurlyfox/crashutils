#ifndef CHUNK_H
#define CHUNK_H

#include "entry.h"

#define MAX_ENTRIES 100

#define CHUNK_MAGIC         0x0
#define CHUNK_TYPE          0x2
#define CHUNK_CID           0x4
#define CHUNK_ENTRYCOUNT    0x8
#define CHUNK_CHECKSUM      0xC
#define CHUNK_ENTRYOFFSETS 0x10

typedef struct
{
  unsigned short magic;
  unsigned short type;
  unsigned long CID;
  unsigned long entryCount;
  unsigned long checksum;

  unsigned char *entryData[MAX_ENTRIES];
  entry entries[MAX_ENTRIES];
  
  unsigned char *data;
  unsigned long entrySize[MAX_ENTRIES];
} chunk;


int readChunk(chunk &outChunk, unsigned char *buffer);
entry *lookupEntry(unsigned long EID, chunk *entryChunk);

#endif