#ifndef ZONELIST_H
#define ZONELIST_H

#include <windows.h>
#include <stdio.h>

#include "win32/raw_controls/tree.h"

#include "crash/nsd.h"
#include "crash/nsf.h"
#include "crash/eid.h"
#include "crash/entry.h"
#include "crash/r3000a.h"

#define ZONELIST_UNCHANGED  0x0000
#define ZONELIST_RCLICK     0x0001
#define ZONELIST_SELCHANGED 0x0002

class zoneList
{
  private:
  
  NSF *nsf;
  NSD *nsd;
  
  HWND hwnd;
  int ID;
  
  unsigned long selectedIndex;
  
  unsigned long modelCount;
  unsigned long headerColCount;
  unsigned long sectionCount;
  unsigned long entityCount; 
  unsigned long neighborCount;
  
  entry *zoneModel[8];
  entry *zoneNeighbor[8];
  
  bool rClick;

  public:
  
  zoneList(int listID, NSF *_nsf, NSD *_nsd, int x, int y, int w, int h, HWND parent);
  
  void occupy(entry *zone);
  
  int handle(LPARAM lParam);
  unsigned int getSelectedIndex() { return selectedIndex; }
  void clear();
};

#endif