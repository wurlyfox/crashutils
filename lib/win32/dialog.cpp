#include "dialog.h"

void dialog::create(INT_PTR &out)
{
  void *mem = calloc(0x1000, 1);
  LPCDLGTEMPLATE dlgTemplate = DlgTemplate(mem);
  
  out = DialogBoxIndirectParam(hInstance, dlgTemplate, hParent, (DLGPROC)DlgProcRouter, (LPARAM)this);
  
  free(mem);
}

BOOL CALLBACK dialog::DlgProcRouter(HWND hWnd, INT uMsg, WPARAM wParam, LPARAM lParam)
{
  dialog *dlg;
  
  LONG_PTR ptr = GetWindowLongPtr(hWnd, GWLP_USERDATA);
  
  if (uMsg == WM_INITDIALOG && ptr == 0)
  {
    dlg = (dialog*)lParam;
    
    if (dlg)
    {    
      SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)dlg);
      dlg->hSelf = hWnd;  //:(
    }
  }
  else
    dlg = (dialog*)ptr; 
  
  if (dlg)
    return dlg->DlgProc(hWnd, uMsg, wParam, lParam);
  else
  {
    //return DefMDIChildProc(hWnd, uMsg, wParam, lParam);
    return FALSE; //??
  }
}

BOOL dialog::DlgProc(HWND hWnd, INT uMsg, WPARAM wParam, LPARAM lParam)
{ 
  switch (uMsg)
  {
    case WM_INITDIALOG: return onInitDialog(uMsg, wParam, lParam); break;
    case WM_CREATE:     return     onCreate(uMsg, wParam, lParam); break;
    case WM_COMMAND:    return    onCommand(uMsg, wParam, lParam); break;
    default:            return      onOther(uMsg, wParam, lParam); break;
  }
  
  //return DefMDIChildProc(hWnd, uMsg, wParam, lParam)
}