#ifndef ENTRYDROP_H
#define ENTRYDROP_H

#include "win32/raw_controls/drop.h"

#include "crash/nsf.h"
#include "crash/nsd.h"
#include "crash/eid.h"

#define ENTRYDROP_NULL    0
#define ENTRYDROP_GOOL    1

class entryDrop : public drop
{
  protected:
  
  NSF *nsf;
  NSD *nsd;
  
  int type;
  unsigned char itemMap[0x40];
  
  public:
  
  entryDrop(int listID, int listType, int x, int y, int w, int h, HWND parent);
  int setNSFNSD(NSF *newNSF, NSD *newNSD) { nsf = newNSF; nsd = newNSD; }
  
  int lookupAddEntry(unsigned long EID);
  void occupy();
  //void clear();

  void selectEntry(int index);
  entry *selectedEntry;
};

#endif
