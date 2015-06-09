#include "zonelist.h"

zoneList::zoneList(int listID, NSF *_nsf, NSD *_nsd, int x, int y, int w, int h, HWND parent)
{
  ID = listID;
	
  HINSTANCE hInstance = GetModuleHandle(NULL);
  
  hwnd = CreateWindowEx(0, WC_TREEVIEW, "", 
    WS_CHILD | WS_VISIBLE | WS_BORDER | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT, 
    x, y, w, h,
    parent, (HMENU)ID, hInstance, NULL);

  if (!hwnd)
    MessageBox(NULL,"Tree Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
  else
  {
    nsf = _nsf;
    nsd = _nsd;
    
    rClick = false;
  }
}

void zoneList::clear()
{
  TreeView_DeleteAllItems(hwnd);
  selectedIndex = -1;
}

//00000000 00000000 IIIIIIII SSSSSTTT
//T = 0: static
//T = 1: zone            S = 0: general      S = 1: light    S = 2: shading  S = 3: collision
//T = 2: neighbor zone   S = 0: selected
//T = 3: model           S = 0: selected
//T = 4: section         S = 0: camera       S = 1: neighbor
//T = 5: entity          S = 0: selected
//T = 6: texture page 
//T = 7: texture chunk
//
//I = index

void zoneList::occupy(entry *zone)
{
  unsigned char *zoneHeader = zone->itemData[0];
  modelCount = getWord(zoneHeader, 0, true);  
  headerColCount = getWord(zoneHeader, 0x204, true);
  sectionCount = getWord(zoneHeader, 0x208, true);
  entityCount = getWord(zoneHeader, 0x20C, true);
  neighborCount = getWord(zoneHeader, 0x210, true);
  
  char zoneModelString[8][30];
  for (int lp=0; lp<modelCount; lp++)
  {
    unsigned long modelEID = getWord(zoneHeader, 4+(lp*0x40), true);
    getEIDstring(zoneModelString[lp], modelEID);
    
    unsigned long parentCID = nsd->lookupCID(modelEID);
    if (parentCID != -1)
    {
      chunk *parentChunk      = nsf->lookupChunk(parentCID);
      zoneModel[lp]           = lookupEntry(modelEID, parentChunk);
    }
    else
    {
      char temp[6];
      getEIDstring(temp, modelEID);
      sprintf(zoneModelString[lp], "BAD REF: %s", temp);
      
      zoneModel[lp] = 0;
    }
  }
  
  char zoneEntityString[20][10];
  for (int lp=0; lp<entityCount; lp++)
  {
    unsigned char *entity = zone->itemData[headerColCount+sectionCount+lp];
    
    unsigned char entityID = getHword(entity, 0x8, true);
    
    unsigned char codeIndex = entity[0x12];
    unsigned long entityCodeEID = nsd->levelEIDs[codeIndex];
    char entityCodeEIDString[5];
    getEIDstring(entityCodeEIDString, entityCodeEID);
    
    sprintf(zoneEntityString[lp], "%i: %s", entityID, entityCodeEIDString);
  }
  
  char zoneNeighborString[20][10];
  for (int lp=0; lp<neighborCount; lp++)
  {
    unsigned long neighborEID = getWord(zoneHeader, 0x214+(lp*4), true);
    getEIDstring(zoneNeighborString[lp], neighborEID);
    
    unsigned long parentCID = nsd->lookupCID(neighborEID);
    if (parentCID != -1)
    {
      chunk *parentChunk      = nsf->lookupChunk(parentCID);
      zoneNeighbor[lp]        = lookupEntry(neighborEID, parentChunk);
    }
    else
    {
      char temp[6];
      getEIDstring(temp, neighborEID);
      sprintf(zoneNeighborString[lp], "BAD REF: %s", temp);
      
      zoneNeighbor[lp] = 0;
    }
  }
  
  char zoneEIDString[6];
  getEIDstring(zoneEIDString, zone->EID);
  
  char zoneString[20];
  sprintf(zoneString, "Zone: %s", zoneEIDString);
  
  AddItemToTree(hwnd, zoneString, 1, 1);
  
  AddItemToTree(hwnd, "Models", 2, 0);
  for (int lp=0; lp<modelCount; lp++)    
    AddItemToTree(hwnd, zoneModelString[lp], 3, (lp << 8) | 3);
  
  AddItemToTree(hwnd, "Entities", 2, 0);
  for (int lp=0; lp<entityCount; lp++)  
    AddItemToTree(hwnd, zoneEntityString[lp], 3, (lp << 8) | 5);
  
  AddItemToTree(hwnd, "Sections", 2, 0);
  for (int lp=0; lp<sectionCount; lp++)
  {
    char sectionString[20];
    sprintf(sectionString, "Section %i", lp);
    AddItemToTree(hwnd, sectionString, 3, 0);
    
    AddItemToTree(hwnd,            "Camera", 4, (lp << 8) | (0 << 3) | 4);
    AddItemToTree(hwnd, "Neighbor Sections", 4, (lp << 8) | (1 << 3) | 4);
  }
  
  AddItemToTree(hwnd, "Neighbor Zones", 2, 0);
  for (int lp=0; lp<neighborCount; lp++)
  {
    AddItemToTree(hwnd, zoneNeighborString[lp], 3, (lp << 8) | 2);
  }
  
  AddItemToTree(hwnd,          "Lighting properties", 2, (1 << 3) | 1);
  AddItemToTree(hwnd, "Shading/rendering properties", 2, (2 << 3) | 1); 
  AddItemToTree(hwnd,             "Collision octree", 2, (3 << 3) | 1);
}

int zoneList::handle(LPARAM lParam)
{
  NMHDR* nmhdr = (NMHDR*)lParam;
	  
  if (nmhdr->idFrom == ID)
  {
	  if (nmhdr->code == TVN_SELCHANGED || nmhdr->code == TVN_SELCHANGING)
	  {
      NMTREEVIEW* tree = (NMTREEVIEW*)lParam;
      selectedIndex    = GetSelectedItemData(tree);
      
      return ZONELIST_SELCHANGED;
    }
    else if (nmhdr->code == NM_RCLICK)
    {
      HandleRClick(hwnd);

      return ZONELIST_RCLICK;
    }
  }

  return ZONELIST_UNCHANGED;
}  
   
   
