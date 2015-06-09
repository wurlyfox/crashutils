// Implementation for csystem class member functions. 
// A CrashUtils csystem is simply a generic observer; 

#include "system.h"

csystem::csystem()
{ 
  parent = 0;
  sibling = 0;
  children = 0;
}

csystem::~csystem()
{
  if (parent)
  {
    if (parent->children != this)
    {
      csystem *prevSibling = parent->children;
      
      while (prevSibling->sibling != this)
      {
        prevSibling = prevSibling->sibling;
      }
      
      prevSibling->sibling = sibling;
    }
  }
    
  if (children)
  {
    csystem *curChild = children;
    csystem *nextChild;
    
    while (nextChild = curChild->sibling)
    {
      delete curChild;
      
      curChild = nextChild;
    }
  }
}

void csystem::addChild(csystem *child)
{
  if (!children)
    children = child;
  else
  {
    csystem *curChild = children;
  
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
  
  child->parent = this;
}

void csystem::removeChild(csystem *child)
{
  if (children)
  {
    csystem *prevChild = 0;
    csystem *curChild  = children;

    while (curChild != child)
    {
      prevChild = curChild;
      
      if (!(curChild = curChild->sibling))
        return;
    }
    
    if (prevChild)
      prevChild->sibling = curChild->sibling;   
  }
} 
      
csystem *csystem::getChild(int index)
{
  if (!children)
    return 0;

  csystem *curChild = children;
  int curIndex = 0;
  
  while (index != curIndex)
  {
    if (!(curChild = curChild->sibling))
      return 0;
  
    curIndex++;
  }
  
  return curChild; 
}
          
void csystem::messageRouter(csystem *src, int msg, param lparam, param rparam)
{ 
  if (src == this)
    return;
    
  if (children)
  {
    csystem *cur = children;
    do 
    {
      cur->messageRouter(src, msg, lparam, rparam);
    } while (cur = cur->sibling);
  }
}

void csystem::postMessage(int msg, param lparam, param rparam, bool filter)
{
  csystem *cur = this;
  
  while (cur->parent)
    cur = cur->parent;
    
  csystem *src = filter ? this : 0;
  cur->messageRouter(src, msg, lparam, rparam);
}

void csystem::postMessage(csystem *src, int msg, param lparam, param rparam)
{
  csystem *cur = this;
  
  while (cur->parent)
    cur = cur->parent;
    
  cur->messageRouter(src, msg, lparam, rparam);
}