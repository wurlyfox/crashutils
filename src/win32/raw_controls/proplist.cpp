#include "proplist.h"

#include <stdio.h>

propertyList::propertyList(int listID, int listType, int x, int y, int w, int h, HWND parent)
{
	  
  ID = listID;
  
  HINSTANCE hInstance = GetModuleHandle(NULL);
  
  hwnd = CreateWindowEx(0, WC_LISTVIEW, "", 
    WS_CHILD | WS_VISIBLE | WS_HSCROLL | LVS_REPORT, 
    x, y, w, h,
    parent, (HMENU)ID, hInstance, NULL);

  if (!hwnd)
  {
    MessageBox(NULL,"Tree Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
    type = PROPERTYLIST_NULL;
  }
  else
  {
    type = listType;
	  propCount = 0;
    curOffset = 0;
  }
}

void propertyList::setLength(unsigned long _length)
{
  length = _length;
}

void propertyList::addProperty(char _name[20], unsigned long _offset, unsigned char _size, unsigned char _type)
{
  strcpy(itemProperties[propCount].name, _name);
  itemProperties[propCount].size = _size;
  itemProperties[propCount].type = _type;
  itemProperties[propCount].offset = _offset;
  curOffset = _offset;
  
  current = &itemProperties[propCount];
  
  propCount++;
}

void propertyList::addProperty(char _name[20], unsigned char _size, unsigned char _type)
{
  strcpy(itemProperties[propCount].name, _name);
  itemProperties[propCount].size = _size;
  itemProperties[propCount].type = _type;
  itemProperties[propCount].offset = curOffset;
  curOffset += _size;
  length = curOffset;
  
  current = &itemProperties[propCount];
  
  propCount++;
}

void propertyList::addProperty(unsigned char _size)
{
  itemProperties[propCount].size = _size;
  itemProperties[propCount].type = PROPERTY_BITS;
  itemProperties[propCount].offset = curOffset;
  itemProperties[propCount].subCount = 0;
  curOffset += _size;
  length = curOffset;
  
  current = &itemProperties[propCount];
  
  propCount++;
  
  curSuboffset = 0;
}

void propertyList::addSubproperty(char _name[20], unsigned char _size, unsigned char _type)
{
  if (current->type == PROPERTY_BITS)
  {
    strcpy(current->bits[current->subCount].name, _name);
    current->bits[current->subCount].size = _size;
    current->bits[current->subCount].type = _type;
    current->bits[current->subCount].offset = curSuboffset;
    curSuboffset += _size;

    current->subCount++;
  }
}
  
bool propertyList::addItem(unsigned char *itemData, unsigned long offset)
{
  char temp[20];
  
  LVITEM lvI;
  
  lvI.mask      = LVIF_TEXT | LVIF_IMAGE |LVIF_STATE;
  lvI.stateMask = 0;
  lvI.iSubItem  = 0;
  lvI.state     = 0;

  for (int prop = 0; prop < propCount; prop++)    
  {
	  unsigned long totOffset = offset + itemProperties[prop].offset;
    unsigned char size = itemProperties[prop].size;
	  long value;
	
    if (type == PROPERTY_BITS)
    {
       //
    
    }
    else
    {
      if (size == PROPERTY_BYTE)
	    {
        value = itemData[totOffset];
	      if (type == PROPERTY_INT)
          sprintf(temp, "%i", value);
        else if (type == PROPERTY_HEX)
	      sprintf(temp, "0x%02x", value);
	    }
      else if (size == PROPERTY_HWORD)
	    {
        value = getHword(itemData, totOffset, true);
	      if (type == PROPERTY_INT)
          sprintf(temp, "%i", value);
        else if (type == PROPERTY_HEX)
	        sprintf(temp, "0x%04x", value);
      }
	    else if (type == PROPERTY_GOOLOFFSET)
	    {
	      if (value != 0x3FFF)
	        sprintf(temp, "0x%04x", value*4);
	      else
		      sprintf(temp, "none");
      }
	    else if (size == PROPERTY_WORD)
	    {
        value = getWord(itemData, totOffset, true);		
        if (type == PROPERTY_INT)
          sprintf(temp, "%i", value);
        else if (type == PROPERTY_HEX)
	        sprintf(temp, "0x%08x", value);
        else if (type == PROPERTY_EID)
          getEIDstring(temp, value);
      }
    
      lvI.pszText = temp;
      lvI.iItem   = 0;            //not right?
      lvI.iImage  = 0;
	  
	    if (prop == 0)
	    {
        if (ListView_InsertItem(hwnd, &lvI) == -1)
          return FALSE;
      }
	    else
	    {
	      lvI.iSubItem = prop;
        if (ListView_SetItem(hwnd, &lvI) == -1)
          return FALSE;
      }
    }
  }
  
  return TRUE;
}

bool propertyList::occupy(unsigned char *itemData, unsigned long offset, int count)
{
  char temp[20];
  
  LVITEM lvI;
  
  lvI.mask      = LVIF_TEXT | LVIF_IMAGE |LVIF_STATE;
  lvI.stateMask = 0;
  lvI.iSubItem  = 0;
  lvI.state     = 0;

  unsigned long totOffset;
  unsigned char size;
  unsigned char type;
  long value;
	
  for (int item = 0; item < count; item++)
  {	
    int curProp = 0;
    for (int prop = 0; prop < propCount; prop++)    
    {
      
      totOffset = offset + itemProperties[prop].offset;
      size      = itemProperties[prop].size;
      type      = itemProperties[prop].type;
	  
      bool skip = false;
      if (type == PROPERTY_BITS)
      {
        long data;
        if (size == PROPERTY_BYTE)
          data = itemData[totOffset];      
        else if (size == PROPERTY_HWORD)
          data = getHword(itemData, totOffset, true);
        else if (size == PROPERTY_WORD)
          data = getWord(itemData, totOffset, true);
        else
          data = 0;
          
        for (int lp = 0; lp < itemProperties[prop].subCount; lp++)
        {
          unsigned char bitSize = itemProperties[prop].bits[lp].size;
          unsigned char bitType = itemProperties[prop].bits[lp].type;
          unsigned char offset  = itemProperties[prop].bits[lp].offset;
        
          unsigned long bits = (1 << bitSize) - 1;
          unsigned char rightOffset = ((size*8) - (offset + bitSize));
          		
          value = (data & (bits << rightOffset)) >> rightOffset;

          if (bitType == PROPERTY_INT)
            sprintf(temp, "%i", value);
          else if (bitType == PROPERTY_HEX)
            sprintf(temp, "0x%02x", value);     
          else if (bitType == PROPERTY_SKIP)
          {
            sprintf(temp, "%i", value);
            if (value == 0) { skip = true; }
          }
          lvI.pszText = temp;
          lvI.iItem   = item;
          lvI.iImage  = item;
          
          if (curProp == 0)
          {
            lvI.iSubItem = 0;
            if (ListView_InsertItem(hwnd, &lvI) == -1)
              return FALSE;
          }
          else
          {
            lvI.iSubItem = curProp;
            if (ListView_SetItem(hwnd, &lvI) == -1)
              return FALSE;
          }
          
          curProp++;
        }
        
        if (skip == true)
        {
          skip = false;
          break;
        }
      }   
      else
      {
        if (size == PROPERTY_BYTE)
        {
          value = itemData[totOffset];
          if (type == PROPERTY_INT)
            sprintf(temp, "%i", value);
          else if (type == PROPERTY_HEX)
            sprintf(temp, "0x%02x", value);
        }
        else if (size == PROPERTY_HWORD)
        {
          value = getHword(itemData, totOffset, true);
          if (type == PROPERTY_INT)
            sprintf(temp, "%i", value);
          else if (type == PROPERTY_HEX)
            sprintf(temp, "0x%04x", value);
          else if (type == PROPERTY_GOOLOFFSET)
          {
            if (value != 0x3FFF)
              sprintf(temp, "sub_%04x", value*4);
            else
              sprintf(temp, "none");
          }
        }
        else if (size == PROPERTY_WORD)
        {
          value = getWord(itemData, totOffset, true);		
          if (type == PROPERTY_INT)
            sprintf(temp, "%i", value);
          else if (type == PROPERTY_HEX)
            sprintf(temp, "0x%08x", value);
          else if (type == PROPERTY_EID)
            getEIDstring(temp, value);
        }
	  
        lvI.pszText  = temp;
        lvI.iItem    = item;
        lvI.iImage   = item;
        if (curProp == 0)
        {
          lvI.iSubItem = 0;
          if (ListView_InsertItem(hwnd, &lvI) == -1)
            return FALSE;
        }
        else
        {
          lvI.iSubItem = curProp;
          if (ListView_SetItem(hwnd, &lvI) == -1)
            return FALSE;
        }
          
        curProp++;
      }
    }
	
	offset += length;
  }

  return TRUE;
}

bool propertyList::initColumns()
{
  char szText[256];
  LVCOLUMN lvc;

  // Initialize the LVCOLUMN structure.
  // The mask specifies that the format, width, text,
  // and subitem members of the structure are valid.
  lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

  // Add the columns.
  int itemCount = 0;
  for (int count = 0; count < propCount; count++)
  {  
    if (itemProperties[count].type == PROPERTY_BITS)
    {
      for (int lp = 0; lp < itemProperties[count].subCount; lp++)
      {
        char *name = itemProperties[count].bits[lp].name;
    
        lvc.iSubItem = itemCount;
        lvc.pszText = name;

        int headsize = strlen(name);
        int datasize = ((itemProperties[count].bits[lp].size/8) << 1);
        if (datasize > headsize)
          lvc.cx = datasize*12;               // Width of column in pixels.
        else
          lvc.cx = headsize*12;
      
        if ( itemCount < 2 )
          lvc.fmt = LVCFMT_LEFT;  // Left-aligned column.
        else
          lvc.fmt = LVCFMT_RIGHT; // Right-aligned column.

        // Insert the columns into the list view.
        if (ListView_InsertColumn(hwnd, itemCount, &lvc) == -1)
          return false;
        
        itemCount++;
      }
    }
    else
    {    
      char *name = itemProperties[count].name;
    
      lvc.iSubItem = count;
      lvc.pszText = name;

      int headsize = strlen(name);
      int datasize = (itemProperties[count].size << 1);
      if (datasize > headsize)
        lvc.cx = datasize*12;               // Width of column in pixels.
      else
        lvc.cx = headsize*12;
      
      if ( itemCount < 2 )
        lvc.fmt = LVCFMT_LEFT;  // Left-aligned column.
      else
        lvc.fmt = LVCFMT_RIGHT; // Right-aligned column.

      // Insert the columns into the list view.
      if (ListView_InsertColumn(hwnd, count, &lvc) == -1)
        return false;
        
      itemCount++;
    }
  }
    
  return true;
} 
  
void propertyList::clear()
{
  ListView_DeleteAllItems(hwnd);
}

//in WM_NOTIFY
int propertyList::handle(LPARAM lParam)
{
  NMHDR* nmhdr = (NMHDR*)lParam;
	  
  if (nmhdr->code == LVN_ITEMCHANGED && nmhdr->idFrom == ID)
  {
    NMLISTVIEW* list = (NMLISTVIEW*)lParam;    
    return list->iItem;
  }
  else
    return -1;
}  