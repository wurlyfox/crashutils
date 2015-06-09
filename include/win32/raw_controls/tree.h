#include <windows.h>
#include <commctrl.h>

HTREEITEM AddItemToTree(HWND hwndTV, LPSTR lpszItem, int nLevel, int pIndex, int nIndex);
HTREEITEM AddItemToTree(HWND hwndTV, LPSTR lpszItem, int nLevel, void *item);
HTREEITEM AddItemToTree(HWND hwndTV, LPSTR lpszItem, int nLevel, int data);
HTREEITEM AddItemToTreeAfter(HWND hwndTV, LPSTR lpszItem, int nLevel, void *item, HTREEITEM hParent, HTREEITEM hPrev);
HTREEITEM AddItemToTreeAfter(HWND hwndTV, LPSTR lpszItem, int nLevel, int data, HTREEITEM hParent, HTREEITEM hPrev);
unsigned int GetSelectedIndex(NMTREEVIEW *treeView);
unsigned int GetSelectedLevel(NMTREEVIEW *treeView);
unsigned int GetSelectedParent(NMTREEVIEW *treeView);
void *GetSelectedItem(NMTREEVIEW *treeView);
unsigned int GetSelectedItemData(NMTREEVIEW *treeView);

//workarounds for handling right click in a treeview
TVITEM HandleRClick(HWND treeView);

/*unsigned int GetItemIndex(TVITEM item);
unsigned int GetItemLevel(TVITEM item);
unsigned int GetItemParent(TVITEM item);
void *GetItemItem(TVITEM item);*/
