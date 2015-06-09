#include "select.h"

// child plugins can use this plugin as a parent to record the 3-dimensional 
// location of a selectable item (or rather, project it to a 2-d map) within
// the context scene; when the context is hovered over, this plugin will then 
// locate the selectable item that is closest to the hover point. the plugin
// houses a single 'selections' model for child plugins to reserve the 
// appropriate primitives for display as selections of the respective items


void context_plugin_select::onInit()
{
  if (!inited)
  {
    parentContext->getViewport(contextWidth, contextHeight);
    contextMap = new selectable*[contextWidth*contextHeight];
      
    selections = geom->getMisc();
    selections->color = { 1.0, 1.0, 1.0 };
    
    inited = true;
    
    // handle all 'select' type plugins' init
    // context_plugin::onInit();
  }
}

void context_plugin_select::onRender()
{
  selectableCount = 0;  

  // reset context selectables z-buffer/map 
  for (int lp=0; lp<contextWidth*contextHeight; lp++)
    contextMap[lp] = 0;
      
  // handle all 'select' type plugins' onRender
  // context_plugin::onRender();
}

void context_plugin_select::onHover(int x, int y)
{
  int index = x + (y * contextWidth);
  
  unsigned char sortedDists[54] =  { 1, 0,  1, 1,  2, 0,  2, 1,  2, 2,  3, 0,  3, 1,  3, 2,  
                                     4, 0,  4, 1,  3, 3,  4, 2,  5, 0,  4, 3,  5, 1 , 5, 2,         
                                     4, 4,  5, 3,  6, 0,  6, 1,  6, 2,  5, 4,  6, 3 , 5, 5, 
                                     6, 4,  6, 5,  6, 6, };
                              
  int min = 0;
  int max = contextWidth * contextHeight;
  
  if (!contextMap[index])
  {
    int xoff, yoff;
    for (int lp=0; lp<27; lp++)
    {
      int i    = lp*2;
      
      xoff = sortedDists[i];
      yoff = sortedDists[i+1];  
      index = (x + xoff) + ((y+yoff) * contextWidth);
      if (index >= min && index < max)
      {
        if (contextMap[index])
          break;
      }

      xoff = -sortedDists[i];
      yoff = sortedDists[i+1];
      index = (x + xoff) + ((y+yoff) * contextWidth);
      if (index >= min && index < max)
      {
        if (contextMap[index])
          break;
      }
        
      xoff = sortedDists[i];
      yoff = -sortedDists[i+1];
      index = (x + xoff) + ((y+yoff) * contextWidth);
      if (index >= min && index < max)
      {
        if (contextMap[index])
          break;
      }
      
      xoff = -sortedDists[i];
      yoff = -sortedDists[i+1];
      index = (x + xoff) + ((y+yoff) * contextWidth);
      if (index >= min && index < max)
      {
        if (contextMap[index])
          break;
      }
        
      if (sortedDists[i] != sortedDists[i+1])
      {
        xoff = sortedDists[i+1];
        yoff = sortedDists[i];  
        index = (x + xoff) + ((y+yoff) * contextWidth);
        if (index >= min && index < max)
        {
          if (contextMap[index])
            break;
        }

        xoff = -sortedDists[i+1];
        yoff = sortedDists[i];
        index = (x + xoff) + ((y+yoff) * contextWidth);
        if (index >= min && index < max)
        {
          if (contextMap[index])
            break;
        }
          
        xoff = sortedDists[i+1];
        yoff = -sortedDists[i];
        index = (x + xoff) + ((y+yoff) * contextWidth);
        if (index >= min && index < max)
        {
          if (contextMap[index])
            break;
        }
        
        xoff = -sortedDists[i+1];
        yoff = -sortedDists[i];
        index = (x + xoff) + ((y+yoff) * contextWidth);
        if (index >= min && index < max)
        {
          if (contextMap[index])
            break;
        }
      }
    }
  }
  
  if (index >= min && index < max)
    closestSelectable = contextMap[index];
  else
    closestSelectable = 0;
  
  // handle all 'select' type plugins' onHover
  // context_plugin::onHover(x, y);
}

void context_plugin_select::recordSelectable(dpoint loc, int type, int index, int subindex)
{
  dpoint coords = parentContext->project2d(loc.X, loc.Y, loc.Z);

  int drwX = (int)coords.X;
  int drwY = (int)coords.Y;
  int priority = (int)coords.Z * 16;
  
  if (drwX >= 0 && drwX < contextWidth &&
      drwY >= 0 && drwY < contextHeight)
  {          
    int map_index = drwX + (drwY * contextWidth);
    
    selectable *cur = contextMap[map_index];
    
    if (cur)
    {
      if (cur->z < priority)
      {
        while (cur->next     && 
               cur->next->z < priority)
          cur = cur->next;
        
        selectable *next = cur->next;
        cur->next = &selectables[selectableCount++];
        
        cur = cur->next;
        cur->next = next;
      }
      else
      {
        contextMap[map_index] = &selectables[selectableCount++];
        contextMap[map_index]->next = cur;
        
        cur = contextMap[map_index];
      }
    }
    else
    {
      contextMap[map_index] = &selectables[selectableCount++];
     
      cur = contextMap[map_index];
      cur->next = 0;
    }
   
    cur->z = priority;
    cur->type = type;
    cur->item_index = index;
    cur->vert_index = subindex;
  }
}