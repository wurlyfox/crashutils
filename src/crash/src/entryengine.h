#ifndef ENTRYENGINE_H
#define ENTRYENGINE_H

#include "crash/r3000a.h"
#include "crash/nsf.h"
#include "crash/entry.h"

#include "objectengine.h"
#include "zoneengine.h"

struct subsystem
{
  char name[8];
  void (*initA)();
  unsigned long (*initB)();
  void (*initEntry)(entry *);
  void (*killA)();
  void (*killB)();
};

void initSubsystems();
void initAllEntries(NSF *nsfA);
void nullsub(entry *e);
void nullsub();

#endif