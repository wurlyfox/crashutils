#include "gool_window.h"

LRESULT gool_window::onCreate(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  RECT rcClient;
  GetClientRect(&rcClient);
  
  //create popup menus for execution of events/subroutine (do we even want these anymore?)
  hEventMenu = CreatePopupMenu();
  AppendMenu(hEventMenu, MF_STRING, ID_GOOL_EXECEVENT, "Execute Event");
   
  hSubMenu = CreatePopupMenu();
  AppendMenu(hSubMenu, MF_STRING, ID_GOOL_EXECSUB, "Execute Subroutine"); 
  
  //allocate the gool disassembler
  disassembler = new goolDisasm();
  
  //allocate and create controls
  codeEdit     = new textEdit(IDC_CODEEDIT, 200, 0, 400, rcClient.bottom, hSelf);
  codeData     = new hexEdit(IDC_CODEDATA,  600, 0, 22, 16, 1, 1, 1, 1, hSelf);      
  subIDlist    = new list(IDC_SUBIDLIST, 0, 0, 100, rcClient.bottom/2, hSelf);
  eventIDlist  = new list(IDC_EVENTIDLIST, 0, rcClient.bottom/2, 100, rcClient.bottom/2, hSelf);
  functionList = new propertyList(IDC_FUNCTIONLIST, PROPERTYLIST_DEFAULT, 100, 0, 100, rcClient.bottom, hSelf);
	
  //init the property list  
  functionList->addProperty("Function Name", 0xE, PROPERTY_HWORD, PROPERTY_GOOLOFFSET);
	functionList->addProperty("Head A", 0x0, PROPERTY_WORD, PROPERTY_HEX);
	functionList->addProperty("Head B", 0x4, PROPERTY_WORD, PROPERTY_HEX);
	functionList->addProperty("Index", 0x8, PROPERTY_HWORD, PROPERTY_INT);
	functionList->addProperty("Sub A", 0xA, PROPERTY_HWORD, PROPERTY_GOOLOFFSET);
	functionList->addProperty("Sub B", 0xC, PROPERTY_HWORD, PROPERTY_GOOLOFFSET);
	functionList->setLength(0x10);
	functionList->initColumns();
}
 
LRESULT gool_window::onCommand(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  codeEdit->handle(wParam);
      
  switch(LOWORD(wParam))
  {
    case ID_GOOL_EXECSUB:
    {
    }
    break; 
  }
}

LRESULT gool_window::onNotify(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if (eventIDlist->handle(lParam) == LIST_RCLICK)
  {
  }
  else if (subIDlist->handle(lParam) == LIST_RCLICK)
  {
  }
}

LRESULT gool_window::onPaint(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  codeEdit->paint();
  codeData->paint();
}

LRESULT gool_window::onOther(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
    case WM_VSCROLL:
    {
      codeData->scroll(wParam);
    }
    break;
    
    case WM_CTLCOLOREDIT:
    {
      codeEdit->onCtlColorEdit(lParam);
    }
    break;
    
    default:
    {
      if ((uMsg >= WM_APP) && (uMsg < 0xC000))
      {
        unsigned long *params = (unsigned long*)lParam;
        unsigned long lparam = lParam ? params[0] : 0;
        unsigned long rparam = lParam ? params[1] : 0;
        
        if (uMsg == MSG_SELECT_GOOL) 
          onSelectGool((entry*)lparam);
      }
    }
  }
}
    
void gool_window::onSelectGool(entry *gool)
{
  currentGool = gool;
  
  char EIDstring[6];
  getEIDstring(EIDstring, gool->EID);
  SetWindowText(EIDstring);
  
  unsigned char *item1 = currentGool->itemData[0];
  unsigned char *item2 = currentGool->itemData[1];
  unsigned char *item3 = currentGool->itemData[2];
  unsigned char *item4 = currentGool->itemData[3];
  unsigned char *item5 = currentGool->itemData[4];

  int IDcount = currentGool->itemSize[3]/2;
  
  subIDlist->clear();
  eventIDlist->clear();
  
  unsigned long subIDoffset = getWord(item1, 0x10, true);
  for (int lp=0; lp < subIDoffset; lp++)
    eventIDlist->add("event", getHword(item4, lp*2, true));
    
  for (int lp=0; lp < (IDcount-subIDoffset); lp++)
    subIDlist->add("sub", getHword(item4, (subIDoffset+lp)*2, true));
  
  int functionCount = currentGool->itemSize[4] / 0x10;
  functionList->clear();
  functionList->occupy(item5, 0, functionCount);

  int codeLength = currentGool->itemSize[1];
  codeEdit->clear();

  disassembler->setSource(item2, codeLength);
  disassembler->disassemble(codeEdit->lines);

  codeEdit->occupy(codeLength/4);
  
  SetFocus();

  int dataLength = currentGool->itemSize[2];
  codeData->setSource(item3, dataLength);
  codeData->occupy();
  codeData->invalidate();
}