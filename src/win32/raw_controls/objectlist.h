#ifndef OBJECTLIST_H
#define OBJECTLIST_H

#define OBJECTLIST_UNCHANGED  0x0000
#define OBJECTLIST_RCLICK     0x0001
#define OBJECTLIST_SELCHANGED 0x0002

#include <windows.h>
#include <stdio.h>

#include "win32/raw_controls/tree.h"

#include "../../crash/src/goolengine.h"

class objlink
{
  public:
  
  HTREEITEM treeitem;
  object *obj;
  objlink *parent;
  objlink *prev;
  objlink *next;
  objlink *child;

  objlink(object *_obj);
  ~objlink();
  
  objlink *addChild(object *_obj);
  objlink *addAfter(object *_obj);
  objlink *addBefore(object *_obj);
  objlink *addChild(object *_obj, HWND hwnd, const char *heading, int level);
  objlink *addAfter(object *_obj, HWND hwnd, const char *heading, int level);
  objlink *addBefore(object *_obj, HWND hwnd, const char *heading, int level);
  void build(HWND hwnd, const char *heading, int level, int index=-1);
  void makeNode(HWND hwnd, const char *heading, int level, int index=-1);
  void deleteNode(HWND hwnd);
  objlink *diff(bool &equiv);  
  void update(HWND hwnd, const char *heading, int level);   //update the child list's order to game's child list's order
};

class objectList
{
  private:
  
  HWND hwnd;
  int ID;
  int parentCount; 
  object *selectedObject;
  
  public:
  
  objlink *links[10];
  
  objectList(int listID, int x, int y, int w, int h, HWND parent);

  int occupy(object *parentList, int count, const char *heading);
  int update();
  
  int handle(LPARAM lParam);
  object *getSelectedObject();
  void clear();
};



#endif