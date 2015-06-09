#include "eid.h"

char alphaTable[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
                      'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
                      'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D',
                      'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
                      'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                      'Y', 'Z', '_', '!' 
                    };


void getEIDstring(char* _EIDstring, long EID)
{
  int shift = 1;                            //shift the string type bit/flag out

  for (int lp = 0; lp < 5; lp++)
  {
    int charIndex  = (EID >> shift) & 0x3F;
    _EIDstring[4 - lp] = alphaTable[charIndex];      
    shift += 6;
  }

  _EIDstring[5] = '\0'; 
    
}