#ifndef MESSAGE_H
#define MESSAGE_H

#define MSG_CREATE                      0x8000 //unused
#define MSG_START                       0x8001
#define MSG_HANDLE_KEYS                 0x8009
                                        
#define MSG_LOAD_NSD                    0x8011
#define MSG_SAVE_NSD                    0x8012
                                        
#define MSG_INIT_NSD                    0x8020
#define MSG_GET_NSD                     0x8021
                            
#define MSG_SELECT_ITEM                 0x8030
#define MSG_EXPORT_ITEM                 0x8031
                                        
#define MSG_LOOKUP_ENTRY                0x8040
                                        
#define MSG_SELECT_SVTX                 0x8051
#define MSG_SELECT_WGEO                 0x8053
#define MSG_SELECT_ZDAT                 0x8057
#define MSG_SELECT_ZONE MSG_SELECT_ZDAT
#define MSG_SELECT_GOOL                 0x805B

#define MSG_SELECT_SECTION              0x8060
#define MSG_SELECT_ENTITY               0x8061
                                        
#define MSG_LOAD_LEVEL                  0x8070
                                        
#define MSG_NEW_CONTEXT                 0x8101
#define MSG_NEW_SCENE                   0x8102
#define MSG_NEW_SCENE_SHARED            0x8103
#define MSG_NEW_TIMER                   0x8104
#define MSG_NEW_THREAD                  0x8105

#define MSG_GET_CONTEXT                 0x8110
                                        
#define GSM_CREATE                      0x8200
                                        
#define GSM_RENDER                      0x8210
#define GSM_INVALIDATE                  0x8211
                                        
#define GSM_GET_CONTEXT                 0x8220
#define GSM_ALLOW_THREAD                0x8221 //todo
                                        
#define GSM_CLEAR_MODELS                0x8230                              
#define GSM_WLDMODEL                    0x8231
#define GSM_OBJMODEL                    0x8232
#define GSM_SPRMODEL                    0x8233
#define GSM_FRGMODEL                    0x8234

#define GSM_CAMERA_SYNC_EXTERNAL        0x8240
                                        
#define WSM_CREATE                      0x8300
#define WSM_SELECT_WINDOW_GENERAL       0x8320
#define WSM_SELECT_WINDOW_SHADER        0x8321
#define WSM_SELECT_WINDOW_CAMERAPATH    0x8322
#define WSM_SELECT_WINDOW_ENTITY        0x8323                                       
                                        
#define CSM_CREATE                      0x8400
#define CSM_INIT                        0x8401
#define CSM_SYNC                        0x8402
                                        
#define CSM_PAGE                        0x8410
                                        
#define CSM_DRAW                        0x8420
#define CSM_POSTDRAW                    0x8421
                                        
#define CSM_ZONE_WORLD_MODELS           0X8430                         
#define CSM_GFX_OBJMODEL                0x8431
#define CSM_GFX_SPRMODEL                0x8432
#define CSM_GFX_FRGMODEL                0x8433
                                        
#define CSM_CONTROL                     0x8440
#define CSM_TIME                        0x8441
                                        
#define CSM_VIEWMATRIX                  0x8450
#define CSM_CAMERA_UPDATE               0x8451
                                        
#define CSM_HANDLE_OBJECTS              0x8460

#define CSM_CONTROL_TOGGLE_ACTIVE       0x84A0

#define CSM_SPAWN_USER_OBJECT           0x84B0
#define CSM_CREATE_USER_PROCESS         0x84B1
#define CSM_CREATE_USER_EVENT           0x84B2

#define CPM_INIT                        0x9001
#define CPM_RENDER                      0x9002
#define CPM_HOVER                       0x9003
#define CPM_POINT                       0x9004
                        
#define CPM_RESET                       0x9100
                  
#define CPM_LOAD_LEVEL                  MSG_LOAD_LEVEL                 
#define CPM_LOAD_ZONE                   0x9110
#define CPM_LOAD_SVTX                   MSG_SELECT_SVTX
#define CPM_LOAD_WGEO                   MSG_SELECT_WGEO
#define CPM_ADD_ZONE                    0x9111

#define CPM_SELECT_ZONE                 MSG_SELECT_ZONE
#define CPM_SELECT_SECTION              MSG_SELECT_SECTION
                                        
#define CPM_CAMERA_SELECT               0x9200
#define CPM_CAMERA_SYNC                 0x9201

#define CPM_CAMERA_SMOOTH_TRANS         0x9210

#define CPM_VIEW_TOGGLE_FLAGS           0x9300

#define CPM_VISUAL_WIREFRAME_TOGGLE     0x9400
#define CPM_VISUAL_ZONE_TOGGLE_OCTREES  0x9410
#define CPM_VISUAL_SECTION_TOGGLE       0x9420

#endif
