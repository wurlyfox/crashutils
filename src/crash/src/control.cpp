#include "control.h"

#include "../crash_system.h"

//the following 3 are for psx controller emulation
unsigned long port1;
unsigned long port2;

unsigned long ctrlCount; //0x5E718  
ctrl controls[2];        //0x5E71C

unsigned long ctrlLock;  //0x61A30

//0 = released, 1 = pressed
//0 = L2      0x0001      
//1 = R2      0x0002
//2 = L1      0x0004
//3 = R1      0x0008
//4 = /\      0x0010
//5 = ()      0x0020
//6 = ><      0x0040
//7 = []      0x0080  
//8 = select  0x0100
//9 = L3      0x0200 
//10 = R3     0x0400
//11 = start  0x0800
//12 = up     0x1000
//13 = right  0x2000
//14 = down   0x4000
//15 = left   0x8000
    
void initControls()
{
  ctrlCount = 2;
}  
  
void updateControls()     //reads controller buttons *(0x5E718) times, updating a struct for each read (i.e. to determine an actual button 'press' via                       change in state from released to pressed; one of these structs -should- hold the proper data from the point at which a                            button is pressed, if any, after reading)
{
  if (ctrlCount <= 0) { return; }    //count of controller button structs  
  
  unsigned long count = 0;
  
  do
  {
    ctrl *control = &controls[count];
      
    //a1 = s0[0];
    //a0 = counter;           //counter1

    //s0[0x10] = s0[8];  //remember the controller state said to be 'previous' from the previous state
    //s0[8]    = s0[4];  //remember previous controller state in s0[8] (since the new one will be grabbed in s0[4])
    //s0[0xC]  = s0[0];  //remember the previous 'changed' state 

    control->prevPrevState = control->prevHeldState;
    control->prevTapState  = control->tapState;
    control->prevHeldState = control->heldState;
    
    //BIOS FUNCTION PAD_DR & READ FROM 0X57054 - returned NOR'ed with 0 (reverse bits)
    //this returns the bitfield of currently held controller buttons with
    //controller 1 in the left hword and controller 2 in the right hword
    unsigned long heldState12 = crashSystemControl();           
    
    //we want the held state for the corresponding controller
    unsigned long heldState;
    if (count != 0)    
      heldState = heldState12 & 0xFFFF; //controller 2
    else
      heldState = heldState12 >> 16;    //controller 1

    control->heldState = heldState;
  
    if (control->heldState & 0x1000)    //if up is held         
      control->heldState &= 0xBFFF;     //ignore holding down
  
    if (control->heldState & 0x8000)    //if left is held
      control->heldState &= 0xDFFF;         //ignore holding right

    if (count == 0)
    {
      if (ctrlLock == 2)
      {
        //ctrlObj   = gp[0x308];      //gp = 0x563fc on this sub,  gp[0x308] = 800B07D8, ptr to 00000001 00000003 etc
        //demoFrame = gp[0x2AC];                                   gp[0x2AC] = 0, ptr to a previous controller state
                                      
        //if (demoFrame == (ctrlObj + 0x52C))                           ? maybe when this not 0, it is gp[0x308]+0x52c (ptr to the end of a 52c byte long struct)
          //*(0x58404)[0x84] = ctrlObj[0x528];

        //demoFrame += 8

        //unsigned long oldState = control->heldState;                   //remember the current controller state
        //control->heldState = demoFrame[4];
        
        //if (oldState & 0x9F0 == 0)             //if ><, [], /\, (), start, and select NOT all pressed down [for a soft reset]
        
        //v0 = (v1[0] * 8) + 0x52C + v1; 
        //if (a0 >= v0)
        //{
        //  if (*(619BC))
        //    issueEvent(0, *(0x619BC), 0xE00, 1, {0});    //learn this sub
           
        //  ctrlLock = 3;
        //}
      }
      else
      {
        if (ctrlLock == 3)
          control->heldState = 0;
      }
    }      

    //grabs only bits for buttons not pressed in the previous state (except for L3 & R3)
    control->tapState = ((~control->prevHeldState) & control->heldState) & 0xF9FF;      

    count++;

  } while (count < ctrlCount);                
}

