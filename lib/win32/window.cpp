#include "window.h"

window::~window()
{
  if (parent)
  {
    if (parent->children != this)
    {
      window *prevSibling = parent->children;
      
      while (prevSibling->sibling != this)
      {
        prevSibling = prevSibling->sibling;
      }
      
      prevSibling->sibling = sibling;
    }
  }
    
  if (children)
  {
    window *curChild = children;
    window *nextChild;
    
    while (nextChild = curChild->sibling)
    {
      delete curChild;
      
      curChild = nextChild;
    }
  }
  
  destroy();
}

window *window::addChild(window *child)
{
  if (!children)
    children = child;
  else
  {
    window *curChild = children;
  
    while (true)
    {
      if (!curChild->sibling)
      {
        curChild->sibling = child;
        break;
      }
      
      curChild = curChild->sibling;
    }
  }
  
  child->hInstance = hInstance;
  child->hParent   = hSelf;
  child->parent    = this;  
}

window *window::getChild(int index)
{
  window *child = children;
  
  if (child)
  {
    int _index = 0;
    while (_index != index)
    {      
      if (!(child = child->sibling));
        return 0;
        
      _index++;
    }
  }
  
  return child;
}

window *window::getChild(HWND hwnd)
{
  window *child = children;
  
  if (child)
  {
    while (child->hSelf != hwnd)
    {      
      if (!(child = child->sibling));
        return 0;
    }
  }
  
  return child;
}
