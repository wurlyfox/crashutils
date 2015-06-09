#include "raw_controls/edit.h"
#include <stdio.h>

edit::edit(int editID, int x, int y, int w, int h, HWND _parent, char *text, DWORD dwStyle) :
raw_control(editID, WS_EX_CLIENTEDGE, WC_EDIT, text ? text : (char*)"", ES_LEFT | dwStyle, 
            x, y, w, h, _parent) 
{
  charValue  = 0;
  shortValue = 0;
  longValue  = 0;
  scale      = 1;
}

edit::~edit() 
{
  if (spin && !DestroyWindow(spin))
  {
    MessageBox(NULL, "Could Not Release spin control handle...", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
    spin = NULL;
  }
}

void edit::addSpin(int spinID, int min, int max)
{
  HINSTANCE hInstance = GetModuleHandle(NULL);

  spin = CreateWindowEx(0, UPDOWN_CLASS, "", 
    WS_CHILD | WS_VISIBLE |
    UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_NOTHOUSANDS, 
    0, 0, 0, 0,
    parent, (HMENU)spinID, hInstance, NULL);

  if (!spin)
    MessageBox(NULL, "Spin Creation Error.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
  else
  {
    SendMessage(spin, UDM_SETBUDDY, (WPARAM)hwnd, 0);
    SendMessage(spin, UDM_SETRANGE32, (WPARAM)min, (LPARAM)max);
  }
}

void edit::setSpinRange(int min, int max)
{
  SendMessage(spin, UDM_SETRANGE32, (WPARAM)min, (LPARAM)max);
}

void edit::useValue(char *_value, long _scale)
{
  charValue  = _value;
  shortValue = 0;
  longValue  = 0;  
  scale      = _scale;
  setValue(*charValue);
}

void edit::useValue(short *_value, long _scale)
{
  charValue  = 0;
  shortValue = _value;
  longValue  = 0;  
  scale      = _scale;
  setValue(*shortValue);
}

void edit::useValue(long *_value, long _scale)
{
  charValue  = 0;
  shortValue = 0;
  longValue  = _value;  
  scale      = _scale;
  setValue(*longValue);
}

long edit::getValue()
{
  bool unused;
  return getValue(unused);
}

long edit::getValue(bool &valid)
{
  valid = false;

  char text[30];
  GetDlgItemText(parent, ID, text, 30);
  
  int c=0;
  bool dot=false;
  char cur;
  while (cur = text[c++])
  {
    if (cur == '-')
    {
      if (c != 0)
      {
        valid = false;
        break;
      }
    }
    else if (cur == '.')
    {
      if (!dot)
        dot = true;
      else
      {
        valid = false;
        break;
      }
    }
    else if (cur >= 48 && cur <= 57)
      continue;
  }
  
  valid = (cur == 0);
  
  if (valid)
  {
    float pval = atof((const char*)text);
    
    long newVal;
    newVal = pval * scale;
    if (charValue)
    {
      *charValue = newVal;
      SendMessage(spin, UDM_SETPOS, 0, newVal);
    }
    else if (shortValue)
    {
      *shortValue = newVal;
      SendMessage(spin, UDM_SETPOS, 0, newVal);
    }
    else if (longValue)
    {
      *longValue = newVal;
      SendMessage(spin, UDM_SETPOS32, 0, newVal);
    }
    
    return newVal;
  }
 
  if (shortValue)
    return *shortValue;
  else if (longValue)
    return *longValue;
  else
    return 0;  
}

void edit::setValue(char newValue)
{  
  char temp[30];
  if (charValue)
  {
    char newCharValue = newValue;
    *charValue = newCharValue;
    
    if (scale == 1)
      sprintf(temp, "%i", newCharValue);
    else
      sprintf(temp, "%f", ((float)newCharValue)/scale);
    
    SendMessage(spin, UDM_SETPOS, 0, newCharValue);
  }
  else if (shortValue)
  {
    short newShortValue = newValue;
    *shortValue = newShortValue;
    
    if (scale == 1)
      sprintf(temp, "%i", newShortValue);
    else
      sprintf(temp, "%f", ((float)newShortValue)/scale);
    
    SendMessage(spin, UDM_SETPOS, 0, newShortValue);
  }
  else if (longValue)
  {
    long newLongValue = newValue;
    *longValue = newLongValue;
    
    if (scale == 1)
      sprintf(temp, "%i", newLongValue);
    else
      sprintf(temp, "%f", ((float)newLongValue)/scale);
    
    SendMessage(spin, UDM_SETPOS32, 0, newLongValue);
  }
  else
  {
    if (scale == 1)
      sprintf(temp, "%i", newValue);
    else
      sprintf(temp, "%f", ((float)newValue)/scale);
    
    SendMessage(spin, UDM_SETPOS32, 0, newValue);
  }
     
  SetDlgItemText(parent, ID, temp);
}

void edit::setValue(short newValue)
{
  char temp[30];
  if (charValue)
  {
    char newCharValue = newValue;
    *charValue = newCharValue;
    
    if (scale == 1)
      sprintf(temp, "%i", newCharValue);
    else
      sprintf(temp, "%f", ((float)newCharValue)/scale);
      
    SendMessage(spin, UDM_SETPOS, 0, newCharValue);
  }
  else if (shortValue)
  {
    short newShortValue = newValue;
    *shortValue = newShortValue;
    
    if (scale == 1)
      sprintf(temp, "%i", newShortValue);
    else
      sprintf(temp, "%f", ((float)newShortValue)/scale);
      
    SendMessage(spin, UDM_SETPOS, 0, newShortValue);
  }
  else if (longValue)
  {
    long newLongValue = newValue;
    *longValue = newLongValue;
    
    if (scale == 1)
      sprintf(temp, "%i", newLongValue);
    else
      sprintf(temp, "%f", ((float)newLongValue)/scale);
          
    SendMessage(spin, UDM_SETPOS32, 0, newLongValue);
  }
  else
  {
    if (scale == 1)
      sprintf(temp, "%i", newValue);
    else
      sprintf(temp, "%f", ((float)newValue)/scale);
     
    SendMessage(spin, UDM_SETPOS32, 0, newValue);
  }
     
  SetDlgItemText(parent, ID, temp);
}

void edit::setValue(long newValue)
{  
  char temp[30];
  if (charValue)
  {
    char newCharValue = newValue;
    *charValue = newCharValue;
    
    if (scale == 1)
      sprintf(temp, "%i", newCharValue);
    else
      sprintf(temp, "%f", ((float)newCharValue)/scale);
      
    SendMessage(spin, UDM_SETPOS, 0, newCharValue);
  }
  else if (shortValue)
  {
    short newShortValue = newValue;
    *shortValue = newShortValue;
    
    if (scale == 1)
      sprintf(temp, "%i", newShortValue);
    else
      sprintf(temp, "%f", ((float)newShortValue)/scale);
      
    SendMessage(spin, UDM_SETPOS, 0, newShortValue);
  }
  else if (longValue)
  {
    long newLongValue = newValue;
    *longValue = newLongValue;
    
    if (scale == 1)
      sprintf(temp, "%i", newLongValue);
    else
      sprintf(temp, "%f", ((float)newLongValue)/scale);
          
    SendMessage(spin, UDM_SETPOS32, 0, newLongValue);
  }
  else
  {
    if (scale == 1)
      sprintf(temp, "%i", newValue);
    else
      sprintf(temp, "%f", ((float)newValue)/scale);
     
    SendMessage(spin, UDM_SETPOS32, 0, newValue);
  }
     
  SetDlgItemText(parent, ID, temp);
}

void edit::handle(WPARAM wParam, LPARAM lParam)
{
  if (wParam)
  {
    if (LOWORD(wParam) == ID)
    {
      switch (HIWORD(wParam))
      {
        case EN_CHANGE:
        {
          bool valid;
          getValue(valid);
        }
      }
    }
  }
  
  if (lParam)
  {
    NMHDR* nmhdr = (NMHDR*)lParam;  
    if (nmhdr->code == UDN_DELTAPOS)
    {
      NMUPDOWN *udn = (NMUPDOWN*)lParam;          
    
      if (udn->hdr.hwndFrom == spin)
      {      
        long newValue;
        if (longValue)
          newValue = *longValue + udn->iDelta;
        else if (shortValue)
          newValue = *shortValue + udn->iDelta;
        else if (charValue)
          newValue = *charValue + udn->iDelta;
        else
          return;
          
        setValue(newValue);
      }
    }
  }
}

    