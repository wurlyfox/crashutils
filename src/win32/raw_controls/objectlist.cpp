#include "objectlist.h"

extern object *objects;

objectList::objectList(int listID, int x, int y, int w, int h, HWND parent)
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
    parentCount = -1;
  }
  else
  {
    parentCount = 0;
    selectedObject = 0;
  }
}

int objectList::occupy(object *parentList, int count, const char *heading)
{
  char dataString[20];
  
  for (int lp = 0; lp < count; lp++)
  {
    sprintf(dataString, "%s %i", heading, lp+1);
    
    links[parentCount] = new objlink(&parentList[lp]);
    links[parentCount]->build(hwnd, dataString, 1, lp);
    
    parentCount++;
  }
  
  return 1;
} 

int objectList::update()
{
  for (int lp = 0; lp < parentCount; lp++)
    links[lp]->update(hwnd, "Object", 1);
}
    
int objectList::handle(LPARAM lParam)
{
  NMHDR* nmhdr = (NMHDR*)lParam;
  if (nmhdr->idFrom == ID)
  {
    if (nmhdr->code == TVN_SELCHANGED)
    {
      NMTREEVIEW* tree =    (NMTREEVIEW*)lParam;
      selectedObject   = (object*)GetSelectedItem(tree);
      
      return OBJECTLIST_SELCHANGED;
    }
    else if (nmhdr->code == NM_RCLICK)
    {
      return OBJECTLIST_RCLICK;
    }
  }
}

object *objectList::getSelectedObject()
{
  return selectedObject;
}


objlink::objlink(object *_obj)
{
  obj = _obj;
  parent = 0;
  prev = 0;
  next = 0;
  child = 0;
} 

objlink::~objlink()
{
  if (prev)
    prev->next = next;

  if (next)
    next->prev = prev;

  objlink *cur_child = child;
  objlink *next_child;

  while (cur_child)
  {
    next_child = cur_child->next;
    delete cur_child;
    cur_child = next_child;
  }     
} 

objlink *objlink::addChild(object *_obj)
{
  if (child)
    child = child->addBefore(_obj);
  else
    child = new objlink(_obj);

  child->parent = this;

  return child;
  
}

objlink *objlink::addBefore(object *_obj)
{
  objlink *newlink = new objlink(_obj);
  newlink->parent = parent;

  if (prev)
  {
    prev->next = newlink;
    newlink->prev = prev;
  }

  prev = newlink;
  newlink->next = this;

  return newlink;
}

objlink *objlink::addAfter(object *_obj)
{
  objlink *newlink = new objlink(_obj);
  newlink->parent = parent;

  if (next)
  {
    next->prev = newlink;
    newlink->next = next;
  }

  next = newlink;
  newlink->prev = this;

  return newlink;
}

objlink *objlink::addChild(object *_obj, HWND hwnd, const char *heading, int level)
{
  objlink *newnode = addChild(_obj);
  newnode->makeNode(hwnd, heading, level);
  return newnode;
}

objlink *objlink::addBefore(object *_obj, HWND hwnd, const char *heading, int level)
{
  objlink *newnode = addBefore(_obj);
  newnode->makeNode(hwnd, heading, level);
  return newnode;
}

objlink *objlink::addAfter(object *_obj, HWND hwnd, const char *heading, int level)
{
  objlink *newnode = addAfter(_obj);
  newnode->makeNode(hwnd, heading, level);
  return newnode;
}

//for building the initial instance of the list
void objlink::build(HWND hwnd, const char *heading, int level, int index)
{
  makeNode(hwnd, heading, level, index);

  object *children = getChildren(obj);
  if (children)
  {
    addChild(children);
    child->build(hwnd, "Object", level+1);

    objlink *next_child = child;
    while (next_child->obj->process.sibling)
    {
      next_child = child->addAfter(next_child->obj->process.sibling);
      next_child->build(hwnd, "Object", level+1);
    }
  }
}

void objlink::makeNode(HWND hwnd, const char *heading, int level, int index)
{
  if (index == -1)
    index = getObjectIndex(obj);

  char dataString[20];
  sprintf(dataString, "%s %i", heading, index+1);

  HTREEITEM item_parent = parent ? parent->treeitem : 0;
  HTREEITEM item_prev = prev ? prev->treeitem : 0;
  treeitem = AddItemToTreeAfter(hwnd, dataString, level, obj, item_parent, item_prev);
}

void objlink::deleteNode(HWND hwnd)
{
  if (treeitem)
    TreeView_DeleteItem(hwnd, treeitem);
}

objlink *objlink::diff(bool &equiv)  //compare child list with game's child list
{
  equiv = false;
  if (!child)                          //child null, children null or non null
  {
    equiv = !getChildren(obj);

    return 0;
  } 
  else if (getChildren(obj) != child->obj)  //child nonnull, children != child (including child == null)
    return 0;
  else
  {
    objlink *link = child;
         
    while (link->next &&
           link->next->obj == link->obj->process.sibling &&
           (link = link->next));
        
    if (link->next == 0 && link->obj->process.sibling == 0)   //if ...   n 0           //if ...   n 0          //   ...   n ?  th                                        equiv = true;                
      equiv = true;
                                                              //   ...   n x???  then lists not equivalent                                      
    return link;
  }
}

//for updating to future instances of the list
void objlink::update(HWND hwnd, const char *heading, int level)   //update the child list's order to game's child list's order
{
  if (!child && getChildren(obj))
  {    
    object *children = getChildren(obj);
    
    if ((unsigned long)children < (unsigned long)objects || (unsigned long)children > (unsigned long)&objects[96])
    {
      return;
    }
    
    if (children)
    {
      addChild(children);
      child->build(hwnd, "Object", level+1);

      objlink *next_child = child;
      while (next_child->obj->process.sibling)
      {
        next_child = next_child->addAfter(next_child->obj->process.sibling);
        next_child->build(hwnd, "Object", level+1);
      }
    }
    
    return;
  }
  else if (child && !getChildren(obj))
  {
    objlink *victim = child;

    while (victim)
    {
      objlink *tmp = victim->next;
      victim->deleteNode(hwnd);
      delete victim;
      victim = tmp;
    }
    
    child = 0;
    
    return;        //1  2
  }                //2          
  
  bool equiv = false;
  objlink *diff_pt = diff(equiv);
     
  if (!equiv && !diff_pt)
  {
    if (child->obj != getChildren(obj))
    {
      child->deleteNode(hwnd);
      delete child;
      child = 0;
      addChild(getChildren(obj));
      child->makeNode(hwnd, heading, level+1);
      diff_pt=child;
    }
    else
    {
      object *children = getChildren(obj);
      object *nextchild = children->process.sibling;
      objlink *nextlink = addAfter(nextchild);
      nextlink->makeNode(hwnd, heading, level+1);
      diff_pt = diff(equiv);
    }
  }
    
  while(!equiv) 
  { 
    object *newlinkobj;
      
    if (diff_pt->next == 0)
    {
      object *newlinkobj = diff_pt->obj->process.sibling;

      while (newlinkobj)
      {
        diff_pt = diff_pt->addAfter(newlinkobj, hwnd, heading, level+1); 
        newlinkobj = diff_pt->obj->process.sibling;
      }

      equiv = true; //ends loop
    }
    else if (diff_pt->obj->process.sibling == 0)
    {
      objlink *victim = diff_pt->next;

      while (victim)
      {
        objlink *tmp = victim->next;
        victim->deleteNode(hwnd);
        delete victim;
        victim = tmp;
      }

      equiv = true;  //ends loop
    }
    else
    {
      object *rec = diff_pt->next->obj;
      object *gam = diff_pt->obj->process.sibling;

      int n=1;
      do
      {
        if (rec)                                              
        {
          object *newlinkobj = diff_pt->obj->process.sibling;
          
          if (!diff_pt->next ||
              !diff_pt->next->obj->process.sibling ||
              rec == diff_pt->next->obj->process.sibling)   
          {  
            objlink *_next = diff_pt->next;   //cant have name next...
       
            //add the n objects starting at link     
            for (int lp = 0; lp < n-1 && newlinkobj; lp++)
            {
              _next = _next->addAfter(newlinkobj, hwnd, heading, level+1);
              newlinkobj = newlinkobj->process.sibling;
            }
                  
            break;
          }
        }
        if (gam)
        {
          if (!diff_pt->next->next ||
              !diff_pt->next->next->obj ||
              gam == diff_pt->next->next->obj)
          {
            //remove the n objects starting after link
            for (int lp = 0; lp < n-1 && diff_pt; lp++)
            {     
              diff_pt->next->deleteNode(hwnd);
              delete diff_pt->next;
              diff_pt = diff_pt->next;
            }
             
            break;
          }
        }
               
        n++;

        diff_pt = diff_pt->next;
 
      } while (diff_pt);
      
      diff_pt = diff(equiv);
    }
  }    

  //update all child nodes as well
  objlink *link = child;
  while (link)
  {
    child->update(hwnd, heading, level+1);
    link = link->next;
  }    
} 
    