#ifndef ENTRY_H
#define ENTRY_H

#define MAX_ITEMS 100

#define ENTRY_MAGIC         0x0
#define ENTRY_EID           0x4
#define ENTRY_TYPE          0x8
#define ENTRY_ITEMCOUNT     0xC
#define ENTRY_ITEMOFFSETS  0x10

typedef struct
{
  unsigned long magic;
  unsigned long EID;
  unsigned long type;
  unsigned long itemCount;

  unsigned char *itemData[MAX_ITEMS];
  unsigned long itemSize[MAX_ITEMS];

} entry;

void readEntry(entry &outEntry, unsigned char *entryData);

#endif