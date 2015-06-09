#include "entity_window.h"

LRESULT entity_window::onCreate(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  RECT rcClient;
  GetClientRect(&rcClient);
 
  text_type    = createText(0, 20, 20, 200, 30, "Type/executable name: ");
  text_subtype = createText(0, 20, 50, 200, 30, "Subtype/initial state: ");
  text_group   = createText(0, 20, 80, 200, 30, "Group");
  
  drop_type    = new entryDrop(IDC_GOOLDROP, ENTRYDROP_GOOL, rcClient.right-140, 20, 120, 300, hSelf);
  //drop_subtype =      new drop(IDC_STATEDROP, 120, 40, 30, 30, hSelf);
  //drop_group   =      new drop(IDC_GROUPDROP, 120, 70, 30, 30, hSelf);
  
  static_subtype = createText(0, rcClient.right-140, 50, 120, 30, "");
  static_group   = createText(0, rcClient.right-140, 80, 120, 30, "");
}

void entity_window::onExternal(int msg, param lparam, param rparam)
{
  switch (msg)
  {
    case MSG_INIT_NSD: onInitNSD((NSF*)lparam, (NSD*)rparam); break;
    case WSM_SELECT_WINDOW_ENTITY: select(); break;
  }
}

void entity_window::onInitNSD(NSF *nsf, NSD *nsd)
{
  drop_type->setNSFNSD(nsf, nsd);
  drop_type->occupy();    
}

void entity_window::onSelectEntity(unsigned char *entityData)
{  
  readCrashZoneEntity(entityData, entity);
  
  drop_type->selectEntry(entity.type);
  //drop_subtype->selectItem(entity.subtype);
  //drop_group->
  
  char tempString[10];
  sprintf(tempString, "%i", entity.subtype);
  ::SetWindowText(static_subtype, tempString);
  sprintf(tempString, "%x", entity.group);
  ::SetWindowText(static_group, tempString);
}