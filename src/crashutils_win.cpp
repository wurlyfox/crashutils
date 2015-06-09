#include "crashutils_win.h"

void crashutils_win::messageRouter(csystem *src, int msg, param lparam, param rparam)
{
  crashutils::messageRouter(src, msg, lparam, rparam);
  
  switch (msg)
  {
    case MSG_NEW_CONTEXT:
    {
      *((context**)lparam) = (context*)(new context_wgl());
    }
    break;
    
    case MSG_GET_CONTEXT:
    {
      // ugly work-around :(
      postMessage(GSM_GET_CONTEXT, lparam, rparam);
      *((context_win**)lparam) = dynamic_cast<context_win*>(*(context**)lparam);
    }
    break;
    
    case MSG_NEW_SCENE:
    {
      *((scene**)lparam) = (scene*)(new scene_gl(nsd, nsf));
    }
    break;
    
    case MSG_NEW_SCENE_SHARED:
    { 
      scene *sharer = (scene*)rparam;
      
      *((scene**)lparam) = (scene*)(new scene_gl(sharer, SCENE_SCENEFLAGS_SHAREDALLBUTCAM));
    }
    break;
    
    case MSG_NEW_TIMER:
    {
      *((time**)lparam) = (time*)(new time_win());
    }
    break;
    
    case MSG_NEW_THREAD:
    {
      *((thread**)lparam) = (thread*)(new thread_win());
    }
  }
}
    