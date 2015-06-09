#include "entryengine.h"

extern NSF *nsf;

subsystem subsystems[MAX_TYPES] = { 
{ "NONE", 0,           0,        0, 0, 0 },
{ "SVTX", 0,           0,        0, 0, 0 },
{ "TGEO", 0,           0,        0, 0, 0 },
{ "WGEO", 0,           0,        0, 0, 0 },
{ "SLST", 0,           0,        0, 0, 0 },
{ "TPAG", 0,           0,        0, 0, 0 },
{ "LDAT", 0,   initLevel,        0, 0, 0 },
{ "ZDAT", 0,           0, initZone, 0, 0 },
{ "CPAT", 0,           0,        0, 0, 0 },
{ "BINF", 0,           0,        0, 0, 0 },
{ "OPAT", 0, initObjects,        0, 0, 0 },
{ "GOOL", 0,           0,        0, 0, 0 },
{ "ADIO", 0,           0,        0, 0, 0 },
{ "MIDI", 0,           0,        0, 0, 0 },
{ "INST", 0,           0,        0, 0, 0 },
{ "IMAG", 0,           0,        0, 0, 0 },
{ "LINK", 0,           0,        0, 0, 0 },
{ "MDAT", 0,           0,        0, 0, 0 },
{ "IPAL", 0,           0,        0, 0, 0 },
{ "PBAK", 0,           0,        0, 0, 0 },
{ "CVTX", 0,           0,        0, 0, 0 } };
                            
void initSubsystems()
{
  for (int type = 0; type < MAX_TYPES; type++)
  {
    if (subsystems[type].initA)
      (*subsystems[type].initA)();
  }
  
  for (int type = 0; type < MAX_TYPES; type++)
  {
    if (subsystems[type].initB)
      (*subsystems[type].initB)();
  }
  
  initAllEntries(nsf);
}
    
void initAllEntries(NSF *nsfA)
{
  for (int type = 0; type < MAX_TYPES; type++)
  {
    if (subsystems[type].initEntry)
    {
      for (int count = 0; count < nsfA->entryCount[type]; count++)   
        (*subsystems[type].initEntry)(nsfA->entries[type][count]);
    }
  }
}

void nullsub() {}
void nullsub(entry *e) {}
                                           
