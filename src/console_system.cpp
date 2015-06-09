#include "console_system.h"

#include <iostream>
using namespace std;

void console_system::messageRouter(csystem *src, int msg, param lparam, param rparam)
{
  char *msgString="";
  
  switch (msg)
  {
    case MSG_CREATE                     : msgString = "MSG_CREATE"; break;                     
    case MSG_START                      : msgString = "MSG_START"; break;                    
    //case MSG_HANDLE_KEYS                : msgString = "MSG_HANDLE_KEYS"; break;               

    case MSG_LOAD_NSD                   : msgString = "MSG_LOAD_NSD"; break;              
    case MSG_SAVE_NSD                   : msgString = "MSG_SAVE_NSD"; break;                  

    case MSG_INIT_NSD                   : msgString = "MSG_INIT_NSD"; break;                  

    case MSG_SELECT_ITEM                : msgString = "MSG_SELECT_ITEM"; break;              
    case MSG_EXPORT_ITEM                : msgString = "MSG_EXPORT_ITEM"; break;               

    case MSG_LOOKUP_ENTRY               : msgString = "MSG_LOOKUP_ENTRY"; break;              

    case MSG_SELECT_SVTX                : msgString = "MSG_SELECT_SVTX"; break;               
    case MSG_SELECT_WGEO                : msgString = "MSG_SELECT_WGEO"; break;               
    case MSG_SELECT_ZDAT                : msgString = "MSG_SELECT_ZDAT"; break;               

    case MSG_SELECT_SECTION             : msgString = "MSG_SELECT_SECTION"; break;            
    case MSG_SELECT_ENTITY              : msgString = "MSG_SELECT_ENTITY"; break;             

    case MSG_LOAD_LEVEL                 : msgString = "MSG_LOAD_LEVEL"; break;                

    case MSG_NEW_CONTEXT                : msgString = "MSG_NEW_CONTEXT"; break;               
    case MSG_NEW_SCENE                  : msgString = "MSG_NEW_SCENE"; break;                 
    case MSG_NEW_SCENE_SHARED           : msgString = "MSG_NEW_SCENE_SHARED"; break;          
    case MSG_NEW_TIMER                  : msgString = "MSG_NEW_TIMER"; break;                 
    case MSG_NEW_THREAD                 : msgString = "MSG_NEW_THREAD"; break;                

    case GSM_CREATE                     : msgString = "GSM_CREATE"; break;                    

    //case GSM_RENDER                     : msgString = "GSM_RENDER"; break;                    

    case GSM_GET_CONTEXT                : msgString = "GSM_GET_CONTEXT"; break;               
    case GSM_ALLOW_THREAD               : msgString = "GSM_ALLOW_THREAD"; break;              

    case GSM_CLEAR_MODELS               : msgString = "GSM_CLEAR_MODELS"; break;                              
    case GSM_WLDMODEL                   : msgString = "GSM_WLDMODEL"; break;                  
    case GSM_OBJMODEL                   : msgString = "GSM_OBJMODEL"; break;                  
    case GSM_SPRMODEL                   : msgString = "GSM_SPRMODEL"; break;                  
    case GSM_FRGMODEL                   : msgString = "GSM_FRGMODEL"; break;                  

    case GSM_CAMERA_SYNC_EXTERNAL       : msgString = "GSM_CAMERA_SYNC_EXTERNAL"; break;

    case WSM_CREATE                     : msgString = "WSM_CREATE"; break;                    
    case WSM_SELECT_WINDOW_GENERAL      : msgString = "WSM_SELECT_WINDOW_GENERAL"; break;     
    case WSM_SELECT_WINDOW_SHADER       : msgString = "WSM_SELECT_WINDOW_SHADER"; break;      
    case WSM_SELECT_WINDOW_CAMERAPATH   : msgString = "WSM_SELECT_WINDOW_CAMERAPATH"; break;  
    case WSM_SELECT_WINDOW_ENTITY       : msgString = "WSM_SELECT_WINDOW_ENTITY"; break;                               

    case CSM_CREATE                     : msgString = "CSM_CREATE"; break;                    
    case CSM_INIT                       : msgString = "CSM_INIT"; break;                      
    case CSM_SYNC                       : msgString = "CSM_SYNC"; break;                      

    case CSM_PAGE                       : msgString = "CSM_PAGE"; break;                      

    case CSM_DRAW                       : msgString = "CSM_DRAW"; break;                      
    case CSM_POSTDRAW                   : msgString = "CSM_POSTDRAW"; break;                  

    case CSM_ZONE_WORLD_MODELS          : msgString = "CSM_ZONE_WORLD_MODELS"; break;                    
    case CSM_GFX_OBJMODEL               : msgString = "CSM_GFX_OBJMODEL"; break;              
    case CSM_GFX_SPRMODEL               : msgString = "CSM_GFX_SPRMODEL"; break;              
    case CSM_GFX_FRGMODEL               : msgString = "CSM_GFX_FRGMODEL"; break;              

    case CSM_CONTROL                    : msgString = "CSM_CONTROL"; break;                   
    case CSM_TIME                       : msgString = "CSM_TIME"; break;                      

    case CSM_VIEWMATRIX                 : msgString = "CSM_VIEWMATRIX"; break;               
    case CSM_CAMERA_UPDATE              : msgString = "CSM_CAMERA_UPDATE"; break;             

    case CSM_HANDLE_OBJECTS             : msgString = "CSM_HANDLE_OBJECTS"; break;            

    case CSM_SPAWN_USER_OBJECT          : msgString = "CSM_SPAWN_USER_OBJECT"; break;  
    case CSM_CREATE_USER_PROCESS        : msgString = "CSM_CREATE_USER_PROCESS"; break;
    case CSM_CREATE_USER_EVENT          : msgString = "CSM_CREATE_USER_EVENT"; break;  

    case CPM_INIT                       : msgString = "CPM_INIT"; break;
    case CPM_RENDER                     : msgString = "CPM_RENDER"; break;
    case CPM_HOVER                      : msgString = "CPM_HOVER"; break;
    case CPM_POINT                      : msgString = "CPM_POINT"; break; 
    
    case CPM_RESET                      : msgString = "CPM_RESET"; break;                     
    
    //case CPM_LOAD_LEVEL                 : msgString = "CPM_LOAD_LEVEL"; break;                
    case CPM_LOAD_ZONE                  : msgString = "CPM_LOAD_ZONE"; break;                 
    //case CPM_LOAD_SVTX                  : msgString = "CPM_LOAD_SVTX"; break;                 
    //case CPM_LOAD_WGEO                  : msgString = "CPM_LOAD_WGEO"; break;                 
    case CPM_ADD_ZONE                   : msgString = "CPM_ADD_ZONE"; break;                  

    //case CPM_SELECT_ZONE                : msgString = "CPM_SELECT_ZONE"; break;               
    //case CPM_SELECT_SECTION             : msgString = "CPM_SELECT_SECTION"; break;            
 
    case CPM_CAMERA_SELECT              : msgString = "CPM_CAMERA_SELECT"; break;           
    case CPM_CAMERA_SYNC                : msgString = "CPM_CAMERA_SYNC"; break;

    case CPM_CAMERA_SMOOTH_TRANS        : msgString = "CPM_CAMERA_SMOOTH_TRANS"; break;       

    case CPM_VIEW_TOGGLE_FLAGS          : msgString = "CPM_VIEW_TOGGLE_FLAGS"; break;         

    case CPM_VISUAL_ZONE_TOGGLE_OCTREES : msgString = "CPM_VISUAL_ZONE_TOGGLE_OCTREES"; break;
    case CPM_VISUAL_SECTION_TOGGLE      : msgString = "CPM_VISUAL_SECTION_TOGGLE"; break;
    
    default: return;
  }    
   
  unsigned long ticks = 0;
  if (timer)
    ticks = timer->getElapsedTime(false);
    
  cout << "[" << ticks << "] " << msgString << endl;
}