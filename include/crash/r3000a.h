#ifndef R3000A_H
#define R3000A_H

//#define getWord(b, l, e) (e ? ((b[l+3] << 24) | (b[l+2] << 16) | (b[l+1] << 8) | (b[l])) : ((b[l] << 24) | (b[l+1] << 16) | (b[l+2] << 8) | (b[l+3])))  
//#define getHword(b, l, e) (e ? ((b[l+1] << 8) | b[l]) : (b[l] | (b[l+1] << 8)))

unsigned long getWord(unsigned char *buffer, int location, bool endian);
unsigned short getHword(unsigned char *buffer, int location, bool endian);
void setWord(unsigned char *buffer, int location, unsigned long data, bool endian);
void setHword(unsigned char *buffer, int location, unsigned short data, bool endian);

#endif