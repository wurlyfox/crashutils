#include "entrylist.h"

entryList::entryList(int listID, int listType, NSF *nsfA, NSD *nsdA, int x, int y, int w, int h, HWND parent)
{
  ID = listID;
	
  HINSTANCE hInstance = GetModuleHandle(NULL);
  
  hwnd = CreateWindowEx(0, WC_TREEVIEW, "", 
    WS_CHILD | WS_VISIBLE | WS_BORDER | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT, 
    x, y, w, h,
    parent, (HMENU)ID, hInstance, NULL);

  if (!hwnd)
  {
    MessageBox(NULL,"Tree Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
    type = ENTRYLIST_NULL;
  }
  else
  {
    nsf  = nsfA;
    nsd  = nsdA;
    type = listType;
    
    rClick = false;
  }
}

int entryList::occupy()
{
  switch (type)
  {
    case ENTRYLIST_NSF:
	  { 
	    if (nsf)
      {
	      char chunkString[0x10] = "";
        for (int curChunk = 0; curChunk < nsf->countChunks; curChunk++)
        { 
          sprintf(chunkString, "Chunk %x\n", nsf->chunks[curChunk].CID);
          AddItemToTree(hwnd, chunkString, 1, 0, curChunk);
			  
	        if (nsf->chunks[curChunk].type == 0)  //TODO: define consts for chunk types
	        {
		        char entryString[0x10] = "";
		        for (int curEntry = 0; curEntry < nsf->chunks[curChunk].entryCount; curEntry++)
            {
              char eidString[6];
              getEIDstring(eidString, nsf->chunks[curChunk].entries[curEntry].EID);
			
              sprintf(entryString, "Entry %i: %s", curEntry+1, eidString); 
              AddItemToTree(hwnd, entryString, 2, curChunk, curEntry);
              
              int itemCount = nsf->chunks[curChunk].entries[curEntry].itemCount;
              if (nsf->chunks[curChunk].entries[curEntry].type == 1)
              {
                char itemString[0x10] = "";
                for (int curItem = 0; curItem < itemCount; curItem++)
                {
                  sprintf(itemString, "Item %i", curItem);
                  AddItemToTree(hwnd, itemString, 3, curChunk, (curItem<<7)|curEntry);
                }
              }
            }				  
          }
        }
      }
      else
      {
        //TODO: error for null nsf
      }      
	  }
	  break;
	
    case ENTRYLIST_NSD:
    {
      if (nsd)
      {
        //for (int curEID = 0; curEID < MAX_LEVELEIDS; curEID++)
        //AddItemToTree(hwnd, nsd->levelEIDstrings[curEID], 1, 0, curEID);
         
        //TODO: implement a better system rather than having to sort through		   
        int numRepeat=0;
        int lastIndex=0;
        int count=0;

        for (int lp=0; lp<MAX_LISTINDICES; lp+=numRepeat)
        {
          int listIndex = nsd->listIndices[lp];

          numRepeat = 1;
          while (listIndex == nsd->listIndices[lp+numRepeat])
            numRepeat++;
    
          if (lp+numRepeat >= MAX_LISTINDICES)
            count = MAX_LISTINDICES - listIndex;  
          else
            count = nsd->listIndices[lp+numRepeat] - listIndex;
      
          char curlistString[20];
          if (numRepeat == 1)
            sprintf(curlistString, "Group %i", lp);
          else
            sprintf(curlistString, "Groups %i-%i", lp, lp+(numRepeat-1));
    
          AddItemToTree(hwnd, curlistString, 1, 0, 0);
          for (int lp2=0; lp2<count; lp2++)
          {
            listEntry curlistEntry = nsd->listEntries[listIndex+lp2];
            unsigned long curCID = curlistEntry.CID;
            unsigned long curEID = curlistEntry.EID;
      
            char curEIDString[6];
            getEIDstring(curEIDString, curEID);
      
            char curlistEntryString[20];
            sprintf(curlistEntryString, "Entry: %s  CID: %x", curEIDString, curCID);
      
          //TODO: implement index, parent, and handle so the nsd lists can also be used
            AddItemToTree(hwnd, curlistEntryString, 2, 0, 0);
          }
        }
      }
    }
	  break;	
  }
}  
	
int entryList::handle(LPARAM lParam)
{
  NMHDR* nmhdr = (NMHDR*)lParam;
	  
  if (nmhdr->idFrom == ID)
  {
	  if (nmhdr->code == TVN_SELCHANGED || nmhdr->code == TVN_SELCHANGING)
	  {
      NMTREEVIEW* tree = (NMTREEVIEW*)lParam;
          
      int level = GetSelectedLevel(tree); 
      
      if (level == 1)
      {
        if (type == ENTRYLIST_NSF)
        {
          selChunkIndex = GetSelectedIndex(tree);
          selectedChunk = nsf->chunks[selChunkIndex];
        }
        else if (type == ENTRYLIST_DEFAULT)
        {
          int parentChunkIndex = GetSelectedParent(tree);
          chunk parentChunk    = nsf->chunks[parentChunkIndex];
          
          selEntryIndex = GetSelectedIndex(tree);
          selectedEntry = parentChunk.entries[selEntryIndex];
        }
      }
      else if (level == 2)
      {
        selChunkIndex = GetSelectedParent(tree);
        selectedChunk = nsf->chunks[selChunkIndex];
          
        selEntryIndex = GetSelectedIndex(tree);
        selectedEntry = selectedChunk.entries[selEntryIndex];
        
        selItemIndex = -1;
        selectedItem = 0;
      }
      else if (level == 3)
      {
        selChunkIndex = GetSelectedParent(tree);
        selectedChunk = nsf->chunks[selChunkIndex];
        
        unsigned short index = GetSelectedIndex(tree);

        selEntryIndex = index & 0x7F;
        selectedEntry = selectedChunk.entries[selEntryIndex];

        selItemIndex = (index >> 7) & 0xFF;
        selectedItem = selectedChunk.entries[selEntryIndex].itemData[selItemIndex];
      }
      
      return ENTRYLIST_SELCHANGED;
	  }
    else if (nmhdr->code == NM_RCLICK)
    {
      HandleRClick(hwnd);
     
      return ENTRYLIST_RCLICK;
    } 
  }
	
  return ENTRYLIST_UNCHANGED;
}  

int entryList::setNSFNSD(NSF *nsfA, NSD *nsdA)
{
  if (nsfA && nsdA)
  {
    nsf = nsfA;
    nsd = nsdA;
  }
  else
    return 0;
	
  return 1;
}

int entryList::lookupAddEntry(unsigned long EID)
{
  char eidString[6];
  getEIDstring(eidString, EID);
  
  if (EID != EID_NONE)
  {
    unsigned long CID = nsd->lookupCID(EID);
    if (CID != -1)
    {
      int chunkIndex = nsf->lookupChunkIndex(CID);
      int entryIndex = nsf->lookupEntryIndex(EID, chunkIndex);
    
      AddItemToTree(hwnd, eidString, 1, chunkIndex, entryIndex);        
    }
    else
      AddItemToTree(hwnd, "non-existant", 1, 0, 0);        
  }
  else
    AddItemToTree(hwnd, eidString, 1, 0, 0);
	
  return 1;
  
  //TODO: what if entry fails to be found
} 

int entryList::addEntry(unsigned long EID)
{
  char eidString[6];
  getEIDstring(eidString, EID);
  AddItemToTree(hwnd, eidString, 1, 0, 0);
	
  return 1;
  
  //TODO: what if entry fails to be found
} 

void entryList::clear()
{
  TreeView_DeleteAllItems(hwnd);
  
  //TODO: clear class variables
}
