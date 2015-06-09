#ifndef GOOLDISASM_H
#define GOOLDISASM_H

#include "crash/r3000a.h"

#define MAX_OPERANDLIST 10
#define MAX_CODELINE 80

class goolDisasm
{
  private:
  unsigned char *buffer;
  int buffersize;
  int pc;
  long instructionBuffer;
  unsigned char opcodeBuffer;

  long operandBuffer[MAX_OPERANDLIST];
  
  public:
  goolDisasm();
  goolDisasm(unsigned char *code, int length);
  void setSource(unsigned char *code, int length);
  
  bool lastInstruction();
  long nextInstruction();
  long prevInstruction();
  long getInstruction(int offset);
  
  unsigned char getOpcode();
  unsigned char getOpcode(long instr);
  unsigned char getOpcode(int offset);
  
  long *getOperands();
  long *getOperands(long instr, unsigned char opcode);
  long *getOperands(long instr);
  long *getOperands(int offset);
  
  const char *getOpcodeID(unsigned char opcode);
  void getAddrFormat(char *sBuf, unsigned short addr, int mode);
  
  void getOperandFormat(char *instString, unsigned char opcode, long *operands, bool lineBreak);
  void getOperandFormat(char *instString, unsigned char opcode, bool lineBreak);
  void getOperandFormat(char *instString, bool lineBreak);
  
  void disassemble(char output[][MAX_CODELINE]);
};

#endif