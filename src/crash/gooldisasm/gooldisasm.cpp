#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gooldisasm.h"

const char *opcodeTable[] = {
  "ADD", "SUB", "MULT", "DIV", "CEQ", "ANDL", "ORL", "ANDB", 
  "ORB", "SLT",  "SLE", "SGT" ,"SGE",  "MOD", "XOR",  "TST",
  "RND","MOVE", "NOTL","0x13", "LEA" , "SHA","PSHA", "NOTB",    
 "MOVC", "ABS",  "PAD","0x1B", "MSC", "0x1D","0x1E", "0x1F",
 "0x20","0x21", "0x22","PLCV","WLCV", "0x25","PSHB", "0x27",
 "0x28","0x29", "0x2A","0x2B","0x2C", "0x2D","0x2E", "0x2F",
 "0x30","0x31", "0x32","0x33","0x34", "0x35","0x36", "0x37",
 "0x38","0x39", "0x3A","0x3B","0x3C", "0x3D","0x3E", "0x3F",
 "0x40","0x41", "0x42","0x43","0x44", "0x45","0x46", "0x47",
 "0x48","0x49", "0x4A","0x4B","0x4C", "0x4D","0x4E", "0x4F",
 "0x50","0x51", "0x52","0x53","0x54", "0x55","0x56", "0x57",
 "0x58","0x59", "0x5A","0x5B","0x5C", "0x5D","0x5E", "0x5F",
 "0x60","0x61", "0x62","0x63","0x64", "0x65","0x66", "0x67",
 "0x68","0x69", "0x6A","0x6B","0x6C", "0x6D","0x6E", "0x6F",
 "0x70","0x71", "0x72","0x73","0x74", "0x75","0x76", "0x77",
 "0x78","0x79", "0x7A","0x7B","0x7C", "0x7D","0x7E", "0x7F",
  "NOP", "NOP", "0x82","ANIS","ANIF", "VECT"," JAL", "0x87",
  "ERR", "ERR", "CHLD","NTRY","SNDA", "SNDB","0x8E", "0x8F",
 "0x90","CHDN", "0x92","0x93","0x94", "0x95","0x96", "0x97",
 "0x98","0x99", "0x9A","0x9B","0x9C", "0x9D","0x9E", "0x9F",
 "0xA0","0xA1", "0xA2","0xA3","0xA4", "0xA5","0xA6", "0xA7",
 "0xA8","0xA9", "0xAA","0xAB","0xAC", "0xAD","0xAE", "0xAF",
 "0xB0","0xB1", "0xB2","0xB3","0xB4", "0xB5","0xB6", "0xB7",
 "0xB8","0xB9", "0xBA","0xBB","0xBC", "0xBD","0xBE", "0xBF",
 "0xC0","0xC1", "0xC2","0xC3","0xC4", "0xC5","0xC6", "0xC7",
 "0xC8","0xC9", "0xCA","0xCB","0xCC", "0xCD","0xCE", "0xCF",
 "0xD0","0xD1", "0xD2","0xD3","0xD4", "0xD5","0xD6", "0xD7",
 "0xD8","0xD9", "0xDA","0xDB","0xDC", "0xDD","0xDE", "0xDF",
};

goolDisasm::goolDisasm()
{
  pc                = 0;
  instructionBuffer = 0;
  opcodeBuffer      = 0;
}

goolDisasm::goolDisasm(unsigned char *code, int length)
{
  buffer            =   code;
  buffersize        = length;
  pc                =      0;
  instructionBuffer =      0;
  opcodeBuffer      =      0;
}

void goolDisasm::setSource(unsigned char *code, int length)
{
  buffer     =   code;
  buffersize = length;
}

bool goolDisasm::lastInstruction()
{
  if ((pc*sizeof(long)) > (buffersize - 4))
  {
    return true;
  }
  else
  {
    return false;
  }
}

long goolDisasm::getInstruction(int offset)
{
  instructionBuffer = getWord(buffer, offset*4, true);
  return instructionBuffer;
}

long goolDisasm::prevInstruction()
{
  pc--;
  instructionBuffer = getWord(buffer, pc*4, true);
  return instructionBuffer;
}

long goolDisasm::nextInstruction()
{

  instructionBuffer = getWord(buffer, pc*4, true);
  pc++;
  return instructionBuffer;
}

unsigned char goolDisasm::getOpcode(int offset)
{
  long instr = getWord(buffer, offset*4, true);
  opcodeBuffer = (instr & 0xFF000000)>>24;
  return opcodeBuffer;
}

unsigned char goolDisasm::getOpcode(long instr)
{
  opcodeBuffer = (instr & 0xFF000000)>>24;
  return opcodeBuffer; 
} 

unsigned char goolDisasm::getOpcode()
{
  opcodeBuffer = (instructionBuffer & 0xFF000000)>>24;
  return opcodeBuffer;
}

long *goolDisasm::getOperands(int offset)
{
  instructionBuffer = getWord(buffer, offset*4, true);
  opcodeBuffer = (instructionBuffer & 0xFF000000) >> 24;
  return getOperands(instructionBuffer);
}

long *goolDisasm::getOperands(long instr)
{
  opcodeBuffer = (instr & 0xFF000000) >> 24;
  return getOperands(instr, opcodeBuffer);
}

long *goolDisasm::getOperands(long instr, unsigned char opcode)
{
  long operands = instr & 0x00FFFFFF;

  if (opcode == 0x18)
  {
    operandBuffer[0] = operands & 0x3FFF;
    operandBuffer[1] = (operands >> 14) & 0x3F;
  }
  else if (opcode == 0x1A)
  {
    operandBuffer[0] = operands & 0xFFF;
    operandBuffer[1] = (operands >> 12) & 3;
    operandBuffer[2] = (operands >> 14) & 3;
    operandBuffer[3] = (operands >> 16) & 0xF;
    operandBuffer[4] = (operands >> 20) & 1;
  }
  else if (opcode == 0x1C)
  {
    operandBuffer[0] = operands & 0xFFF;
    operandBuffer[1] = (operands >> 12) & 0x7;
    operandBuffer[2] = (operands >> 15) & 0x1F;
    operandBuffer[3] = (operands >> 20) & 0xF;
  }
  else if (opcode == 0x23 || opcode == 0x24)
  {
    operandBuffer[0] = operands & 0xFFF;
    operandBuffer[1] = (operands >> 12) & 0x7;
    operandBuffer[2] = (operands >> 15) & 0x3F;
  } 
  else if (opcode == 0x26)
  {
    operandBuffer[0] = operands & 0xFFF;
    operandBuffer[1] = (operands >> 12) & 0x3;
    operandBuffer[2] = (operands >> 14) & 0x3;
    operandBuffer[3] = (operands >> 16) & 0xF;
    operandBuffer[4] = (operands >> 20) & 1;
  }
  else if (opcode == 0x82)
  {
    operandBuffer[0] = operands & 0x3FFF;
    operandBuffer[1] = (operands >> 14) & 0x3F;
    operandBuffer[2] = (operands >> 20) & 3;
    operandBuffer[3] = (operands >> 22) & 3;
  }
  else if (opcode == 0x83)
  {
    operandBuffer[0] = operands & 0x7F;
    operandBuffer[1] = (operands >> 7) & 0x1FF;
    operandBuffer[2] = (operands >> 16) & 0x3F;
    operandBuffer[3] = (operands >> 22) & 0x3;
  }
  else if (opcode == 0x84)
  {
    operandBuffer[0] = operands & 0xFFF;
    operandBuffer[1] = (operands >> 16) & 0x3F;
    operandBuffer[2] = (operands >> 22) & 0x3;
  }
  else if (opcode == 0x85)
  {
    operandBuffer[0] = operands & 0xFFF;        //src value
    operandBuffer[1] = (operands >> 12) & 0x7;  //inout vector (2)
    operandBuffer[2] = (operands >> 15) & 0x7;  //misc vector  (3)
    operandBuffer[3] = (operands >> 18) & 0x7;  //subfunction  (1)
    operandBuffer[4] = (operands >> 21) & 0x7;  //misc vector  (4)
  }
  else if (opcode == 0x86)
  {
    operandBuffer[0] = operands & 0x3FFF;
    operandBuffer[1] = (operands >> 20) & 0xF;
  }
  else if (opcode == 0x87 || opcode == 0x8F || opcode == 0x90)
  {
    operandBuffer[0] = operands & 0xFFF;
    operandBuffer[1] = (operands >> 12) & 0x3F;
    operandBuffer[2] = (operands >> 18) & 7;
    operandBuffer[3] = (operands >> 21) & 7;
  }
  else if (opcode == 0x88 || opcode == 0x89)
  {
    operandBuffer[0] = operands & 0x3FFF;
    operandBuffer[1] = (operands >> 14) & 0x3F;
    operandBuffer[2] = (operands >> 20) & 0x3;
    operandBuffer[3] = (operands >> 22) & 0x3;
  }
  else if (opcode == 0x8A || opcode == 0x91)
  {
    operandBuffer[0] = operands & 0x3F;
    operandBuffer[1] = (operands >> 6) & 0x3F;
    operandBuffer[2] = (operands >> 12) & 0xFF;
    operandBuffer[3] = (operands >> 20) & 0xF;
  }
  else if (opcode == 0x8D)
  {
    operandBuffer[0] = operands & 0x3FFF;
    operandBuffer[1] = (operands >> 12) & 0x3F;
    operandBuffer[2] = (operands >> 18) & 0x3;
    operandBuffer[3] = (operands >> 20) & 0x3;
    operandBuffer[3] = (operands >> 22) & 1;
    operandBuffer[4] = (operands >> 23) & 1;
  }
  else if (opcode == 0x8E)
  {
    operandBuffer[0] = operands & 0xFFF;
    operandBuffer[1] = (operands >> 12) & 0x7;
    operandBuffer[2] = (operands >> 15) & 0x7;
    operandBuffer[3] = (operands >> 18) & 0x7;
  }
  else
  {
    operandBuffer[0] = operands & 0xFFF;
    operandBuffer[1] = (operands & 0xFFF000) >> 12;
  }

  return operandBuffer;
}

long *goolDisasm::getOperands()
{
  return getOperands(instructionBuffer, opcodeBuffer);
}

const char *goolDisasm::getOpcodeID(unsigned char opcode)
{
  if (opcode >= 0 && opcode < 0xDF)
  {
    return opcodeTable[opcode];
  }
}

void goolDisasm::getAddrFormat(char *sBuf, unsigned short addr, int mode)
{
  if ((addr & 0xFFF) == 0xE1F)
  {
    sprintf(sBuf, "pop()");
  }
  else if ((addr & 0xE00) == 0xE00)
  {
    unsigned short link = addr & 0x1FF;
    if (mode == 0)
      sprintf(sBuf, "process[0x%x]", (link*4)+0x60);
    else
      sprintf(sBuf, "process[0x60+0x%x]", link*4);
  }
  else if ((addr & 0x0800) == 0)
  {
    unsigned short eidIndex = addr & 0x3FF;  
    if ((addr & 0x400) == 0)
    {
      if (mode == 0)
        sprintf(sBuf, "static[0x%x]", eidIndex*4);
      else
        sprintf(sBuf, "process[0x20][0x18][0x%x]", eidIndex*4); 
    }
    else
    {
      if (mode == 0)
        sprintf(sBuf, "externstatic[0x%x]", eidIndex*4);
      else
        sprintf(sBuf, "process[0x24][0x18][0x%x]", eidIndex*4); 
    }
  }
  else if ((addr & 0x400) == 0)
  {
    if ((addr & 0x200) == 0)
    {
      long _addr = (addr & 0xFF);
      if (_addr != 0)
      {
        unsigned char sign = (addr & 0x100) >> 8;
        if (sign)
        {
          _addr = 0x100 - _addr;
          sprintf(sBuf, "-0x%x00", _addr);
        }
        else
          sprintf(sBuf, "0x%x00", _addr);
      }
      else
        sprintf(sBuf, "0");
    }
    else if ((addr & 0x100) == 0)
    {
      long _addr = (addr & 0x7F);
      if (_addr != 0)
      {
        unsigned char sign = (addr & 0x80) >> 7;
        if (sign)
        {
          _addr = 0x80 - _addr;
          sprintf(sBuf, "-0x%x0", _addr);
        }
        else
          sprintf(sBuf, "0x%x0", _addr);
      }
      else
        sprintf(sBuf, "0");
    }
    else if ((addr & 0x80) == 0)
    {
      unsigned char sign = (addr & 0x40) >> 6;
      unsigned short offset = (addr & 0x3F) << 2;
      
      if (mode == 0)
      { 
        
        if (sign == 1) 
        { 
          offset = 0x100 - offset;
          sprintf(sBuf,"-0x%x($fp)", offset);
        }
        else
          sprintf(sBuf, "0x%x($fp)", offset);
      }
      else
      {
        sprintf(sBuf,"process[0xE4][0x%x]",offset);
      }
    }
    else if (addr == 0xBE0)
    {            
      if (mode == 0)
      {
        sprintf(sBuf, "false");        
      }
      else
      {
        sprintf(sBuf, "0");
      }
    }
    else if (addr == 0xBF0)
    {            
      if (mode == 0)
      {
        sprintf(sBuf, "true");        
      }
      else
      {
        sprintf(sBuf, "1");
      }       
    }
    else
    {            
      sprintf(sBuf, "ERROR");        
    }
  }
  else
  {
    if (mode == 0)
    {
      unsigned short linkA = ((addr >> 4) & 0x1C)>>2;
      unsigned short linkB = (addr & 0x3F);

	  if (linkA == 0)
        sprintf(sBuf, "self[0x%x]", 0x60+(linkB*4));
	  else if (linkA == 1)
	    sprintf(sBuf, "parent[0x%x]", 0x60+(linkB*4));
	  else if (linkA == 2)
	    sprintf(sBuf, "sibling[0x%x]", 0x60+(linkB*4));
	  else if (linkA == 3)
	    sprintf(sBuf, "child[0x%x]", 0x60+(linkB*4));
	  else
	    sprintf(sBuf, "link%i[0x%x]", linkA, 0x60+(linkB*4));
    }
    else
    {
      unsigned short linkA = (addr >> 4) & 0x1C;
      unsigned short linkB = (addr & 0x3F) << 2;

      sprintf(sBuf, "process[0x60+0x%x][0x60+0x%x]", linkA, linkB); 
    }
  }
}

void goolDisasm::getOperandFormat(char *instString, unsigned char opcode, long *operands, bool lineBreak)
{
  char sBuf[40];
  char dBuf[40];

  const char *_opcodeID = getOpcodeID(opcode);
  char opcodeID[10];
  sprintf(opcodeID, "%-8s", _opcodeID);
  
  if (opcode <= 0x27 || (opcode >= 0x82 && opcode <= 0x91))
  {
    if (opcode == 0x11)
    {
      unsigned short dst = operandBuffer[0];
      unsigned short src = operandBuffer[1];
       
      if (dst == 0xE1F)
        sprintf(dBuf, "push()");
      else
        getAddrFormat(dBuf, dst, 0);
        
      getAddrFormat(sBuf, src, 0); 
	 	    
      if (!lineBreak)
        sprintf(instString, "%s %s,%s", opcodeID, sBuf, dBuf);     
      else
        sprintf(instString, "%s %s, %s\r\n", opcodeID, sBuf, dBuf);
    }
    else if (opcode == 0x18)
    {
      unsigned long codeOffset = operandBuffer[0] * 4;
      unsigned long destOffset = 0x60 + (operandBuffer[1] * 4);
      
      if (operandBuffer[1] == 0x1F)
        sprintf(dBuf, "code[0x%04x], push()", codeOffset);
      else
        sprintf(dBuf, "code[0x%04x], process[0x%x]", codeOffset, destOffset);
        
    	if (!lineBreak)
        sprintf(instString, "%s%s", opcodeID, dBuf);
      else
	      sprintf(instString, "%s%s\r\n", opcodeID, dBuf);
        
      return;
    }
    else if (opcode == 0x1A)
    {
      sprintf(dBuf, "0x%03x, %i, %i, %i", operandBuffer[0], operandBuffer[1], operandBuffer[2], operandBuffer[3]);

      if (!lineBreak)
        sprintf(instString, "%s%s", opcodeID, dBuf);     
      else
	    sprintf(instString, "%s%s\r\n", opcodeID, dBuf);

       
      return;
    }
    else if (opcode == 0x1C)
    {
      unsigned short src = operandBuffer[0];
      getAddrFormat(sBuf, src, 0);
      
      sprintf(dBuf, "%s, %i, 0x%x, %i", sBuf, operandBuffer[1], operandBuffer[2], operandBuffer[3]);
        
      if (!lineBreak)
        sprintf(instString, "%s%s", opcodeID, dBuf);     
      else
  	    sprintf(instString, "%s%s\r\n", opcodeID, dBuf);

      return;
    
    }
    else if (opcode == 0x23)
    {      
      int link = (operandBuffer[1]*4) + 0x60;
      sprintf(dBuf, "process[0x%x], %i", link, operandBuffer[2]);
      
	    if (!lineBreak)
        sprintf(instString, "%s%s", opcodeID, dBuf);     
      else
	    sprintf(instString, "%s%s\r\n", opcodeID, dBuf);

        
      return;
    } 
    else if (opcode == 0x24)
    {
      unsigned short src = operandBuffer[0];
      getAddrFormat(sBuf, src, 0);
      
      int link = (operandBuffer[1]*4) + 0x60;
      sprintf(dBuf, "%s, process[0x%x], %i", sBuf, link, operandBuffer[2]);
    
      if (!lineBreak)
        sprintf(instString, "%s%s", opcodeID, dBuf);     
      else
	    sprintf(instString, "%s%s\r\n", opcodeID, dBuf);
 
      return;
    }
    else if (opcode == 0x82)
    {
      int test = operandBuffer[1];
      if (operandBuffer[3] == 0)
      {
        if (operandBuffer[2] == 0)
          sprintf(opcodeID, "%-8s", "J");
        else if (operandBuffer[2] == 1)
          sprintf(opcodeID, "%-8s", "BNEZ");
        else if (operandBuffer[2] == 2)
          sprintf(opcodeID, "%-8s", "BEQZ");
        else
          sprintf(opcodeID, "%-8s", "Jv48");

	    	int jump = operandBuffer[0] & 0x3FF;
	      int framesize = (operandBuffer[0] & 0x3C00) >> 10;
        if (test == 0x1F)
          sprintf(dBuf, "0x%x($pc), -0x%x", jump * 4, framesize);
        else
          sprintf(dBuf, "0x%x($pc), -0x%x, process[0x%x]", jump * 4, framesize, 0x60 + (test*4));
      }
      else if (operandBuffer[3] == 1)
      {
        if (operandBuffer[2] == 0)
          sprintf(opcodeID, "%-8s", "JALR");
        else if (operandBuffer[2] == 1)
          sprintf(opcodeID, "%-8s", "BNEZAL");
        else if (operandBuffer[2] == 2)
          sprintf(opcodeID, "%-8s", "BEQZAL");
        else
          sprintf(opcodeID, "%-8s", "JALv48");
 
        if (test == 0x1F)
          sprintf(dBuf, "subID(%i)", operandBuffer[0]);
        else
          sprintf(dBuf, "subID(%i), process[0x%x]", operandBuffer[0], 0x60 + (test*4));
         
      }   
      else if (operandBuffer[3] == 2)
      {
        if (operandBuffer[2] == 0)
          sprintf(opcodeID, "%-8s", "RTE");
        else if (operandBuffer[2] == 1)
          sprintf(opcodeID, "%-8s", "RTN");
        else if (operandBuffer[2] == 2)
          sprintf(opcodeID, "%-8s", "RTZ");
        else
          sprintf(opcodeID, "%-8s", "RTv48");
      
        if (test == 0x1F)
          sprintf(dBuf, " ");
        else
          sprintf(dBuf, " (process[0x%x])", 0x60+(test*4));
      }  

      if (!lineBreak)
        sprintf(instString, "%s%s", opcodeID, dBuf);     
      else
	      sprintf(instString, "%s%s\r\n", opcodeID, dBuf);
		
      return;
    }      
    else if (opcode == 0x83)
    {
      
      sprintf(dBuf, "%i, %i, %i, %i", operandBuffer[0], operandBuffer[1], operandBuffer[2], operandBuffer[3]);
    
      if (!lineBreak)
        sprintf(instString, "%s%s", opcodeID, dBuf);     
      else
	    sprintf(instString, "%s%s\r\n", opcodeID, dBuf);

        
      return;
    }
    else if (opcode == 0x84)
    {
      unsigned short src = operandBuffer[0];
      getAddrFormat(sBuf, src, 0);
      
      sprintf(dBuf, "%s, %i, %i", sBuf, operandBuffer[1], operandBuffer[2]);
    
     if (!lineBreak)
        sprintf(instString, "%s%s", opcodeID, dBuf);     
      else
	    sprintf(instString, "%s%s\r\n", opcodeID, dBuf);
        
      return;
    }
    else if (opcode == 0x85)
    {
      unsigned short src = operandBuffer[0];
      getAddrFormat(sBuf, src, 0);
      
      sprintf(dBuf, "%s, %i, %i, %i, %i", sBuf, operandBuffer[3], operandBuffer[1], operandBuffer[2], operandBuffer[4]);
    
      if (!lineBreak)
        sprintf(instString, "%s%s", opcodeID, dBuf);     
      else
	    sprintf(instString, "%s%s\r\n", opcodeID, dBuf);

        
      return;
    }
    else if (opcode == 0x86)
    {
      sprintf(dBuf, "0x%04x, %i", operandBuffer[0]*4, operandBuffer[1]*4);
    
      if (!lineBreak)
        sprintf(instString, "%s%s", opcodeID, dBuf);     
      else
	    sprintf(instString, "%s%s\r\n", opcodeID, dBuf);
        
      return;
    }
    else if (opcode == 0x88 || opcode == 0x89)
    {
      if (opcode == 0x88)
      {
        if (operandBuffer[3] == 1)
        {
          if (operandBuffer[2] == 0)
            sprintf(opcodeID, "%-8s", "RSRT");
          else if (operandBuffer[2] == 1)
            sprintf(opcodeID, "%-8s", "RSNT");
          else if (operandBuffer[2] == 2)
            sprintf(opcodeID, "%-8s", "RSZT");
          else
            sprintf(opcodeID, "%-8s", "RSTv48");
        }
        else if (operandBuffer[3] == 0 || operandBuffer[3] == 2)
        {
          if (operandBuffer[2] == 0)
            sprintf(opcodeID, "%-8s", "RNRT");
          else if (operandBuffer[2] == 1)
            sprintf(opcodeID, "%-8s", "RNNT");
          else if (operandBuffer[2] == 2)
            sprintf(opcodeID, "%-8s", "RNZT");
          else
            sprintf(opcodeID, "%-8s", "RNTv48");
        }
        else if (operandBuffer[3] == 3)
        {
          if (operandBuffer[2] == 0)
            sprintf(opcodeID, "%-8s", "EFS");
          else if (operandBuffer[2] == 1)
            sprintf(opcodeID, "%-8s", "EFSN");
          else if (operandBuffer[2] == 2)
            sprintf(opcodeID, "%-8s", "EFSZ");
          else
            sprintf(opcodeID, "%-8s", "EFSv48");
        }
      }
      else
      {
        if (operandBuffer[3] == 1)
        {
          if (operandBuffer[2] == 0)
            sprintf(opcodeID, "%-8s", "RSRF");
          else if (operandBuffer[2] == 1)
            sprintf(opcodeID, "%-8s", "RSNF");
          else if (operandBuffer[2] == 2)
            sprintf(opcodeID, "%-8s", "RSZF");
          else
            sprintf(opcodeID, "%-8s", "RSFv48");
        }
        else if (operandBuffer[3] == 0 || operandBuffer[3] == 2)
        {
          if (operandBuffer[2] == 0)
            sprintf(opcodeID, "%-8s", "RNRF");
          else if (operandBuffer[2] == 1)
            sprintf(opcodeID, "%-8s", "RNNF");
          else if (operandBuffer[2] == 2)
            sprintf(opcodeID, "%-8s", "RNZF");
          else
            sprintf(opcodeID, "%-8s", "RNFv48");
        }
        else if (operandBuffer[3] == 3)
        {
          if (operandBuffer[2] == 0)
            sprintf(opcodeID, "%-8s", "EFC");
          else if (operandBuffer[2] == 1)
            sprintf(opcodeID, "%-8s", "EFCN");
          else if (operandBuffer[2] == 2)
            sprintf(opcodeID, "%-8s", "EFCZ");
          else
            sprintf(opcodeID, "%-8s", "EFCv48");
        }
      }
      
      if (operandBuffer[1] != 0x1F)
      {
        int link = (operandBuffer[1]*4) + 0x60;
        sprintf(dBuf, "subID(%i), process[0x%x]", operandBuffer[0], link);
      }
      else
        sprintf(dBuf, "subID(%i), pop()", operandBuffer[0]);

      if (!lineBreak)
        sprintf(instString, "%s%s", opcodeID, dBuf);     
      else
	      sprintf(instString, "%s%s\r\n", opcodeID, dBuf);

        
      return;
    }
    else if (opcode == 0x8A || opcode == 0x91)
    {
      sprintf(dBuf, "%i, %i, %i, %i", operandBuffer[0], operandBuffer[1], operandBuffer[2], operandBuffer[3]);
    
      if (!lineBreak)
        sprintf(instString, "%s%s", opcodeID, dBuf);     
      else
	    sprintf(instString, "%s%s\r\n", opcodeID, dBuf);
        
      return;
    }
    else if (opcode == 0x8D)
    {
      unsigned short src = operandBuffer[0];
      getAddrFormat(sBuf, src, 0);
     
      sprintf(dBuf, "%s, %x, %i, %i, %i", sBuf, operandBuffer[1], operandBuffer[2], operandBuffer[3], operandBuffer[4]);
    
      if (!lineBreak)
        sprintf(instString, "%s%s", opcodeID, dBuf);     
      else
	    sprintf(instString, "%s%s\r\n", opcodeID, dBuf);

        
      return;
    }
    else if (opcode == 0x87 || opcode == 0x8F || opcode == 0x90)
    {
      unsigned short src = operandBuffer[0];
      getAddrFormat(sBuf, src, 0);
     
      sprintf(dBuf, "%s, %i, %i, %i", sBuf, operandBuffer[1], operandBuffer[2], operandBuffer[3]);
    
      if (!lineBreak)
        sprintf(instString, "%s%s", opcodeID, dBuf);     
      else 
	    sprintf(instString, "%s%s\r\n", opcodeID, dBuf);
        
      return;
    }
    else
    {
      unsigned short dst = operandBuffer[0];
      unsigned short src = operandBuffer[1];
       
      getAddrFormat(dBuf, dst, 0);
      getAddrFormat(sBuf, src, 0); 
	 	    
      if (!lineBreak)
        sprintf(instString, "%s %s,%s", opcodeID, sBuf, dBuf);     
      else
        sprintf(instString, "%s %s, %s\r\n", opcodeID, sBuf, dBuf);

      return;	
    }
  }
  else
  {
    if (!lineBreak)
      sprintf(instString, "%s ", opcodeID);
    else
      sprintf(instString, "%s \r\n", opcodeID);
  
    return;
  }

}

void goolDisasm::getOperandFormat(char *instString, bool lineBreak)
{
  return getOperandFormat(instString, opcodeBuffer, operandBuffer, lineBreak);
}

void goolDisasm::getOperandFormat(char *instString, unsigned char opcode, bool lineBreak)
{
  return getOperandFormat(instString, opcode, operandBuffer, lineBreak);
}

void goolDisasm::disassemble(char output[][MAX_CODELINE])
{
  char temp[80];
  int index = 0;
  pc = 0;
  while (!lastInstruction())
  {
    nextInstruction();
    getOpcode();
    getOperands();
    
    getOperandFormat(output[index++], true);
    //strcpy(output[index++], temp);
  }
} 