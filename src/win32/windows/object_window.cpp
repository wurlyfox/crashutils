#include "object_window.h"

#include "gool_window.h"

LRESULT object_window::onCreate(UINT uMsg, WPARAM wParam, LPARAM lParam)
{     
  //InitPropertyGrid(hInstance);
      
  RECT rcClient;
  GetClientRect(&rcClient);
      
  //objectProps = CreateWindowEx(0, "PropGridCtl", "",
  //  WS_CHILD | WS_VISIBLE | WS_TABSTOP, 200, 0, 300, rcClient.bottom, hSelf, (HMENU)IDC_OBJPROPS, hInstance, NULL);
      
  initObjects();
  //initObjectProperties();
      
  objectMem = new hexEdit(IDC_OBJMEM, 500, 0, 20, 16, 2, 2, 2, 2, hSelf);
  objectMem->addSelection(0, 0, RGB(255, 255, 255));
  objectMem->addSelection(0, 0, RGB(255, 255, 255));
    
  //setGoolSuspendCallback(&updateDebugInfo);
}

LRESULT object_window::onPaint(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  objectMem->paint();
}

LRESULT object_window::onOther(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
    /*case WM_ACTIVATE:
    {
	    if (debug)
	    {
        resumeGoolSystem();  		
      }
    }
    break;*/
    
    case WM_VSCROLL:
    {
      objectMem->scroll(wParam);
    }
    break;
  }
}

unsigned short curFP = 0;
unsigned short curSP = 0;
unsigned short prevFP = 0;
unsigned short prevSP = 0;

void object_window::updateDebugInfo(object *obj)
{
  //updateObjectList();
            
  //if (debugObject != obj)
  //  goolWindow->viewCodeEntry(obj->global);
  
  debugObject = obj;
  
  //int line = ((unsigned char*)debugObject->process.pc - (unsigned char*)debugObject->global->itemData[1])/4;
  //goolWindow->codeEdit->selectLine(line);
  
  //if (debug && wait)
  //{
    updateObjectProperties(debugObject);
          
    int dataLength = sizeof(debugObject) - ((unsigned char*)debugObject->process.memory - (unsigned char*)debugObject);
    objectMem->setSource((unsigned char*)debugObject->process.memory, dataLength);
    objectMem->occupy();
          
    curFP = (unsigned long)debugObject->process.fp - (unsigned long)debugObject->process.memory;
    curSP = (unsigned long)debugObject->process.sp - (unsigned long)debugObject->process.memory;
          
    objectMem->changeSelection(0, curFP, curSP-curFP, RGB(32, 32, 32));
         
    unsigned short start, end;
    if (prevSP != 0)
    { 
      start = prevSP;        
      if (prevSP > curSP)
      {          
        start = curSP;
        end = prevSP - curSP;
      }
      else
      {
        start = prevSP;
        end = curSP - prevSP;
      }
      objectMem->changeSelection(1, start, end, RGB(0, 64, 0));
    }
          
    prevFP = curFP;
    prevSP = curSP;
              
    //goolWindow->codeEdit->invalidate();            
    objectMem->invalidate();
  //}
}


/*
void object_window::initObjectProperties()
{
  PROPGRIDITEM Item;
  
  PropGrid_ItemInit(Item);

  char temp[40];
  char tempB[40];
  sprintf(temp, "Object Properties");
  sprintf(tempB, "Type");
  
  Item.lpszCatalog = temp;
  Item.lpszPropName = tempB; //Static text
  Item.lpCurValue = 0; //Depends on ItemType
  //Item.lpszPropDesc = _T("This field is editable"); //Static text
  Item.iItemType = PIT_STATIC;
  PropGrid_AddItem(objectProps, &Item);

  Item.lpszPropName = "Subtype";
  PropGrid_AddItem(objectProps, &Item);
  
  Item.lpszPropName = "Bounding Box";
  PropGrid_AddItem(objectProps, &Item);
  
  Item.lpszPropName = "GOOL Entry";
  PropGrid_AddItem(objectProps, &Item);
  
  Item.lpszPropName = "External GOOL Entry";
  PropGrid_AddItem(objectProps, &Item);
  
  Item.lpszPropName = "Zone";
  PropGrid_AddItem(objectProps, &Item);
  
  Item.lpszPropName = "Routine";
  PropGrid_AddItem(objectProps, &Item);
  
  Item.lpszCatalog = "Object Colors";
  Item.lpszPropName = "Light Matrix Column 1";
  PropGrid_AddItem(objectProps, &Item);

  Item.lpszPropName = "Light Matrix Column 2";
  PropGrid_AddItem(objectProps, &Item);

  Item.lpszPropName = "Light Matrix Column 3";
  PropGrid_AddItem(objectProps, &Item);

  Item.lpszPropName = "Light Color";
  Item.iItemType = PIT_COLOR;
  PropGrid_AddItem(objectProps, &Item);
  
  Item.lpszPropName = "Color Matrix Column 1";
  PropGrid_AddItem(objectProps, &Item);
  
  Item.lpszPropName = "Color Matrix Column 2";
  PropGrid_AddItem(objectProps, &Item);
  
  Item.lpszPropName = "Color Matrix Column 3";
  PropGrid_AddItem(objectProps, &Item);
 
  Item.lpszPropName = "Color Intensity";
  PropGrid_AddItem(objectProps, &Item);
  
  Item.lpszCatalog = "Links";
  Item.iItemType = PIT_STATIC;
  Item.lpszPropName = "Self";
  PropGrid_AddItem(objectProps, &Item);
  
  Item.lpszPropName = "Parent";
  PropGrid_AddItem(objectProps, &Item);
  
  Item.lpszPropName = "Sibling";
  PropGrid_AddItem(objectProps, &Item);
  
  Item.lpszPropName = "Child";
  PropGrid_AddItem(objectProps, &Item);
  
  Item.lpszPropName = "Link A";
  PropGrid_AddItem(objectProps, &Item);
  
  Item.lpszPropName = "Link B";
  PropGrid_AddItem(objectProps, &Item);
  
  Item.lpszPropName = "Link C";
  PropGrid_AddItem(objectProps, &Item);
  
  Item.lpszPropName = "Link D";
  PropGrid_AddItem(objectProps, &Item);

  Item.lpszCatalog = "Vectors";
  Item.lpszPropName = "Translate";
  PropGrid_AddItem(objectProps, &Item);
  
  Item.lpszPropName = "Rotate";
  PropGrid_AddItem(objectProps, &Item);

  Item.lpszPropName = "Scale";
  PropGrid_AddItem(objectProps, &Item);  
  
  Item.lpszPropName = "Misc A";
  PropGrid_AddItem(objectProps, &Item);  
  
  Item.lpszPropName = "Misc B";
  PropGrid_AddItem(objectProps, &Item);  
  
  Item.lpszPropName = "Misc C";
  PropGrid_AddItem(objectProps, &Item);  
  
  Item.lpszCatalog = "Object Process";
  Item.lpszPropName = "Status A";
  PropGrid_AddItem(objectProps, &Item);  
  
  Item.lpszPropName = "Status B";
  PropGrid_AddItem(objectProps, &Item);  
  
  Item.lpszPropName = "Status C";
  PropGrid_AddItem(objectProps, &Item);  

  Item.lpszPropName = "Initial Subroutine";
  PropGrid_AddItem(objectProps, &Item); 
  
  Item.lpszPropName = "PID/Flags";
  PropGrid_AddItem(objectProps, &Item);

  Item.lpszPropName = "Stack Pointer";
  PropGrid_AddItem(objectProps, &Item);

  Item.lpszPropName = "Program Counter";
  PropGrid_AddItem(objectProps, &Item); 

  Item.lpszPropName = "Frame Pointer";
  PropGrid_AddItem(objectProps, &Item);   
    
  Item.lpszPropName = "Subfunction A";
  PropGrid_AddItem(objectProps, &Item);

  Item.lpszPropName = "Subfunction B";
  PropGrid_AddItem(objectProps, &Item);

  Item.lpszPropName = "0xF0";
  PropGrid_AddItem(objectProps, &Item);
  
  Item.lpszPropName = "Some event value";
  PropGrid_AddItem(objectProps, &Item); 
  
  Item.lpszPropName = "0xF8";
  PropGrid_AddItem(objectProps, &Item);
  
  Item.lpszPropName = "Invoker";
  PropGrid_AddItem(objectProps, &Item);
   
  Item.lpszPropName = "0x100";
  PropGrid_AddItem(objectProps, &Item);
  
  Item.lpszPropName = "Animation Counter";
  PropGrid_AddItem(objectProps, &Item);
  
  Item.lpszPropName = "Animation Sequence";
  PropGrid_AddItem(objectProps, &Item);
  
  Item.lpszPropName = "Animation Frame";
  PropGrid_AddItem(objectProps, &Item);  
  
  PropGrid_ExpandAllCatalogs(objectProps);
  //TODO:: more fields....
}


void object_window::updateObjectProperties(object *obj)
{
  PROPGRIDITEM Item;
  PropGrid_ItemInit(Item);

  Item.lpszCatalog = "Object Properties";
  
  Item.lpszPropName = "Type"; //Static text
  sprintf(objectProperties[0], "%i", obj->type);
  Item.lpCurValue = (LPARAM)objectProperties[0]; //Depends on ItemType
  Item.iItemType = PIT_STATIC;
  PropGrid_SetItemData(objectProps, 0, &Item);
  
  Item.lpszPropName = "Subtype";
  sprintf(objectProperties[1], "%i", obj->subtype);
  Item.lpCurValue = (LPARAM)objectProperties[1];
  PropGrid_SetItemData(objectProps, 1, &Item);
  
  Item.lpszPropName = "Bounding Box";
  sprintf(objectProperties[2], "(%i, %i, %i)-(%i, %i, %i)", obj->bound.xyz1.X, obj->bound.xyz1.Y, obj->bound.xyz1.Z, obj->bound.xyz2.X, obj->bound.xyz2.Y, obj->bound.xyz2.Z);
  Item.lpCurValue = (LPARAM)objectProperties[2];
  PropGrid_SetItemData(objectProps, 2, &Item);
  
  Item.lpszPropName = "GOOL Entry";
  if (obj->global && (unsigned long)obj->global != 0xBAADF00D)
    getEIDstring(objectProperties[3], obj->global->EID);
  else
    sprintf(objectProperties[3], "NULL EID");
    
  Item.lpCurValue = (LPARAM)objectProperties[3];
  PropGrid_SetItemData(objectProps, 3, &Item);
  
  Item.lpszPropName = "External GOOL Entry";
  if (obj->external && (unsigned long)obj->external != 0xBAADF00D)
    getEIDstring(objectProperties[4], obj->external->EID);
  else
    sprintf(objectProperties[4], "NULL EID");
  Item.lpCurValue = (LPARAM)objectProperties[4];
  
  PropGrid_SetItemData(objectProps, 4, &Item);
  
  Item.lpszPropName = "Zone";
  if (obj->zone != 0 && (unsigned long)obj->zone != 0xBAADF00D)
    getEIDstring(objectProperties[5], obj->zone->EID);
  else
    sprintf(objectProperties[5], "NULL EID");
    
  Item.lpCurValue = (LPARAM)objectProperties[5];
  PropGrid_SetItemData(objectProps, 5, &Item);
  
  Item.lpszPropName = "Routine";
  sprintf(objectProperties[6], "test%i", obj->routine);
  Item.lpCurValue = (LPARAM)objectProperties[6];
  PropGrid_SetItemData(objectProps, 6, &Item);
  
  
  
  /*Item.lpszCatalog = "Object Colors";
  Item.lpszPropName = "Light Matrix Column 1";
  PropGrid_AddItem(hPropGrid, &Item);

  Item.lpszPropName = "Light Matrix Column 2";
  PropGrid_AddItem(hPropGrid, &Item);

  Item.lpszPropName = "Light Matrix Column 3";
  PropGrid_AddItem(hPropGrid, &Item);

  Item.lpszPropName = "Light Color";
  Item.iItemType = PIT_COLOR;
  PropGrid_AddItem(hPropGrid, &Item);
  
  Item.lpszPropName = "Color Matrix Column 1";
  PropGrid_AddItem(hPropGrid, &Item);
  
  Item.lpszPropName = "Color Matrix Column 2";
  PropGrid_AddItem(hPropGrid, &Item);
  
  Item.lpszPropName = "Color Matrix Column 3";
  PropGrid_AddItem(hPropGrid, &Item);
 
  Item.lpszPropName = "Color Intensity";
  PropGrid_AddItem(hPropGrid, &Item);
  
  Item.lpszPropName = "Object Process";
  Item.iItemType = PIT_CATALOG;
  PropGrid_AddItem(hPropGrid, &Item);
  
  //Item.lpszCatalog = "Object Process";
  //Item.lpszPropName = "Links";
  //PropGrid_SetItemData(hPropGrid, &Item);
  
  Item.lpszCatalog = "Links";  
  Item.iItemType = PIT_STATIC;
  
  Item.lpszPropName = "Self";
  if (obj->process.self && (unsigned long)obj->process.self != 0xBAADF00D)
    sprintf(objectProperties[7], "Object %i", getObjectIndex(obj->process.self));
  else
    sprintf(objectProperties[7], "None");
  Item.lpCurValue = (LPARAM)objectProperties[7];
  PropGrid_SetItemData(objectProps, 15, &Item);
  
  Item.lpszPropName = "Parent";
  if (obj->process.parent && (unsigned long)obj->process.parent != 0xBAADF00D)
    sprintf(objectProperties[8], "Object %i", getObjectIndex(obj->process.parent));
  else
    sprintf(objectProperties[8], "None");
  
  Item.lpCurValue = (LPARAM)objectProperties[8];
  PropGrid_SetItemData(objectProps, 16, &Item);
  
  Item.lpszPropName = "Sibling";
  if (obj->process.sibling && (unsigned long)obj->process.sibling != 0xBAADF00D)
    sprintf(objectProperties[9], "Object %i", getObjectIndex(obj->process.sibling));
  else
    sprintf(objectProperties[9], "None");
  
  Item.lpCurValue = (LPARAM)objectProperties[9];
  PropGrid_SetItemData(objectProps, 17, &Item);

  Item.lpszPropName = "Children";
  if (obj->process.children && (unsigned long)obj->process.children != 0xBAADF00D)
    sprintf(objectProperties[10], "Object %i", getObjectIndex(obj->process.children));
  else
    sprintf(objectProperties[10], "None");
  
  Item.lpCurValue = (LPARAM)objectProperties[10];
  PropGrid_SetItemData(objectProps, 18, &Item);
  
  Item.lpszPropName = "Link A";
  if (obj->process.link[0] && (unsigned long)obj->process.link[0] != 0xBAADF00D)
    sprintf(objectProperties[11], "Object %i", getObjectIndex(obj->process.link[0]));
  else
    sprintf(objectProperties[11], "None");
  
  Item.lpCurValue = (LPARAM)objectProperties[11];
  PropGrid_SetItemData(objectProps, 19, &Item);
  
  Item.lpszPropName = "Link B";
  if (obj->process.link[1] && (unsigned long)obj->process.link[1] != 0xBAADF00D)
    sprintf(objectProperties[12], "Object %i", getObjectIndex(obj->process.link[1]));
  else
    sprintf(objectProperties[12], "None");
  
  Item.lpCurValue = (LPARAM)objectProperties[12];
  PropGrid_SetItemData(objectProps, 20, &Item);
  
  Item.lpszPropName = "Link C";
  if (obj->process.link[2] && (unsigned long)obj->process.link[2] != 0xBAADF00D)
    sprintf(objectProperties[13], "Object %i", getObjectIndex(obj->process.link[2]));
  else
    sprintf(objectProperties[13], "None");
  
  Item.lpCurValue = (LPARAM)objectProperties[13];
  PropGrid_SetItemData(objectProps, 21, &Item); 
  
  Item.lpszPropName = "Link D";
  if (obj->process.link[3] && (unsigned long)obj->process.link[3] != 0xBAADF00D)
    sprintf(objectProperties[14], "Object %i", getObjectIndex(obj->process.link[3]));
  else
    sprintf(objectProperties[14], "None");
  
  Item.lpCurValue = (LPARAM)objectProperties[14];
  PropGrid_SetItemData(objectProps, 22, &Item);

  Item.lpszCatalog = "Vectors";
  
  Item.lpszPropName = "Translate";
  sprintf(objectProperties[15], "(%i, %i, %i)", obj->process.vectors.trans.X, obj->process.vectors.trans.Y, obj->process.vectors.trans.Z);
  Item.lpCurValue = (LPARAM)objectProperties[15];
  PropGrid_SetItemData(objectProps, 23, &Item);
 
  Item.lpszPropName = "Rotate";
  sprintf(objectProperties[16], "(%i, %i, %i)", obj->process.vectors.rot.X, obj->process.vectors.rot.Y, obj->process.vectors.rot.Z);
  Item.lpCurValue = (LPARAM)objectProperties[16];
  PropGrid_SetItemData(objectProps, 24, &Item);

  Item.lpszPropName = "Scale";
  sprintf(objectProperties[17], "(%i, %i, %i)", obj->process.vectors.scale.X, obj->process.vectors.scale.Y, obj->process.vectors.scale.Z);
  Item.lpCurValue = (LPARAM)objectProperties[17];
  PropGrid_SetItemData(objectProps, 25, &Item);

  Item.lpszPropName = "Misc A";
  sprintf(objectProperties[18], "(%i, %i, %i)", obj->process.vectors.miscA.X, obj->process.vectors.miscA.Y, obj->process.vectors.miscA.Z);
  Item.lpCurValue = (LPARAM)objectProperties[18];
  PropGrid_SetItemData(objectProps, 26, &Item);
  
  Item.lpszPropName = "Misc B";
  sprintf(objectProperties[19], "(%i, %i, %i)", obj->process.vectors.miscB.X, obj->process.vectors.miscB.Y, obj->process.vectors.miscB.Z);
  Item.lpCurValue = (LPARAM)objectProperties[19];
  PropGrid_SetItemData(objectProps, 27, &Item);
  
  Item.lpszPropName = "Misc C";
  sprintf(objectProperties[20], "(%i, %i, %i)", obj->process.vectors.miscC.X, obj->process.vectors.miscC.Y, obj->process.vectors.miscC.Z);
  Item.lpCurValue = (LPARAM)objectProperties[20];
  PropGrid_SetItemData(objectProps, 28, &Item);
  
  Item.lpszCatalog = "Object Process";
  
  Item.lpszPropName = "Status A";
  sprintf(objectProperties[21], "%x", obj->process.statusA);
  Item.lpCurValue = (LPARAM)objectProperties[21];
  PropGrid_SetItemData(objectProps, 29, &Item);
  
  Item.lpszPropName = "Status B";
  sprintf(objectProperties[22], "%x", obj->process.statusB);
  Item.lpCurValue = (LPARAM)objectProperties[22];
  PropGrid_SetItemData(objectProps, 30, &Item);
  
  Item.lpszPropName = "Status C";
  sprintf(objectProperties[23], "%x", obj->process.statusC);
  Item.lpCurValue = (LPARAM)objectProperties[23];
  PropGrid_SetItemData(objectProps, 31, &Item);

  Item.lpszPropName = "Initial Subroutine";
  sprintf(objectProperties[24], "%x", obj->process.initSubIndex);
  Item.lpCurValue = (LPARAM)objectProperties[24];
  PropGrid_SetItemData(objectProps, 32, &Item);
  
  Item.lpszPropName = "PID/Flags";
  sprintf(objectProperties[25], "%x", obj->process.PIDflags);
  Item.lpCurValue = (LPARAM)objectProperties[25];
  PropGrid_SetItemData(objectProps, 33, &Item);

  Item.lpszPropName = "Stack Pointer";
  sprintf(objectProperties[26], "%x", getSP(obj));
  Item.lpCurValue = (LPARAM)objectProperties[26];
  PropGrid_SetItemData(objectProps, 34, &Item);

  Item.lpszPropName = "Program Counter";
  sprintf(objectProperties[27], "%x", (unsigned char*)obj->process.pc - (unsigned char*)obj->global->itemData[1]);
  Item.lpCurValue = (LPARAM)objectProperties[27];
  PropGrid_SetItemData(objectProps, 35, &Item);

  Item.lpszPropName = "Frame Pointer";
  sprintf(objectProperties[28], "%x", getFP(obj));
  Item.lpCurValue = (LPARAM)objectProperties[28];
  PropGrid_SetItemData(objectProps, 36, &Item);  
    
  Item.lpszPropName = "Subfunction A";
  sprintf(objectProperties[29], "%x", obj->process.subFuncA);
  Item.lpCurValue = (LPARAM)objectProperties[29];
  PropGrid_SetItemData(objectProps, 37, &Item);  

  Item.lpszPropName = "Subfunction B";
  sprintf(objectProperties[30], "%x", obj->process.subFuncB);
  Item.lpCurValue = (LPARAM)objectProperties[30];
  PropGrid_SetItemData(objectProps, 38, &Item);  
 
  Item.lpszPropName = "0xF0";
  sprintf(objectProperties[31], "%x", obj->process.F0);
  Item.lpCurValue = (LPARAM)objectProperties[31];
  PropGrid_SetItemData(objectProps, 39, &Item);  
  
  Item.lpszPropName = "Some event value";
  sprintf(objectProperties[32], "%i", obj->process.miscChild);
  Item.lpCurValue = (LPARAM)objectProperties[32];
  PropGrid_SetItemData(objectProps, 40, &Item);  
  
  Item.lpszPropName = "0xF8";
  sprintf(objectProperties[33], "%x", obj->process.F8);
  Item.lpCurValue = (LPARAM)objectProperties[33];
  PropGrid_SetItemData(objectProps, 41, &Item); 
  
  Item.lpszPropName = "Invoker";
  if (obj->process.invoker && (unsigned long)obj->process.invoker != 0xBAADF00D)
    sprintf(objectProperties[34], "Object %i", getObjectIndex(obj->process.invoker));
  else
    sprintf(objectProperties[34], "None");
  Item.lpCurValue = (LPARAM)objectProperties[34];
  PropGrid_SetItemData(objectProps, 42, &Item); 
   
  Item.lpszPropName = "Init Timestamp";
  sprintf(objectProperties[35], "%x", obj->process.initStamp);
  Item.lpCurValue = (LPARAM)objectProperties[35];
  PropGrid_SetItemData(objectProps, 43, &Item); 
  
  Item.lpszPropName = "Animation Counter";
  sprintf(objectProperties[36], "%i", obj->process.aniCounter);
  Item.lpCurValue = (LPARAM)objectProperties[36];
  PropGrid_SetItemData(objectProps, 44, &Item); 
  
  Item.lpszPropName = "Animation Sequence";
  sprintf(objectProperties[37], "%x", obj->process.aniSeq);
  Item.lpCurValue = (LPARAM)objectProperties[37];
  PropGrid_SetItemData(objectProps, 45, &Item); 
  
  Item.lpszPropName = "Animation Frame";
  sprintf(objectProperties[38], "%i", obj->process.aniFrame);
  Item.lpCurValue = (LPARAM)objectProperties[38];
  PropGrid_SetItemData(objectProps, 46, &Item);  
}
  */