

#ifndef NSF_H
#define NSF_H

#include <stdlib.h>
#include <stdio.h>

#include "chunk.h"

#define CHUNKSIZE       0x10000
#define MAX_CHUNKS          148   
#define MAX_TEXTURECHUNKS    32

#define MAGIC_CHUNK      0x1234
#define MAGIC_COMPRESSED 0x1235

#define CHUNK_COMPRESSED_MAGIC  0x0
#define CHUNK_COMPRESSED_PAD    0x2
#define CHUNK_COMPRESSED_LENGTH 0x4
#define CHUNK_COMPRESSED_SKIP   0x8
#define CHUNK_COMPRESSED_DATA   0xC
#define CHUNK_COMPRESSED_HEADER CHUNK_COMPRESSED_DATA

#define MAX_TYPES 21

typedef struct
{
  unsigned short  magic;
  unsigned short    pad;
  unsigned long  length;
  unsigned long    skip;
}  chunkcmprHeader;

long decompressChunk(unsigned char *output, const unsigned char *input, chunkcmprHeader header);


class NSF
{
  private:
    FILE *NSFfile;                                 //a pointer to the FILE structure that reads the NSF file
    unsigned char *buffer;                         //a buffer to hold the entire NSF file
    long buffersize;                               //total size of the buffer/NSF file

    unsigned char chunkData[MAX_CHUNKS][CHUNKSIZE]; //approx 10 megs of ram for chunk data...

    void loadNSF(const char *file);
    unsigned char *readChunkData();                 //copies/decompresses chunk data from NSF file to chunkData array
    void unloadNSF();
    
    void buildLists();
	
  public:
    int countChunks;
    int countUncompressed;
    int countCompressed;
    int textureCount;

    chunkcmprHeader chunks_compressed[MAX_CHUNKS];
    chunk chunks[MAX_CHUNKS];
    
    chunk *textureChunks[MAX_TEXTURECHUNKS];
    
    entry *entries[MAX_TYPES][MAX_ENTRIES*3];
    unsigned long entryCount[MAX_TYPES];
    
    void init(const char *file);
    void readChunks(const char *file);
	
    int lookupChunkIndex(unsigned long CID);
    chunk *lookupChunk(unsigned long CID);
    int lookupEntryIndex(unsigned long EID, unsigned long CID);
    int lookupEntryIndex(unsigned long EID, int chunkIndex);
    entry *lookupEntry(unsigned long EID, unsigned long CID);
    entry *lookupEntry(unsigned long EID, int chunkIndex);

  	int lookupTextureChunkIndex(unsigned long EID);
};

#endif