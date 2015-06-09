#include "r3000a.h"

unsigned long getWord(unsigned char *buffer, int location, bool endian)
{
  unsigned char A = buffer[location+0];
  unsigned char B = buffer[location+1];
  unsigned char C = buffer[location+2];
  unsigned char D = buffer[location+3];

  if (endian)
  {
    unsigned long littleLong = (D << 24) | (C << 16) | (B << 8) | A;
    return littleLong;
  }
  else
  {
    unsigned long bigLong    = (A << 24) | (B << 16) | (C << 8) | D;
    return bigLong;
  }
} 

unsigned short getHword(unsigned char *buffer, int location, bool endian)
{
  unsigned char A = buffer[location+0];
  unsigned char B = buffer[location+1];

  if (endian)
  {
    unsigned short littleShort = (B << 8) | A;
    return littleShort;
  }
  else
  {
    unsigned short bigShort    = (A << 8) | B;
    return bigShort;
  }
} 

/* TODO: THESE WOULD BE A LOT BETTER AS MACROS */

void setWord(unsigned char *buffer, int location, unsigned long data, bool endian)
{
  unsigned char *dataPtr = (unsigned char*)&data;
  unsigned char A = dataPtr[0];
  unsigned char B = dataPtr[1];
  unsigned char C = dataPtr[2];
  unsigned char D = dataPtr[3];

  if (endian)
  {
    buffer[location+3] = A;
    buffer[location+2] = B;
    buffer[location+1] = C;
    buffer[location+0] = D;
  }
  else
  {
    buffer[location+0] = A;
    buffer[location+1] = B;
    buffer[location+2] = C;
    buffer[location+3] = D;
  }
} 

void setHword(unsigned char *buffer, int location, unsigned short data, bool endian)
{
  unsigned char *dataPtr = (unsigned char*)&data;
  unsigned char A = dataPtr[0];
  unsigned char B = dataPtr[1];

  if (endian)
  {
    buffer[location+1] = A;
    buffer[location+0] = B;
  }
  else
  {
    buffer[location+0] = A;
    buffer[location+1] = B;
  }
} 
