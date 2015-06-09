#include "game.h"

extern unsigned short levelStates[COUNT_GAMEOBJECTS];
extern unsigned long globals[0x100];

void game_init()
{
  // initial game progress value; 0x1F = full completion, i.e. 31+1 = 32 levels
  globals[0x14] = 0x63;
  
  // ???
  globals[0x13] = -1;
  
  // initial title sequence state
  globals[0x12] = 7;  //sony computer entertainment america
  
  // ???
  globals[0x22] = 0xFF;
  globals[0x23] = 0xFF;
  
  // ???
  globals[0x1F] = 0x400;
  
  // ???
  globals[0x15] = 0;
  
  // ???
  globals[0x20] = 0;
  globals[0x21] = 0;
  
  // ?? checked by boxes and fruit
  globals[0x2E] = 1;

  // ??
  globals[0x6D] = 0;
  
  // ??
  globals[0x73] = 0;
  
  // camera mode?
  globals[0x11] = 0;
  
  // used by DispC...
  globals[0x64] = 0;
  
  // ??
  globals[0x65] = 0;
  
  // ??
  globals[0x71] = 1;
  
  game_reset(true);
}

void game_reset(bool flag)
{
  if (flag)
  {
    // PID of most recently hit checkpoint box
    globals[0x45] = -1;
    
    // ???
    globals[0x6C] = 0;
    
    // level reload count?
    globals[0x5] = 0;
    
    // ???
    globals[0x19] = 0;
    globals[0x1A] = 0;
    globals[0x1B] = 0;
    globals[0x1C] = 0;
    globals[0x1D] = 0;
    
    // maximum initial Z position for entities spawned during title sequence?
    globals[0x2F] = 1;
    
    // number of gems collected
    globals[0x3F] = 0;
    
    // read by GemsC, BoxsC, and FruiC
    globals[0x48] = 0;
    
    // cleared when a new zone is loaded
    globals[0x43] = 1;
    
    // reset game progress to initial progress
    globals[0x14] = 0x63;
    
    // checked by BoxsC and FruiC
    globals[0x2E] = 1;
    
    // used by DispC...
    globals[0x64] = 0;
    
    // ???
    globals[0x65] = 0;
    
    // ???
    globals[0x71] = 1;
    
    // lives/fruit counter?
    globals[0x18] = 0;
    
    // clear level states
    memset(levelStates, 0, 512);
    
    // load useless inventory string here?
    //initInventory();
  }
}  