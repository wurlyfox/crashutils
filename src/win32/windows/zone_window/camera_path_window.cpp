#include "camera_path_window.h"

LRESULT camera_path_window::onCreate(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  RECT rcClient;
  GetClientRect(&rcClient);

  createText(0, 20, 20,  200, 30, "Camera mode: ");
  createText(0, 20, 55,  200, 30, "Camera path point count: ");
  createText(0, 20, 90,  200, 30, "Average distance between consecutive camera path points: ");
  createText(0, 20, 125, 200, 30, "Approximate camera path length: ");
  createText(0, 20, 160, 200, 30, "Index of entrance point: ");
  createText(0, 20, 195, 200, 30, "Index of exit point: ");
  createText(0, 20, 230, 200, 30, "Camera path direction normal: ");
  createText(0, 20, 265, 200, 30, "Camera zoom-back distance: ");
  
  cameraMode    = new drop(IDC_CAMERAMODEDROP, rcClient.right-140, 20, 120, 300, hSelf);
  cameraMode->addItem("Stationary");
  cameraMode->addItem("Auto");
  cameraMode->addItem("unused?");
  cameraMode->addItem("Auto (alt)");
  cameraMode->addItem("unused?");
  cameraMode->addItem("Follow");
  cameraMode->addItem("Follow (alt)");
  cameraMode->addItem("Map?");
  cameraMode->addItem("Or is this map?");
  
  pointCount    = createText(WS_EX_CLIENTEDGE, rcClient.right-140, 55, 120, 30, "");
  pointDist     = new edit(IDC_EDITPOINTDIST, rcClient.right-140, 90, 120, 18, hSelf);
  pointDist->addSpin(IDC_SPINPOINTDIST);
  pathLength    = createText(WS_EX_CLIENTEDGE, rcClient.right-140, 125, 120, 30, "");
  entrancePoint = new edit(IDC_EDITENTRANCEPOINT, rcClient.right-140, 160, 120, 18, hSelf);
  entrancePoint->addSpin(IDC_SPINENTRANCEPOINT);
  exitPoint     = new edit(IDC_EDITEXITPOINT, rcClient.right-140, 195, 120, 18, hSelf);
  exitPoint->addSpin(IDC_SPINEXITPOINT);
  
  raw_group_createparams params = {   3,   1, 
                                    IDC_EDITCAMERAVECTORX, 
                                    180, 230, 60, 20,
                                      2,   2,  2,  2,
                                      0,   0 };
                                    
  raw_control::createGroup(cameraVector, params, hSelf);
  for (int lp=0; lp<3; lp++)
    cameraVector[lp]->addSpin(IDC_SPINCAMERAVECTORX+lp, -100000, 100000);
    
  zoomBack = new edit(IDC_EDITZOOMBACK, rcClient.right-140, 265, 120, 18, hSelf);
  zoomBack->addSpin(IDC_SPINZOOMBACK);
}

LRESULT camera_path_window::onNotify(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  cameraVector[0]->handle(0, lParam);
  cameraVector[1]->handle(0, lParam);
  cameraVector[2]->handle(0, lParam);
}

LRESULT camera_path_window::onCommand(UINT uMsg, WPARAM wParam, LPARAM lParam)
{  
  pointDist->handle(wParam, 0);
  entrancePoint->handle(wParam, 0);
  exitPoint->handle(wParam, 0);
  zoomBack->handle(wParam, 0);
  cameraVector[0]->handle(wParam, 0);
  cameraVector[1]->handle(wParam, 0);
  cameraVector[2]->handle(wParam, 0);
  
  if (LOWORD(wParam) == IDC_EDITPOINTDIST
   && HIWORD(wParam) == EN_CHANGE)
  {
    char tempString[10];
    sprintf(tempString, "%i", section.length*section.avgNodeDist);
    ::SetWindowText(pathLength, tempString);
  }
}

void camera_path_window::onExternal(int msg, param lparam, param rparam)
{
  if (msg == WSM_SELECT_WINDOW_CAMERAPATH) { select(); }
}

void camera_path_window::onSelectSection(unsigned char *sectionData)
{  
  readCrashZoneSection(sectionData, section);
 
  cameraMode->selectItem(section.camMode);
  
  char tempString[10];
  sprintf(tempString, "%i", section.length);
  ::SetWindowText(pointCount, tempString);
  sprintf(tempString, "%i", section.length*section.avgNodeDist);
  ::SetWindowText(pathLength, tempString);
  
  entrancePoint->setSpinRange(0, section.length);
  exitPoint->setSpinRange(0, section.length);
  
  pointDist->useValue((short*)&section.avgNodeDist);
  entrancePoint->useValue((char*)&section.entranceIndex);
  exitPoint->useValue((char*)&section.exitIndex);
  zoomBack->useValue((short*)&section.camZoom);
  cameraVector[0]->useValue((short*)&section.pathDirectionX, 0x1000);
  cameraVector[1]->useValue((short*)&section.pathDirectionY, 0x1000);
  cameraVector[2]->useValue((short*)&section.pathDirectionZ, 0x1000);   
}