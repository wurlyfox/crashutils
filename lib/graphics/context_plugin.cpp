#include "context_plugin.h"

context_plugin::context_plugin()
{
  parentContext = 0;
  contextScene = 0;
  geom = 0;
  
  parent = 0;
  sibling = 0;
  children = 0;
}

context_plugin::context_plugin(context *ctxt)
{
  parentContext = ctxt;
  
  contextScene = parentContext->getScene();
  geom = contextScene->getGeometry();
  
  parent = 0;
  sibling = 0;
  children = 0;
}

context_plugin::~context_plugin()
{
  if (parent)
  {
    if (parent->children != this)
    {
      context_plugin *prevSibling = parent->children;
      
      while (prevSibling->sibling != this)
      {
        prevSibling = prevSibling->sibling;
      }
      
      prevSibling->sibling = sibling;
    }
  }
    
  if (children)
  {
    context_plugin *curChild = children;
    context_plugin *nextChild;
    
    while (nextChild = curChild->sibling)
    {
      delete curChild;
      
      curChild = nextChild;
    }
  }
}

void context_plugin::addChild(context_plugin *child)
{
  if (!children)
    children = child;
  else
  {
    context_plugin *curChild = children;
  
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

context_plugin *context_plugin::getChild(int index)
{
  if (!children)
    return 0;

  context_plugin *curChild = children;
  int curIndex = 0;
  
  while (index != curIndex)
  {
    if (!(curChild = curChild->sibling))
      return 0;
  
    curIndex++;
  }
  
  return curChild; 
}

void context_plugin::setContext(context *ctxt)
{
	context_plugin *curChild = children;
  while (curChild)
	{
	  curChild->setContext(ctxt);
		curChild = curChild->sibling;
  }
	
	parentContext = ctxt;
  contextScene = parentContext->getScene();
  geom = contextScene->getGeometry();
}
  
void context_plugin::messageRouter(int msg, param lparam, param rparam)
{  
  switch (msg)
  {
    case CPM_INIT:                           onInit(); break;
    case CPM_RENDER:                       onRender(); break;
    case CPM_HOVER: onHover((int)lparam, (int)rparam); break;
    case CPM_POINT: onPoint((int)lparam, (int)rparam); break;
  }
  
  if (children)
  {
    context_plugin *cur = children;
    do 
    {
      cur->messageRouter(msg, lparam, rparam);
    } while (cur = cur->sibling);
  }
}

void context_plugin::postMessage(int msg, param lparam, param rparam)
{
  context_plugin *cur = this;
  
  while (cur->parent)
    cur = cur->parent;
    
  cur->messageRouter(msg, lparam, rparam);
}

