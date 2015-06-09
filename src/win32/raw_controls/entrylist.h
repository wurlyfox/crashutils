#ifndef ENTRYLIST_H
#define ENTRYLIST_H

#include <windows.h>

#include "win32/raw_controls/tree.h"

#include "crash/nsf.h"
#include "crash/nsd.h"
#include "crash/eid.h"

#define ENTRYLIST_NULL    0
#define ENTRYLIST_DEFAULT 1
#define ENTRYLIST_NSF     2
#define ENTRYLIST_NSD     3 

#define ENTRYLIST_UNCHANGED  100
#define ENTRYLIST_SELCHANGED 101
#define ENTRYLIST_RCLICK     102

class entryList
{
  private:
  
  HWND hwnd;
  int ID;
  int type;
  
  NSF *nsf;
  NSD *nsd;
  
  bool rClick;  //workaround for right clicking the list
  
  public:
  
  entryList(int listID, int listType, NSF *nsfA, NSD *nsdA, int x, int y, int w, int h, HWND parent);
  int setNSFNSD(NSF *nsfA, NSD *nsdA);
  
  int lookupAddEntry(unsigned long EID);
  int addEntry(unsigned long EID);
  int occupy();
  void clear();

  int handle(LPARAM lParam);
  
  chunk selectedChunk;
  int selChunkIndex;
  entry selectedEntry;
  int selEntryIndex;
  unsigned char *selectedItem;
  int selItemIndex;
    
};

#endif