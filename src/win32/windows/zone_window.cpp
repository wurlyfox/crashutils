#include "zone_window.h"

LRESULT zone_window::onCreate(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  RECT rcClient;
  GetClientRect(&rcClient);

  // get nsd/nsf
  param params[2];
  params[0] = (param)&nsf;
  params[1] = (param)&nsd;
  SendMessageRoot(MSG_GET_NSD, (WPARAM)0, (LPARAM)params);

  // create menus
  hEntityMenu = CreatePopupMenu();
  AppendMenu(hEntityMenu, MF_STRING, ID_ENTITY_SPAWN, "Spawn Object");
  AppendMenu(hEntityMenu, MF_SEPARATOR, ID_SEPARATOR, "");
  AppendMenu(hEntityMenu, MF_STRING, ID_ENTITY_PROPERTIES, "Properties");

  // create controls

	// zone list
  mainList = new zoneList(IDC_ZONELIST, nsf, nsd, 0, 0, 150, rcClient.bottom, hSelf);

	// overhead context
  context_win *overheadContext;
  params[0] = (param)&overheadContext;
  params[1] = (param)1;
  SendMessageRoot(MSG_GET_CONTEXT, (WPARAM)0, (LPARAM)params);
  addChild(overheadContext);
  overheadContext->create(WS_EX_CLIENTEDGE, 200, 30, 320, 240, IDC_CONTEXT_OVERHEAD);

	// child frame windows

	// general window
  general_window *generalWindow = new general_window;
  addChild(generalWindow);
  generalWindow->create(WS_EX_CLIENTEDGE, 150, 300, 400, rcClient.bottom-150, IDC_GENERALWINDOW);

	// entity window
  entity_window *entityWindow = new entity_window;
  addChild(entityWindow);
  entityWindow->create(WS_EX_CLIENTEDGE, 150, 300, 400, rcClient.bottom-150, IDC_ENTITYWINDOW);

	// shader window
  shader_window *shaderWindow = new shader_window;
  addChild(shaderWindow);
  shaderWindow->create(WS_EX_CLIENTEDGE, 150, 300, 400, rcClient.bottom-150, IDC_ENTITYWINDOW);

	// camera path window
  camera_path_window *cameraPathWindow = new camera_path_window;
  addChild(cameraPathWindow);
  cameraPathWindow->create(WS_EX_CLIENTEDGE, 150, 300, 400, rcClient.bottom-150, IDC_ENTITYWINDOW);

	// hide on init
  generalWindow->ShowWindow(SW_HIDE);
  entityWindow->ShowWindow(SW_HIDE);
  shaderWindow->ShowWindow(SW_HIDE);
  cameraPathWindow->ShowWindow(SW_HIDE);
}

LRESULT zone_window::onNotify(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (int selectType = mainList->handle(lParam))
  {
    int itemIndex = mainList->getSelectedIndex();

    if (selectType == ZONELIST_SELCHANGED)
    {
      if ((itemIndex & 7) == 1)
      {
        int subIndex = (itemIndex & 0xF8) >> 3;

        if (subIndex == 0)
          BroadcastMessageRoot(WSM_SELECT_WINDOW_GENERAL, (WPARAM)0, (LPARAM)0);
        else if (subIndex == 2)
          BroadcastMessageRoot(WSM_SELECT_WINDOW_SHADER, (WPARAM)0, (LPARAM)0);
      }
      else if ((itemIndex & 7) == 2)
      {
        unsigned char *curZoneHeader = currentZone->itemData[0];
        unsigned long neighborEID = getWord(curZoneHeader, 0x214+((itemIndex & 0xF8) >> 1), true);
        unsigned long parentCID   = nsd->lookupCID(neighborEID);
        chunk *parentChunk        = nsf->lookupChunk(parentCID);
        entry *neighbor           = lookupEntry(neighborEID, parentChunk);

        //zoomToNeighbor(neighbor);
      }
      else if ((itemIndex & 7) == 4)
      {
        int sectionIndex = (itemIndex & 0xFF00) >> 8;

        int subIndex = (itemIndex & 0xF8) >> 3;
        if (subIndex == 0)
        {
          unsigned char *section = getCrashZoneSection(currentZone, sectionIndex);

					param params[2];
					params[0] = (param)section;
					params[1] = 0;
					BroadcastMessageRoot(MSG_SELECT_SECTION, (WPARAM)0, (LPARAM)params);
          BroadcastMessageRoot(WSM_SELECT_WINDOW_CAMERAPATH, (WPARAM)0, LPARAM(0));
        }
      }
      else if ((itemIndex & 7) == 5)
      {
        int entityIndex = (itemIndex & 0xFF00) >> 8;
        unsigned char *entity = getCrashZoneEntity(currentZone, entityIndex);

			  param params[2];
				params[0] = (param)entity;
				params[1] = 0;
				BroadcastMessageRoot(MSG_SELECT_ENTITY, (WPARAM)0, (LPARAM)params);
        BroadcastMessageRoot(WSM_SELECT_WINDOW_ENTITY, (WPARAM)0, (LPARAM)0);
      }
    }
    else if (selectType == ZONELIST_RCLICK)
      mouseMenu(hEntityMenu);
  }
}

LRESULT zone_window::onCommand(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch(LOWORD(wParam))
  {
    case ID_ENTITY_SPAWN:
    {
      int itemIndex   = mainList->getSelectedIndex();
      int entityIndex = (itemIndex & 0xFF00) >> 8;

      param paramsA[2];
      paramsA[0] = (param)currentZone;
      paramsA[1] = (param)entityIndex;
      
      param paramsB[2];
      paramsB[0] = (param)paramsA;
      paramsB[1] = (param)0;
      BroadcastMessageRoot(CSM_SPAWN_USER_OBJECT, (WPARAM)0, (LPARAM)paramsB);
    }
  }
}

LRESULT zone_window::onOther(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if ((uMsg >= WM_APP) && (uMsg < 0xC000))
  {
    param *params = (param*)lParam;
    param lparam = lParam ? params[0] : 0;
    param rparam = lParam ? params[1] : 0;

    switch (uMsg)
    {
      case MSG_SELECT_ZONE: onSelectZone((entry*)lparam); break;
    }
  }
}

void zone_window::onSelectZone(entry *zone)
{
  currentZone = zone;

	mainList->clear();
  mainList->occupy(zone);
}