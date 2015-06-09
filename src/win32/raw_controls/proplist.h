#ifndef PROPLIST_H
#define PROPLIST_H

#include <windows.h>
#include <commctrl.h>

#include "crash/eid.h"
#include "crash/r3000a.h"

#define PROPERTYLIST_NULL    -1
#define PROPERTYLIST_DEFAULT  0

#define PROPERTY_WORD  4
#define PROPERTY_HWORD 2
#define PROPERTY_BYTE  1

#define PROPERTY_BITS       32
#define PROPERTY_INT        33
#define PROPERTY_HEX        34
#define PROPERTY_EID        35
#define PROPERTY_GOOLOFFSET 36
#define PROPERTY_SKIP       37

#define MAX_PROPERTIES 0x20

typedef struct
{
  char name[20];
  unsigned char size;
  unsigned char type;
  unsigned char offset;
  
} subProperty;
  
typedef struct
{
  char name[20];
  unsigned char size;
  unsigned char type;
  unsigned long offset;
 
  unsigned char subCount;
  subProperty bits[32];
} itemProperty;

class propertyList
{
  private:
  
  HWND hwnd;
  int ID;
  int type;

  itemProperty itemProperties[MAX_PROPERTIES];
  itemProperty *current;
  int propCount;
  
  unsigned long length;
  
 
  unsigned long curOffset;
  unsigned long curSuboffset;
  
  public:
  
  propertyList(int listID, int listType, int x, int y, int w, int h, HWND parent);
  
  void addProperty(char _name[20], unsigned long _offset, unsigned char _size, unsigned char _type);
  void addProperty(char _name[20], unsigned char _size, unsigned char _type);
  void addProperty(unsigned char _size);
  
  void addSubproperty(char _name[20], unsigned char _size, unsigned char _type);

  void setLength(unsigned long _length);
  
  bool addItem(unsigned char *itemData, unsigned long offset);
  bool occupy(unsigned char *itemData, unsigned long offset, int count);
  void clear();
  bool initColumns();
  
  int handle(LPARAM lParam);
};

#endif  