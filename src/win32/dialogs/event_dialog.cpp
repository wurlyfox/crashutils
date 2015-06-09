#include "event_dialog.h"

LPWORD lpwAlign(LPWORD lpIn, ULONG dw2Power = 4)
{
    ULONG ul;

    ul = (ULONG)lpIn;
    ul += dw2Power-1;
    ul &= ~(dw2Power-1);
    return (LPWORD)ul;
}

LPCDLGTEMPLATE event_dialog::DlgTemplate(void *mem)
{
  LPDLGTEMPLATE lpdt;
  LPDLGITEMTEMPLATE lpdit;
  
  LPWORD lpw;
  LPWSTR lpwsz;
  int nchar;

  lpdt = (LPDLGTEMPLATE)mem;
  
  // Define a dialog box.
  lpdt->style = WS_POPUP | WS_BORDER | WS_SYSMENU | DS_MODALFRAME | DS_SETFONT | WS_CAPTION;
  lpdt->cdit = 14;         // Number of controls
  lpdt->x  = 10;  lpdt->y  = 10;
  lpdt->cx = 150; lpdt->cy = 200;

  lpw = (LPWORD)(lpdt + 1);
  *lpw++ = 0;             // No menu
  *lpw++ = 0;             // Predefined dialog box class (by default)

  lpwsz = (LPWSTR)lpw;
  nchar = MultiByteToWideChar(CP_ACP, 0, "Issue Event", -1, lpwsz, 50);
  lpw += nchar;

  *lpw++ = 9;  //9 pt font
  lpwsz = (LPWSTR)lpw;
  nchar = MultiByteToWideChar(CP_ACP, 0, "Segoe UI", -1, lpwsz, 50);
  lpw += nchar;
  
  //-----------------------
  // Define an OK button.
  //-----------------------
  lpw = lpwAlign(lpw);    // Align DLGITEMTEMPLATE on DWORD boundary
  lpdit = (LPDLGITEMTEMPLATE)lpw;
  lpdit->x  =  10; lpdit->y  = 170;
  lpdit->cx = 130;  lpdit->cy = 20;
  lpdit->id = IDOK;       // OK button identifier
  lpdit->style = WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON;
  
  lpw = (LPWORD)(lpdit + 1);
  *lpw++ = 0xFFFF;
  *lpw++ = 0x0080;        // Button class

  lpwsz = (LPWSTR)lpw;
  nchar = MultiByteToWideChar(CP_ACP, 0, "OK", -1, lpwsz, 50);
  lpw += nchar;
  *lpw++ = 0;             // No creation data

  //-----------------------
  // Define a static text control.
  //-----------------------
  lpw = lpwAlign(lpw);    // Align DLGITEMTEMPLATE on DWORD boundary
  lpdit = (LPDLGITEMTEMPLATE)lpw;
  lpdit->x  = 10; lpdit->y  = 10;
  lpdit->cx = 40; lpdit->cy = 10;
  lpdit->id = ID_TEXT;    // Text identifier
  lpdit->style = WS_CHILD | WS_VISIBLE | SS_LEFT;

  lpw = (LPWORD)(lpdit + 1);
  *lpw++ = 0xFFFF;
  *lpw++ = 0x0082;        // Static class

  lpwsz = (LPWSTR)lpw;
  nchar = MultiByteToWideChar(CP_ACP, 0, "Event:", -1, lpwsz, 150);
  lpw += nchar;
  *lpw++ = 0;             // No creation data

  //-----------------------
  // Define a list box control.
  //-----------------------
  lpw = lpwAlign(lpw);    // Align DLGITEMTEMPLATE on DWORD boundary
  lpdit = (LPDLGITEMTEMPLATE)lpw;
  lpdit->x  = 60; lpdit->y  = 10;
  lpdit->cx = 80; lpdit->cy = 80;
  lpdit->id = ID_EVENTBOX;    // Text identifier
  lpdit->style = WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | WS_VSCROLL | CBS_HASSTRINGS;

  lpw = (LPWORD)(lpdit + 1);
  *lpw++ = 0xFFFF;
  *lpw++ = 0x0085;        

  lpwsz = (LPWSTR)lpw;
  nchar = MultiByteToWideChar(CP_ACP, 0, "Event", -1, lpwsz, 150);
  lpw += nchar;
  *lpw++ = 0;             // No creation data
  
  //-----------------------
  // Define a list box control.
  //-----------------------
  lpw = lpwAlign(lpw);    // Align DLGITEMTEMPLATE on DWORD boundary
  lpdit = (LPDLGITEMTEMPLATE)lpw;
  lpdit->x  = 10; lpdit->y  = 30;
  lpdit->cx = 60; lpdit->cy = 80;
  lpdit->id = ID_EVENTSENDER;    // Text identifier
  lpdit->style = WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | WS_VSCROLL;

  lpw = (LPWORD)(lpdit + 1);
  *lpw++ = 0xFFFF;
  *lpw++ = 0x0085;        

  lpwsz = (LPWSTR)lpw;
  nchar = MultiByteToWideChar(CP_ACP, 0, "Sender", -1, lpwsz, 150);
  lpw += nchar;
  *lpw++ = 0;             // No creation data
  
  
  //-----------------------
  // Define a list box control.
  //-----------------------
  lpw = lpwAlign(lpw);    // Align DLGITEMTEMPLATE on DWORD boundary
  lpdit = (LPDLGITEMTEMPLATE)lpw;
  lpdit->x  = 80; lpdit->y  = 30;
  lpdit->cx = 60; lpdit->cy = 80;
  lpdit->id = ID_EVENTRECIPIENT;    // Text identifier
  lpdit->style = WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | WS_VSCROLL;

  lpw = (LPWORD)(lpdit + 1);
  *lpw++ = 0xFFFF;
  *lpw++ = 0x0085;        // List box class

  lpwsz = (LPWSTR)lpw;
  nchar = MultiByteToWideChar(CP_ACP, 0, "Recipient", -1, lpwsz, 150);
  lpw += nchar;
  *lpw++ = 0;             // No creation data
  
  //-----------------------
  // Define an edit control.
  //-----------------------
  lpw = lpwAlign(lpw);    // Align DLGITEMTEMPLATE on DWORD boundary
  lpdit = (LPDLGITEMTEMPLATE)lpw;
  lpdit->x  =  40; lpdit->y  = 65;
  lpdit->cx = 100; lpdit->cy = 12;
  lpdit->id = ID_EVENTARG1;    // Text identifier
  lpdit->style = WS_CHILD | WS_VISIBLE;
  lpdit->dwExtendedStyle = WS_EX_CLIENTEDGE;
  
  lpw = (LPWORD)(lpdit + 1);
  *lpw++ = 0xFFFF;
  *lpw++ = 0x0081;       

  lpwsz = (LPWSTR)lpw;
  nchar = MultiByteToWideChar(CP_ACP, 0, "Argument 1", -1, lpwsz, 150);
  lpw += nchar;
  *lpw++ = 0;             // No creation data
  
  //-----------------------
  // Define an edit control.
  //-----------------------
  lpw = lpwAlign(lpw);    // Align DLGITEMTEMPLATE on DWORD boundary
  lpdit = (LPDLGITEMTEMPLATE)lpw;
  lpdit->x  =  40; lpdit->y  = 80;
  lpdit->cx = 100; lpdit->cy = 12;
  lpdit->id = ID_EVENTARG2;    // Text identifier
  lpdit->style = WS_CHILD | WS_VISIBLE;
  lpdit->dwExtendedStyle = WS_EX_CLIENTEDGE;

  lpw = (LPWORD)(lpdit + 1);
  *lpw++ = 0xFFFF;
  *lpw++ = 0x0081;        // List box class

  lpwsz = (LPWSTR)lpw;
  nchar = MultiByteToWideChar(CP_ACP, 0, "Argument 2", -1, lpwsz, 150);
  lpw += nchar;
  *lpw++ = 0;             // No creation data
  
  //-----------------------
  // Define an edit control.
  //-----------------------
  lpw = lpwAlign(lpw);    // Align DLGITEMTEMPLATE on DWORD boundary
  lpdit = (LPDLGITEMTEMPLATE)lpw;
  lpdit->x  =  40; lpdit->y  = 95;
  lpdit->cx = 100; lpdit->cy = 12;
  lpdit->id = ID_EVENTARG3;    // Text identifier
  lpdit->style = WS_CHILD | WS_VISIBLE;
  lpdit->dwExtendedStyle = WS_EX_CLIENTEDGE;

  lpw = (LPWORD)(lpdit + 1);
  *lpw++ = 0xFFFF;
  *lpw++ = 0x0081;        // List box class

  lpwsz = (LPWSTR)lpw;
  nchar = MultiByteToWideChar(CP_ACP, 0, "Argument 3", -1, lpwsz, 150);
  lpw += nchar;
  *lpw++ = 0;             // No creation data
  
  //-----------------------
  // Define an edit control.
  //-----------------------
  lpw = lpwAlign(lpw);    // Align DLGITEMTEMPLATE on DWORD boundary
  lpdit = (LPDLGITEMTEMPLATE)lpw;
  lpdit->x  =  40; lpdit->y  = 110;
  lpdit->cx = 100; lpdit->cy =  12;
  lpdit->id = ID_EVENTARG4;    // Text identifier
  lpdit->style = WS_CHILD | WS_VISIBLE;
  lpdit->dwExtendedStyle = WS_EX_CLIENTEDGE;

  lpw = (LPWORD)(lpdit + 1);
  *lpw++ = 0xFFFF;
  *lpw++ = 0x0081;        // List box class

  lpwsz = (LPWSTR)lpw;
  nchar = MultiByteToWideChar(CP_ACP, 0, "Argument 4", -1, lpwsz, 150);
  lpw += nchar;
  *lpw++ = 0;             // No creation data
  
  //-----------------------
  // Define an edit control.
  //-----------------------
  lpw = lpwAlign(lpw);    // Align DLGITEMTEMPLATE on DWORD boundary
  lpdit = (LPDLGITEMTEMPLATE)lpw;
  lpdit->x  =   10; lpdit->y  = 50;
  lpdit->cx =  130; lpdit->cy = 12;
  lpdit->id = ID_RADIOARGN;    // Text identifier
  lpdit->style = WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON;
  
  lpw = (LPWORD)(lpdit + 1);
  *lpw++ = 0xFFFF;
  *lpw++ = 0x0080;       

  lpwsz = (LPWSTR)lpw;
  nchar = MultiByteToWideChar(CP_ACP, 0, "No Arguments", -1, lpwsz, 150);
  lpw += nchar;
  *lpw++ = 0;             // No creation data
  
  
  //-----------------------
  // Define an edit control.
  //-----------------------
  lpw = lpwAlign(lpw);    // Align DLGITEMTEMPLATE on DWORD boundary
  lpdit = (LPDLGITEMTEMPLATE)lpw;
  lpdit->x  =  10; lpdit->y  = 65;
  lpdit->cx =  30; lpdit->cy = 12;
  lpdit->id = ID_RADIOARG1;    // Text identifier
  lpdit->style = WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON;
  
  lpw = (LPWORD)(lpdit + 1);
  *lpw++ = 0xFFFF;
  *lpw++ = 0x0080;       

  lpwsz = (LPWSTR)lpw;
  nchar = MultiByteToWideChar(CP_ACP, 0, "Arg 1", -1, lpwsz, 150);
  lpw += nchar;
  *lpw++ = 0;             // No creation data
  
  //-----------------------
  // Define an edit control.
  //-----------------------
  lpw = lpwAlign(lpw);    // Align DLGITEMTEMPLATE on DWORD boundary
  lpdit = (LPDLGITEMTEMPLATE)lpw;
  lpdit->x  =  10; lpdit->y  = 80;
  lpdit->cx =  30; lpdit->cy = 12;
  lpdit->id = ID_RADIOARG2;    // Text identifier
  lpdit->style = WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON;

  lpw = (LPWORD)(lpdit + 1);
  *lpw++ = 0xFFFF;
  *lpw++ = 0x0080;        

  lpwsz = (LPWSTR)lpw;
  nchar = MultiByteToWideChar(CP_ACP, 0, "Arg 2", -1, lpwsz, 150);
  lpw += nchar;
  *lpw++ = 0;             // No creation data
  
  //-----------------------
  // Define an edit control.
  //-----------------------
  lpw = lpwAlign(lpw);    // Align DLGITEMTEMPLATE on DWORD boundary
  lpdit = (LPDLGITEMTEMPLATE)lpw;
  lpdit->x  =  10; lpdit->y  = 95;
  lpdit->cx =  30; lpdit->cy = 12;
  lpdit->id = ID_RADIOARG3;    // Text identifier
  lpdit->style = WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON;

  lpw = (LPWORD)(lpdit + 1);
  *lpw++ = 0xFFFF;
  *lpw++ = 0x0080;        

  lpwsz = (LPWSTR)lpw;
  nchar = MultiByteToWideChar(CP_ACP, 0, "Arg 3", -1, lpwsz, 150);
  lpw += nchar;
  *lpw++ = 0;             // No creation data
  
    //-----------------------
  // Define an edit control.
  //-----------------------
  lpw = lpwAlign(lpw);    // Align DLGITEMTEMPLATE on DWORD boundary
  lpdit = (LPDLGITEMTEMPLATE)lpw;
  lpdit->x  =  10; lpdit->y  =  110;
  lpdit->cx =  30; lpdit->cy =  12;
  lpdit->id = ID_RADIOARG4;    // Text identifier
  lpdit->style = WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON;

  lpw = (LPWORD)(lpdit + 1);
  *lpw++ = 0xFFFF;
  *lpw++ = 0x0080;        // List box class

  lpwsz = (LPWSTR)lpw;
  nchar = MultiByteToWideChar(CP_ACP, 0, "Arg 4", -1, lpwsz, 150);
  lpw += nchar;
  *lpw++ = 0;             // No creation data
  
  return (LPCDLGTEMPLATE)lpdt;
}
  
BOOL event_dialog::onInitDialog(INT uMsg, WPARAM wParam, LPARAM lParam)
{
  HWND eventList = GetDlgItem(hSelf, ID_EVENTBOX);
  
  char temp[50];
  for (int lp=0; lp<0x20; lp++)
  {
    sprintf(temp, "0x%02x00", lp);
    ComboBox_AddString(eventList, temp);
  }
  
  
  
  return TRUE;
}

BOOL event_dialog::onCreate(INT uMsg, WPARAM wParam, LPARAM lParam)
{
  return TRUE;
}

BOOL event_dialog::onCommand(INT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (LOWORD(wParam))
  {
    case IDOK:
    {      
      char event_string[7];
      GetDlgItemText(hSelf, ID_EVENTBOX, event_string, 7);
      
      char arg_string[4][50];
      GetDlgItemText(hSelf, ID_EVENTARG1, arg_string[0], 49);
      GetDlgItemText(hSelf, ID_EVENTARG2, arg_string[1], 49);
      GetDlgItemText(hSelf, ID_EVENTARG3, arg_string[2], 49);
      GetDlgItemText(hSelf, ID_EVENTARG4, arg_string[3], 49);      
      
      event->sender = 0;
      if (sscanf(event_string, "%x", &event->event))
      {
        bool success = true;
        for (int lp=0; lp<event->count && success; lp++)
        {
          if (arg_string[lp][0] == '0' && arg_string[lp][1] == 'x')
            success = sscanf(arg_string[lp], "%x", &event->args[lp]);
          else
            success = sscanf(arg_string[lp], "%i", &event->args[lp]);
        }
      
        if (success)
        {
          EndDialog(hSelf, (INT_PTR)event);
          return TRUE;
        }
      }
      
      MessageBox(NULL, "Please enter only numeric or hexadecimal arguments.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
      return FALSE;
    }
  }
  
  switch (HIWORD(wParam))
  {
    case BN_CLICKED:
    {
      switch (LOWORD(wParam))
      {
        case ID_RADIOARGN:
        
        Button_SetCheck(GetDlgItem(hSelf, ID_RADIOARGN), BST_CHECKED);
        Button_SetCheck(GetDlgItem(hSelf, ID_RADIOARG1), BST_UNCHECKED);
        Button_SetCheck(GetDlgItem(hSelf, ID_RADIOARG2), BST_UNCHECKED);
        Button_SetCheck(GetDlgItem(hSelf, ID_RADIOARG3), BST_UNCHECKED);
        Button_SetCheck(GetDlgItem(hSelf, ID_RADIOARG4), BST_UNCHECKED);
        
        Edit_Enable(GetDlgItem(hSelf, ID_EVENTARG1), false);
        Edit_Enable(GetDlgItem(hSelf, ID_EVENTARG2), false);
        Edit_Enable(GetDlgItem(hSelf, ID_EVENTARG3), false);
        Edit_Enable(GetDlgItem(hSelf, ID_EVENTARG4), false);
        
        event->count = 0;
        break;
        
        case ID_RADIOARG1:
        
        Button_SetCheck(GetDlgItem(hSelf, ID_RADIOARGN), BST_UNCHECKED);
        Button_SetCheck(GetDlgItem(hSelf, ID_RADIOARG1), BST_CHECKED);
        Button_SetCheck(GetDlgItem(hSelf, ID_RADIOARG2), BST_UNCHECKED);
        Button_SetCheck(GetDlgItem(hSelf, ID_RADIOARG3), BST_UNCHECKED);
        Button_SetCheck(GetDlgItem(hSelf, ID_RADIOARG4), BST_UNCHECKED);
        
        Edit_Enable(GetDlgItem(hSelf, ID_EVENTARG1), true);
        Edit_Enable(GetDlgItem(hSelf, ID_EVENTARG2), false);
        Edit_Enable(GetDlgItem(hSelf, ID_EVENTARG3), false);
        Edit_Enable(GetDlgItem(hSelf, ID_EVENTARG4), false);
        
        event->count = 1;
        break;
        
        case ID_RADIOARG2:
        
        Button_SetCheck(GetDlgItem(hSelf, ID_RADIOARGN), BST_UNCHECKED);
        Button_SetCheck(GetDlgItem(hSelf, ID_RADIOARG1), BST_CHECKED);
        Button_SetCheck(GetDlgItem(hSelf, ID_RADIOARG2), BST_CHECKED);
        Button_SetCheck(GetDlgItem(hSelf, ID_RADIOARG3), BST_UNCHECKED);
        Button_SetCheck(GetDlgItem(hSelf, ID_RADIOARG4), BST_UNCHECKED);
        
        Edit_Enable(GetDlgItem(hSelf, ID_EVENTARG1), true);
        Edit_Enable(GetDlgItem(hSelf, ID_EVENTARG2), true);
        Edit_Enable(GetDlgItem(hSelf, ID_EVENTARG3), false);
        Edit_Enable(GetDlgItem(hSelf, ID_EVENTARG4), false);
        
        event->count = 2;
        break;
        
        case ID_RADIOARG3:
        
        Button_SetCheck(GetDlgItem(hSelf, ID_RADIOARGN), BST_UNCHECKED);
        Button_SetCheck(GetDlgItem(hSelf, ID_RADIOARG1), BST_CHECKED);
        Button_SetCheck(GetDlgItem(hSelf, ID_RADIOARG2), BST_CHECKED);
        Button_SetCheck(GetDlgItem(hSelf, ID_RADIOARG3), BST_CHECKED);
        Button_SetCheck(GetDlgItem(hSelf, ID_RADIOARG4), BST_UNCHECKED);
        
        Edit_Enable(GetDlgItem(hSelf, ID_EVENTARG1), true);
        Edit_Enable(GetDlgItem(hSelf, ID_EVENTARG2), true);
        Edit_Enable(GetDlgItem(hSelf, ID_EVENTARG3), true);
        Edit_Enable(GetDlgItem(hSelf, ID_EVENTARG4), false);
        
        event->count = 3;
        break;
        
        case ID_RADIOARG4:
        
        Button_SetCheck(GetDlgItem(hSelf, ID_RADIOARGN), BST_UNCHECKED);
        Button_SetCheck(GetDlgItem(hSelf, ID_RADIOARG1), BST_CHECKED);
        Button_SetCheck(GetDlgItem(hSelf, ID_RADIOARG2), BST_CHECKED);
        Button_SetCheck(GetDlgItem(hSelf, ID_RADIOARG3), BST_CHECKED);
        Button_SetCheck(GetDlgItem(hSelf, ID_RADIOARG4), BST_CHECKED);
        
        Edit_Enable(GetDlgItem(hSelf, ID_EVENTARG1), true);
        Edit_Enable(GetDlgItem(hSelf, ID_EVENTARG2), true);
        Edit_Enable(GetDlgItem(hSelf, ID_EVENTARG3), true);
        Edit_Enable(GetDlgItem(hSelf, ID_EVENTARG4), true);
        
        event->count = 4;
        break;
      } 
    }   
  }     
}