#include "raw_controls/list.h"
#include "raw_controls/tree.h"

list::list(int listID, int x, int y, int w, int h, HWND parent)
{
  ID = listID;
	
  HINSTANCE hInstance = GetModuleHandle(NULL);
  
  hwnd = CreateWindowEx(0, WC_TREEVIEW, "", 
    WS_CHILD | WS_VISIBLE | WS_BORDER | TVS_HASLINES |  TVS_LINESATROOT, 
    x, y, w, h,
    parent, (HMENU)ID, hInstance, NULL);

  
  if (!hwnd)
  {
    MessageBox(NULL,"Tree Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
    itemCount = -1;
    selectedIndex = -1;
  }
  else
  {
    itemCount = 0;
    selectedIndex = 0;
  }
}

int list::add(const char *header)
{
  char dataString[20];
  sprintf(dataString, "%s %i", header, itemCount+1);
  
  AddItemToTree(hwnd, dataString, 1, 0, itemCount);
  itemCount++;
  
  return 1;
} 

int list::add(const char *header, unsigned long data)
{
  char dataString[20];
  sprintf(dataString, "%s %i: %x", header, itemCount+1, data);
  
  AddItemToTree(hwnd, dataString, 1, 0, itemCount);
  itemCount++;
  
  return 1;
} 

int list::handle(LPARAM lParam)
{
  NMHDR* nmhdr = (NMHDR*)lParam;
  if (nmhdr->idFrom == ID)
  {
    if (nmhdr->code == TVN_SELCHANGED)
    {
      NMTREEVIEW* tree =    (NMTREEVIEW*)lParam;
      selectedIndex    = GetSelectedIndex(tree);
    
      return LIST_SELCHANGED;
    }
    else if (nmhdr->code == NM_RCLICK)
    {
      return LIST_RCLICK;
    }
  }
  
}

unsigned long list::getSelectedItem()
{
  return selectedIndex;
}

void list::clear()
{
  itemCount = 0;
  TreeView_DeleteAllItems(hwnd);
}
