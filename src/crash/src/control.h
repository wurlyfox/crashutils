#ifndef CONTROL_H
#define CONTROL_H

typedef struct
{
  unsigned long tapState;      //a bit set only at the instant of its button being pressed, clear after
  unsigned long heldState;     //a bit set if corresponding button is held down, clear if not
  unsigned long prevHeldState; //previous read
  unsigned long prevTapState;  
  unsigned long prevPrevState;
} ctrl;  

void initControls();
void updateControls();   

#endif
