#include "raw_controls/tree.h"

HTREEITEM AddItemToTree(HWND hwndTV, LPSTR lpszItem, int nLevel, int pIndex, int nIndex)
{ 
  TVITEM tvi; 
  TVINSERTSTRUCT tvins; 
  static HTREEITEM hPrev = (HTREEITEM)TVI_FIRST; 
  static HTREEITEM hPrevRootItem = NULL; 
  static HTREEITEM hPrevLev2Item = NULL; 
  HTREEITEM hti; 

  tvi.mask = TVIF_TEXT /*| TVIF_IMAGE | TVIF_SELECTEDIMAGE */| TVIF_PARAM; 
  if (nLevel != 2)
  {
    tvi.mask |= TVIF_STATE;
    tvi.state = TVIS_EXPANDED; tvi.stateMask = TVIS_EXPANDED;
  }
  
  // Set the text of the item. 
  tvi.pszText = lpszItem; 
  tvi.cchTextMax = sizeof(tvi.pszText)/sizeof(tvi.pszText[0]); 

  // Assume the item is not a parent item, so give it a 
  // document image. 
  //tvi.iImage = g_nDocument; 
  //tvi.iSelectedImage = g_nDocument; 

  // Save the heading level, parent index, and index in the item's application-defined 
  // data area. 
  
  
  tvi.lParam = (LPARAM)((nLevel << 30) | ((pIndex & 0x7FFF) << 15) | (nIndex & 0x7FFF));
  
  tvins.item = tvi; 
  tvins.hInsertAfter = hPrev; 

  // Set the parent item based on the specified level. 
  if (nLevel == 1) 
    tvins.hParent = TVI_ROOT; 
  else if (nLevel == 2) 
    tvins.hParent = hPrevRootItem; 
  else 
    tvins.hParent = hPrevLev2Item; 

  // Add the item to the tree-view control. 
  hPrev = (HTREEITEM)SendMessage(hwndTV, TVM_INSERTITEM, 
    0, (LPARAM)(LPTVINSERTSTRUCT)&tvins); 

  if (hPrev == NULL)
    return NULL;

  // Save the handle to the item. 
  if (nLevel == 1) 
    hPrevRootItem = hPrev; 
  else if (nLevel == 2) 
    hPrevLev2Item = hPrev; 

  // The new item is a child item. Give the parent item a 
  // closed folder bitmap to indicate it now has child items. 
  if (nLevel > 1)
  { 
    hti = TreeView_GetParent(hwndTV, hPrev); 
    tvi.mask = 0; //TVIF_IMAGE | TVIF_SELECTEDIMAGE; 
    tvi.hItem = hti; 
    //tvi.iImage = g_nClosed; 
    //tvi.iSelectedImage = g_nClosed; 
    TreeView_SetItem(hwndTV, &tvi); 
  } 

  return hPrev; 
} 

HTREEITEM AddItemToTree(HWND hwndTV, LPSTR lpszItem, int nLevel, void *item)
{ 
  TVITEM tvi; 
  TVINSERTSTRUCT tvins; 
  static HTREEITEM hPrev = (HTREEITEM)TVI_FIRST; 
  static HTREEITEM hPrevRootItem = NULL; 
  static HTREEITEM hPrevLev2Item = NULL; 
  HTREEITEM hti; 

  tvi.mask = TVIF_TEXT /*| TVIF_IMAGE | TVIF_SELECTEDIMAGE */| TVIF_PARAM | TVIF_STATE; 
  tvi.state = TVIS_EXPANDED; tvi.stateMask = TVIS_EXPANDED;

  // Set the text of the item. 
  tvi.pszText = lpszItem; 
  tvi.cchTextMax = sizeof(tvi.pszText)/sizeof(tvi.pszText[0]); 

  // Assume the item is not a parent item, so give it a 
  // document image. 
  //tvi.iImage = g_nDocument; 
  //tvi.iSelectedImage = g_nDocument; 

  tvi.lParam = (LPARAM)(item);
  
  tvins.item = tvi; 
  tvins.hInsertAfter = hPrev; 

  // Set the parent item based on the specified level. 
  if (nLevel == 1) 
    tvins.hParent = TVI_ROOT; 
  else if (nLevel == 2) 
    tvins.hParent = hPrevRootItem; 
  else 
    tvins.hParent = hPrevLev2Item; 

  // Add the item to the tree-view control. 
  hPrev = (HTREEITEM)SendMessage(hwndTV, TVM_INSERTITEM, 
    0, (LPARAM)(LPTVINSERTSTRUCT)&tvins); 

  if (hPrev == NULL)
    return NULL;

  // Save the handle to the item. 
  if (nLevel == 1) 
    hPrevRootItem = hPrev; 
  else if (nLevel == 2) 
    hPrevLev2Item = hPrev; 

  // The new item is a child item. Give the parent item a 
  // closed folder bitmap to indicate it now has child items. 
  if (nLevel > 1)
  { 
    hti = TreeView_GetParent(hwndTV, hPrev); 
    tvi.mask = 0; //TVIF_IMAGE | TVIF_SELECTEDIMAGE; 
    tvi.hItem = hti; 
    //tvi.iImage = g_nClosed; 
    //tvi.iSelectedImage = g_nClosed; 
    TreeView_SetItem(hwndTV, &tvi); 
  } 

  return hPrev; 
} 

HTREEITEM AddItemToTreeAfter(HWND hwndTV, LPSTR lpszItem, int nLevel, void *item, HTREEITEM hParent, HTREEITEM hPrev)
{ 
  TVITEM tvi; 
  TVINSERTSTRUCT tvins; 
  //static HTREEITEM hPrev = (HTREEITEM)TVI_FIRST; 
  static HTREEITEM hPrevRootItem = NULL; 
  static HTREEITEM hPrevLev2Item = NULL; 
  HTREEITEM hti; 

  tvi.mask = TVIF_TEXT /*| TVIF_IMAGE | TVIF_SELECTEDIMAGE */| TVIF_PARAM | TVIF_STATE; 
  tvi.state = TVIS_EXPANDED; tvi.stateMask = TVIS_EXPANDED;
  
  // Set the text of the item. 
  tvi.pszText = lpszItem; 
  tvi.cchTextMax = sizeof(tvi.pszText)/sizeof(tvi.pszText[0]); 

  // Assume the item is not a parent item, so give it a 
  // document image. 
  //tvi.iImage = g_nDocument; 
  //tvi.iSelectedImage = g_nDocument; 

  tvi.lParam = (LPARAM)(item);
  
  tvins.item = tvi; 
  tvins.hInsertAfter = hPrev; 

  // Set the parent item based on the specified level. 
  //if (nLevel == 1) 
    tvins.hParent = hParent; //TVI_ROOT; 
  //else if (nLevel == 2) 
  //  tvins.hParent = ; 
  //else 
  //  tvins.hParent = hPrevLev2Item; 

  // Add the item to the tree-view control. 
  hPrev = (HTREEITEM)SendMessage(hwndTV, TVM_INSERTITEM, 
    0, (LPARAM)(LPTVINSERTSTRUCT)&tvins); 

  if (hPrev == NULL)
    return NULL;

  // Save the handle to the item. 
  if (nLevel == 1) 
    hPrevRootItem = hPrev; 
  else if (nLevel == 2) 
    hPrevLev2Item = hPrev; 

  // The new item is a child item. Give the parent item a 
  // closed folder bitmap to indicate it now has child items. 
  if (nLevel > 1)
  { 
    hti = TreeView_GetParent(hwndTV, hPrev); 
    tvi.mask = 0; //TVIF_IMAGE | TVIF_SELECTEDIMAGE; 
    tvi.hItem = hti; 
    //tvi.iImage = g_nClosed; 
    //tvi.iSelectedImage = g_nClosed; 
    TreeView_SetItem(hwndTV, &tvi); 
  } 

  return hPrev; 
} 

HTREEITEM AddItemToTree(HWND hwndTV, LPSTR lpszItem, int nLevel, int data)
{ 
  TVITEM tvi; 
  TVINSERTSTRUCT tvins; 
  static HTREEITEM hPrev = (HTREEITEM)TVI_FIRST; 
  static HTREEITEM hPrevRootItem = NULL; 
  static HTREEITEM hPrevLev2Item = NULL; 
  static HTREEITEM hPrevLev3Item = NULL;
  HTREEITEM hti; 

  tvi.mask = TVIF_TEXT /*| TVIF_IMAGE | TVIF_SELECTEDIMAGE */| TVIF_PARAM | TVIF_STATE; 
  tvi.state = TVIS_EXPANDED; tvi.stateMask = TVIS_EXPANDED;
  
  // Set the text of the item. 
  tvi.pszText = lpszItem; 
  tvi.cchTextMax = sizeof(tvi.pszText)/sizeof(tvi.pszText[0]); 

  // Assume the item is not a parent item, so give it a 
  // document image. 
  //tvi.iImage = g_nDocument; 
  //tvi.iSelectedImage = g_nDocument; 

  tvi.lParam = (LPARAM)(data);
  
  tvins.item = tvi; 
  tvins.hInsertAfter = hPrev; 

  // Set the parent item based on the specified level. 
  if (nLevel == 1) 
    tvins.hParent = TVI_ROOT; 
  else if (nLevel == 2) 
    tvins.hParent = hPrevRootItem; 
  else if (nLevel == 3)
    tvins.hParent = hPrevLev2Item; 
  else
    tvins.hParent = hPrevLev3Item; 

  // Add the item to the tree-view control. 
  hPrev = (HTREEITEM)SendMessage(hwndTV, TVM_INSERTITEM, 
    0, (LPARAM)(LPTVINSERTSTRUCT)&tvins); 

  if (hPrev == NULL)
    return NULL;

  // Save the handle to the item. 
  if (nLevel == 1) 
    hPrevRootItem = hPrev; 
  else if (nLevel == 2) 
    hPrevLev2Item = hPrev; 
  else if (nLevel == 3)
    hPrevLev3Item = hPrev;
    
  // The new item is a child item. Give the parent item a 
  // closed folder bitmap to indicate it now has child items. 
  if (nLevel > 1)
  { 
    hti = TreeView_GetParent(hwndTV, hPrev); 
    tvi.mask = 0; //TVIF_IMAGE | TVIF_SELECTEDIMAGE; 
    tvi.hItem = hti; 
    //tvi.iImage = g_nClosed; 
    //tvi.iSelectedImage = g_nClosed; 
    TreeView_SetItem(hwndTV, &tvi); 
  } 

  return hPrev; 
} 

HTREEITEM AddItemToTreeAfter(HWND hwndTV, LPSTR lpszItem, int nLevel, int data, HTREEITEM hParent, HTREEITEM hPrev)
{ 
  TVITEM tvi; 
  TVINSERTSTRUCT tvins; 
  //static HTREEITEM hPrev = (HTREEITEM)TVI_FIRST; 
  static HTREEITEM hPrevRootItem = NULL; 
  static HTREEITEM hPrevLev2Item = NULL; 
  HTREEITEM hti; 

  tvi.mask = TVIF_TEXT /*| TVIF_IMAGE | TVIF_SELECTEDIMAGE */| TVIF_PARAM | TVIF_STATE; 
  tvi.state = TVIS_EXPANDED; tvi.stateMask = TVIS_EXPANDED;

  // Set the text of the item. 
  tvi.pszText = lpszItem; 
  tvi.cchTextMax = sizeof(tvi.pszText)/sizeof(tvi.pszText[0]); 

  // Assume the item is not a parent item, so give it a 
  // document image. 
  //tvi.iImage = g_nDocument; 
  //tvi.iSelectedImage = g_nDocument; 

  tvi.lParam = (LPARAM)(data);
  
  tvins.item = tvi; 
  tvins.hInsertAfter = hPrev; 

  // Set the parent item based on the specified level. 
  //if (nLevel == 1) 
    tvins.hParent = hParent; //TVI_ROOT; 
  //else if (nLevel == 2) 
  //  tvins.hParent = ; 
  //else 
  //  tvins.hParent = hPrevLev2Item; 

  // Add the item to the tree-view control. 
  hPrev = (HTREEITEM)SendMessage(hwndTV, TVM_INSERTITEM, 
    0, (LPARAM)(LPTVINSERTSTRUCT)&tvins); 

  if (hPrev == NULL)
    return NULL;

  // Save the handle to the item. 
  if (nLevel == 1) 
    hPrevRootItem = hPrev; 
  else if (nLevel == 2) 
    hPrevLev2Item = hPrev; 

  // The new item is a child item. Give the parent item a 
  // closed folder bitmap to indicate it now has child items. 
  if (nLevel > 1)
  { 
    hti = TreeView_GetParent(hwndTV, hPrev); 
    tvi.mask = 0; //TVIF_IMAGE | TVIF_SELECTEDIMAGE; 
    tvi.hItem = hti; 
    //tvi.iImage = g_nClosed; 
    //tvi.iSelectedImage = g_nClosed; 
    TreeView_SetItem(hwndTV, &tvi); 
  } 

  return hPrev; 
} 

unsigned int GetSelectedIndex(NMTREEVIEW *treeView)
{
  TVITEM selected = treeView->itemNew;
  return ((unsigned int)selected.lParam & 0x7FFF);
}  

unsigned int GetSelectedLevel(NMTREEVIEW *treeView)
{
  TVITEM selected = treeView->itemNew;
  return ((unsigned int)selected.lParam >> 30);
}

unsigned int GetSelectedParent(NMTREEVIEW *treeView)
{
  TVITEM selected = treeView->itemNew;
  return (((unsigned int)selected.lParam >> 15) & 0x7FFF);
}

void *GetSelectedItem(NMTREEVIEW *treeView)
{
  TVITEM selected = treeView->itemNew;
  return (void*)selected.lParam;
}

unsigned int GetSelectedItemData(NMTREEVIEW *treeView)
{
  TVITEM selected = treeView->itemNew;
  return (unsigned int)selected.lParam;
}

TVITEM HandleRClick(HWND treeView)
{
  HTREEITEM selected = (HTREEITEM)SendMessage(treeView, TVM_GETNEXTITEM, TVGN_DROPHILITE, 0);
      
  TVITEM item;
  if (selected != NULL)
  {    
    SendMessage(treeView, TVM_SELECTITEM, TVGN_CARET, (LPARAM)selected);
  }
  
  return item;
}
