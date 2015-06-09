#include "model_window.h"

LRESULT model_window::onCreate(UINT uMsg, WPARAM wParam, LPARAM lParam)
{    
  RECT rcClient;
  GetClientRect(&rcClient);
    
  param params[2];
  params[0] = (param)&mainContext;
  params[1] = 0;
  SendMessageRoot(MSG_GET_CONTEXT, (WPARAM)0, (LPARAM)params);
  addChild(mainContext);
  mainContext->create(WS_EX_CLIENTEDGE, 0, 0, 640, 480, IDC_RENDER);
  
  zone_window *zoneWindow = new zone_window;
  addChild(zoneWindow);
  zoneWindow->create(WS_EX_CLIENTEDGE, 640, 0, 640, rcClient.bottom, IDC_ZONEWINDOW);  
}    
  
void model_window::onLoadLevel()
{
  SetFocus();
}
  
void model_window::onSelectZone(entry *zone)
{
  ShowWindow(true);
  
  char EIDstring[6];
  getEIDstring(EIDstring, zone->EID);
  SetWindowText(EIDstring);
  SetFocus();
}

void model_window::onSelectModel(entry *tgeo, int frame)
{
  ShowWindow(true);
  
  char EIDstring[6];
  getEIDstring(EIDstring, tgeo->EID);
  SetWindowText(EIDstring);
  SetFocus();
}

void model_window::onSelectModel(entry *wgeo)
{
  ShowWindow(true);
  
  char EIDstring[6];
  getEIDstring(EIDstring, wgeo->EID);
  SetWindowText(EIDstring);
  SetFocus();
}