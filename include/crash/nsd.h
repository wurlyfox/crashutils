#define system _system
#include <stdlib.h>
#include <stdio.h>
#undef system
#define _system system

#ifndef NSD_H
#define NSD_H

#include "nsf.h"

#define LISTINDICES_OFFSET   0x0
#define       COUNT_OFFSET 0x404
#define    LDAT_EID_OFFSET 0x408
#define LISTENTRIES_OFFSET 0x520

#define LISTENTRIES_OFFSET_PROTO LDAT_EID_OFFSET

#define LEVHEADER_MAGIC_OFFSET     0x0
#define LEVHEADER_LID_OFFSET       0x4
#define LEVHEADER_FIRSTZONE_OFFSET 0x8
#define LEVHEADER_FIRSTSECT_OFFSET 0xC
#define LEVHEADER_UNKNOWN_OFFSET   0x10
#define CODEENTRYLIST_OFFSET       0x14


#define MAX_LISTINDICES 0x100
#define MAX_ENTRYLISTS MAX_LISTINDICES
#define MAX_LISTENTRIES 0x200
#define MAX_LEVELEIDS    0x40

#define MAX_LEEIDSTRINGS MAX_LISTENTRIES
#define MAX_LVEIDSTRINGS MAX_LEVELEIDS

typedef struct
{
  unsigned long CID;
  unsigned long EID;
} listEntry;

typedef struct
{
  unsigned long magic;
  unsigned long LID;
  unsigned long firstZone;
  unsigned long firstSect;
  unsigned long unknown;
} levelHeader;

class NSD
{
  private:
    FILE *NSDfile;                                 //a pointer to the FILE structure that reads the NSD file
    unsigned char *buffer;                         //a buffer to hold the entire NSD file
    int buffersize;                                //total size of the buffer/NSD file

    unsigned char *levelEIDbuffer;                 //pointer to start of EIDs in the NSD file buffer

    void loadNSD(const char *file);
    void unloadNSD();
	  void readEntryLists();
    unsigned char *readListEntries(NSF *hasLdat=0);
    unsigned char *readLevelHeader(unsigned char *levelEIDbuffer);

    void readLevelEIDs(unsigned char *levelEIDbuffer);
	  
    
 public:
    int numListEntries;                            //number of list entries in the NSD file
    
	  int listIndices[MAX_LISTINDICES];              //256 relative offsets referring to positions in the listEntrys
    listEntry *entryLists[MAX_ENTRYLISTS];	

    long levelEIDs[MAX_LEVELEIDS];                 //global EIDs in the NSD file
    listEntry listEntries[MAX_LISTENTRIES];        //list entries in the NSD file

    char listEntryEIDstrings[MAX_LEEIDSTRINGS][6]; //identifiers for the list Entry EIDs
    char levelEIDstrings[MAX_LVEIDSTRINGS][6];     //identifiers for the global EIDs    

    levelHeader level;
    unsigned long ldat;
    
    void init(const char *file, NSF *hasLdat=0);
    void readNSD(const char *file, NSF *hasLdat=0);
	
    unsigned long lookupCID(unsigned long EID);
    unsigned long globalEID(int index);
    signed char lookupLevelEIDIndex(unsigned long EID);
};

#endif