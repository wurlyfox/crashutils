#include "main.h"
#include "control.h"
#include "entryengine.h"

#ifdef CRASH_SYSTEM_INTEGRATE
NSD *nsd;
NSF *nsf;
#endif

void crash_init()
{
  initControls();
  
  //TODO: all other hardware-specific init routines go here when implemented 
}

void crash_main()
{
  #ifdef CRASH_SYSTEM_INTEGRATE
  initSubsystems();  
  #else
  //TODO: implement initPaging
  #endif
  
  //TODO: universal entry load here
  
  do
  { 
    //1) TODO: pause screen stuff here
    
    //2) TODO: crash respawn stuff here
    
    //3) TODO: level transition stuff here
    
    //4) TODO: manage pages here
    
    //5) TODO: auto-object spawn here
    
    //6) camera update
    cameraUpdate();
    
    #ifdef CRASH_SYSTEM_INTEGRATE
    crashSystemCameraUpdate();
    #endif
        
    //7) recalculate view matrices
    #ifdef CRASH_SYSTEM_INTEGRATE
    crashSystemViewMatrix();
    #else
    //TODO: implement sub_80017A14()
    #endif 
  
    //8) transform all wgeo models (' vertices)
    #ifdef CRASH_SYSTEM_INTEGRATE
    //crashSystemZoneWorldModels();
    //we dont create models for the worlds yet, we just use their
    //geometry
    //in future we will need to if we want to implement the
    //shading routines used here
    #else
    //TODO: implement all variations of zone wgeo model gte routines
    #endif
    
    //9) handle objects     
    handleObjects(true);
  
    #ifdef CRASH_SYSTEM_INTEGRATE
    crashSystemHandleObjects();
    #endif
    
    //10) TODO: title screen stuff here
    
    //11) draw
    #ifdef CRASH_SYSTEM_INTEGRATE
    crashSystemDraw();
    #else
    //TODO: implement gpu routine
    #endif
  
  } while (true);
}  