#include "nsf_window.h"

LRESULT nsf_window::onCreate(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  RECT rcClient;
  GetClientRect(&rcClient);
  
  // get nsd/nsf
  unsigned long params[2];
  params[0] = (unsigned long)&nsf;
  params[1] = (unsigned long)&nsd;
  SendMessageRoot(MSG_GET_NSD, (WPARAM)0, (LPARAM)params);
  
  mainList   = new entryList(IDC_MAINTREE,       ENTRYLIST_NSF, nsf, nsd,   0, 0, 200, rcClient.bottom, hSelf);
  globalList = new entryList(IDC_GLOBALTREE, ENTRYLIST_DEFAULT, nsf, nsd, 484, 0, 118, rcClient.bottom, hSelf);
  lookupList = new entryList(IDC_LOOKUPTREE,     ENTRYLIST_NSD, nsf, nsd, 602, 0, 236, rcClient.bottom, hSelf);	  

  //484, 354
  hCodeMenu = CreatePopupMenu();
  AppendMenu(hCodeMenu, MF_STRING, ID_GOOL_VIEW, "Open in Code View");
   
  hModelMenuA = CreatePopupMenu();
  AppendMenu(hModelMenuA, MF_STRING, ID_MODEL_VIEW, "Open in Model/Zone View"); 
  AppendMenu(hModelMenuA, MF_SEPARATOR, ID_SEPARATOR, "");
  AppendMenu(hModelMenuA, MF_STRING, ID_MODEL_EXPORT_AS_COLLADA, "Export as COLLADA");    
      
  hModelMenuB = CreatePopupMenu();
  AppendMenu(hModelMenuB, MF_STRING, ID_MODEL_EXPORT_AS_MD3, "Export as .MD3");
  
  CreateWindowEx(0, WC_STATIC, "",
    WS_CHILD | WS_VISIBLE | SS_ETCHEDFRAME,
    210, 10, 250, 110,
    hSelf, NULL, hInstance, NULL);

  CreateWindowEx(0, WC_STATIC, "Type: ",
    WS_CHILD | WS_VISIBLE | SS_CENTER,
    220, 25, 110, 22,
    hSelf, NULL, hInstance, NULL);

  CreateWindowEx(0, WC_STATIC, "Entry Count: ",
    WS_CHILD | WS_VISIBLE | SS_CENTER,
    220, 55, 110, 22,
    hSelf, NULL, hInstance, NULL);

  CreateWindowEx(0, WC_STATIC, "Checksum: ",
    WS_CHILD | WS_VISIBLE | SS_CENTER,
    220, 85, 110, 22,
    hSelf, NULL, hInstance, NULL);

  CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", 
    WS_CHILD | WS_VISIBLE, 
    320, 25, 100, 22, 
    hSelf, (HMENU)IDC_CHUNKTYPEEDIT, hInstance, NULL);

  CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", 
    WS_CHILD | WS_VISIBLE, 
    320, 55, 100, 22, 
    hSelf, (HMENU)IDC_CHUNKENTRYCOUNTEDIT, hInstance, NULL);

  CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", 
    WS_CHILD | WS_VISIBLE, 
    320, 85, 100, 22, 
    hSelf, (HMENU)IDC_CHUNKCHECKSUMEDIT, hInstance, NULL);

  CreateWindowEx(0, WC_STATIC, "",
    WS_CHILD | WS_VISIBLE | SS_ETCHEDFRAME,
    210, 130, 250, 110,
    hSelf, NULL, hInstance, NULL);

  CreateWindowEx(0, WC_STATIC, "Type: ",
    WS_CHILD | WS_VISIBLE | SS_CENTER,
    220, 145, 110, 22,
    hSelf, NULL, hInstance, NULL);

  CreateWindowEx(0, WC_STATIC, "Item Count: ",
    WS_CHILD | WS_VISIBLE | SS_CENTER,
    220, 175, 110, 22,
    hSelf, NULL, hInstance, NULL);

  CreateWindowEx(0, WC_STATIC, "n/a: ",
    WS_CHILD | WS_VISIBLE | SS_CENTER,
    220, 205, 110, 22,
    hSelf, NULL, hInstance, NULL);

  CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", 
    WS_CHILD | WS_VISIBLE, 
    320, 145, 100, 22, 
    hSelf, (HMENU)IDC_ENTRYTYPEEDIT, hInstance, NULL);

  CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", 
    WS_CHILD | WS_VISIBLE, 
    320, 175, 100, 22, 
    hSelf, (HMENU)IDC_ENTRYITEMCOUNTEDIT, hInstance, NULL);

  CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", 
    WS_CHILD | WS_VISIBLE, 
    320, 205, 100, 22, 
    hSelf, (HMENU)IDC_UNUSEDEDIT, hInstance, NULL);
    
  mainList->occupy();
  lookupList->occupy();
		  
  for (int lp=0; lp<MAX_LEVELEIDS; lp++)
    globalList->lookupAddEntry(nsd->levelEIDs[lp]);
}

LRESULT nsf_window::onCommand(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (LOWORD(wParam))
  {
    case ID_MODEL_VIEW:
    case ID_GOOL_VIEW:
    {
      unsigned long params[2];
      params[0] = (unsigned long)&mainList->selectedEntry;
      params[1] = (unsigned long)mainList->selItemIndex;
      
      SendMessageRoot(MSG_SELECT_ITEM, 0, (LPARAM)params);
    }
    break;
    
    case ID_MODEL_EXPORT_AS_MD3:
    case ID_MODEL_EXPORT_AS_COLLADA:
    {
      char filename[MAX_PATH] = "";
      if ((LOWORD(wParam) == ID_MODEL_EXPORT_AS_MD3) && (mainList->selectedEntry.type == 1))
        FileOpenBox("MD3 Files (*.md3)\0*.md3\0All Files (*.*)\0*.*\0", filename, true);
      else if ((LOWORD(wParam) == ID_MODEL_EXPORT_AS_COLLADA) && (mainList->selectedEntry.type == 3))
        FileOpenBox("DAE Files (*.dae)\0*.dae\0All Files (*.*)\0*.*\0", filename, true);
      
      if (filename[0] != 0)
      {
        unsigned long params[2];
        params[0] = (unsigned long)&mainList->selectedEntry;
        params[1] = (unsigned long)&filename;
         
        SendMessageRoot(MSG_EXPORT_ITEM, 0, (LPARAM)params);        
      }
    }
    break;
  }
}
         
LRESULT nsf_window::onNotify(UINT uMsg, WPARAM wParam, LPARAM lParam)
{     
  int result = mainList->handle(lParam);
  if (result == ENTRYLIST_SELCHANGED)
  {
    if (mainList->selectedChunk.type == 1)
    {
      //SendMessage(txtrhWnd, WM_ACTIVATE, 0, 0);
    }
    else
    {          
      char temp[10];
      sprintf(temp, "%i", mainList->selectedChunk.type);
      SetDlgItemText(hSelf, IDC_CHUNKTYPEEDIT, temp);
        
      sprintf(temp, "%i", mainList->selectedChunk.entryCount);
      SetDlgItemText(hSelf, IDC_CHUNKENTRYCOUNTEDIT, temp);
        
      sprintf(temp, "%x", mainList->selectedChunk.checksum);
      SetDlgItemText(hSelf, IDC_CHUNKCHECKSUMEDIT, temp);
          
      sprintf(temp, "%i", mainList->selectedEntry.type);
      SetDlgItemText(hSelf, IDC_ENTRYTYPEEDIT, temp);
        
      sprintf(temp, "%i", mainList->selectedEntry.itemCount);
      SetDlgItemText(hSelf, IDC_ENTRYITEMCOUNTEDIT, temp);
    }
  }
  else if (result == ENTRYLIST_RCLICK)
  {
    if ((mainList->selectedEntry.type == 1 && mainList->selectedItem != 0) ||
         mainList->selectedEntry.type == 3 ||
         mainList->selectedEntry.type == 7)
    {
      long mouse = GetMessagePos();
      short mouseX, mouseY;
      mouseX = GET_X_LPARAM(mouse);
      mouseY = GET_Y_LPARAM(mouse);
    
      TrackPopupMenu(hModelMenuA, TPM_BOTTOMALIGN | TPM_LEFTALIGN, mouseX, mouseY, 0, hSelf, NULL);
    }
    else if (mainList->selectedEntry.type == 1)
    {
      long mouse = GetMessagePos();
      short mouseX, mouseY;
      mouseX = GET_X_LPARAM(mouse);
      mouseY = GET_Y_LPARAM(mouse);
    
      TrackPopupMenu(hModelMenuB, TPM_BOTTOMALIGN | TPM_LEFTALIGN, mouseX, mouseY, 0, hSelf, NULL);
    }
    else if (mainList->selectedEntry.type == 11)
    {
      long mouse = GetMessagePos();
      short mouseX, mouseY;
      mouseX = GET_X_LPARAM(mouse);
      mouseY = GET_Y_LPARAM(mouse);
      
      TrackPopupMenu(hCodeMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, mouseX, mouseY, 0, hSelf, NULL);
    }
  }
}