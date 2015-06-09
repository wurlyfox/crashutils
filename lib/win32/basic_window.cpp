#include "basic_window.h"

void basic_window::init(const char *_className, HINSTANCE _hInstance, HWND _hParent=0)
{
  hInstance = _hInstance;
  hParent   = _hParent;
 
  className = _className;
} 
  
bool basic_window::registerClass(const char *_className)
{
  className = _className;
  
  WNDCLASSEX wc;
  if (!GetClassInfoEx(hInstance, _className, &wc))
  {
    wc.cbSize        = sizeof(WNDCLASSEX); 
    wc.style		     = 0;	    // Redraw On Size, And Own DC For Window.
    wc.lpfnWndProc	 = (WNDPROC) WndProcRouter;			
    wc.cbClsExtra		 = 0;					                  // No Extra Window Data
    wc.cbWndExtra		 = 0;					                  // No Extra Window Data
    wc.hInstance		 = hInstance;				            // Set The Instance
    wc.hIcon		     = LoadIcon(NULL, IDI_WINLOGO); // Load The Default Icon
    wc.hCursor		   = LoadCursor(NULL, IDC_ARROW); // Load The Arrow Pointer
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);				
    wc.lpszMenuName	 = NULL;			     		          // We Don't Want A Menu
    wc.lpszClassName = className;    		            // Set The Class Name
    wc.hIconSm       = LoadIcon(NULL, IDI_WINLOGO);

    if (!RegisterClassEx(&wc))					            // Attempt To Register The Window Class
    {
      char error[50];
      sprintf(error, "Failed To Register %s Window Class.", className);
      
      MessageBox(NULL, error, "ERROR", MB_OK | MB_ICONEXCLAMATION);

      return false;	
    }
  }
  
  return true;
}

void basic_window::create(const char* title, int x, int y, int width, int height)
{
  registerClass(className);
 
  //...
}

void basic_window::destroy()
{
  //...

  if (!UnregisterClass(className, hInstance))	// Are We Able To Unregister Class
  {
    char error[50];
    sprintf(error, "Could Not Unregister %s Class...", className);
    
    MessageBox(NULL, error, "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
    hInstance = NULL;						             // Set hInstance To NULL
  }
}

bool basic_window::FileOpenBox(const char *filter, char *fileName, bool save)
{
  OPENFILENAME ofn;

  ZeroMemory(&ofn, sizeof(ofn));

  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = hSelf;
  ofn.lpstrFilter = filter;
  ofn.lpstrFile = fileName;
  ofn.nMaxFile = MAX_PATH;
  ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
  ofn.lpstrDefExt = "txt";

  if (save)
    return GetSaveFileName(&ofn);
  else
    return GetOpenFileName(&ofn);
}

void basic_window::SendMessageRoot(INT uMsg, WPARAM wParam, LPARAM lParam)
{
  window *root = this;
  
  while (root->parent)
    root = root->parent;
    
  basic_window *rootBasic = (basic_window*)root;
  rootBasic->SendMessage(uMsg, wParam, lParam);
}

void basic_window::PostMessageRoot(INT uMsg, WPARAM wParam, LPARAM lParam)
{
  window *root = this;
  
  while (root->parent)
    root = root->parent;
    
  basic_window *rootBasic = (basic_window*)root;
  rootBasic->PostMessage(uMsg, wParam, lParam);
}

void basic_window::BroadcastMessageRoot(INT uMsg, WPARAM wParam, LPARAM lParam)
{
  window *root = this;
  
  while (root->parent)
    root = root->parent;
    
  basic_window *rootBasic = (basic_window*)root;
  rootBasic->BroadcastMessage(uMsg, wParam, lParam);
}

void basic_window::BroadcastMessage(INT uMsg, WPARAM wParam, LPARAM lParam)
{
  window *curChild = children;
  
  while (curChild)
  {
    basic_window *curChildBasic = (basic_window*)curChild;
    curChildBasic->BroadcastMessage(uMsg, wParam, lParam);
    
    curChild = curChild->sibling;
  }
  
  SendMessage(uMsg, wParam, lParam);
}

LRESULT CALLBACK basic_window::WndProcRouter(HWND hWnd, INT uMsg, WPARAM wParam, LPARAM lParam)
{
  basic_window *win;
  
  LONG_PTR ptr = GetWindowLongPtr(hWnd, GWLP_USERDATA);
  
  if (uMsg == WM_NCCREATE && ptr == 0)
  {
    CREATESTRUCT *pCreate = (CREATESTRUCT*)lParam;
    
    if (pCreate->dwExStyle & WS_EX_MDICHILD)
    {
      MDICREATESTRUCT *mcs = (MDICREATESTRUCT*)pCreate->lpCreateParams;
      win = (basic_window*)mcs->lParam;
    }
    else
    {
      LPARAM cParam = (LPARAM)pCreate->lpCreateParams;
      win = (basic_window*)cParam;
    }
    
    if (win)
    {    
      SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)win);
      win->hSelf = hWnd;  //:(
    }
  }
  else
  {
    win = (basic_window*)ptr;
  }
  
  if (win)
    return win->WndProc(hWnd, uMsg, wParam, lParam);
  else
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT basic_window::WndProc(HWND hWnd, INT uMsg, WPARAM wParam, LPARAM lParam)
{ 
  switch (uMsg)
  {
    case WM_CREATE:      onCreate(uMsg, wParam, lParam); break;
    case WM_COMMAND:    onCommand(uMsg, wParam, lParam); break;
    case WM_NOTIFY:      onNotify(uMsg, wParam, lParam); break;
    case WM_PAINT:        onPaint(uMsg, wParam, lParam); break;
    case WM_LBUTTONDOWN:  onClick(uMsg, wParam, lParam); break;
    case WM_RBUTTONDOWN: onRClick(uMsg, wParam, lParam); break;
    case WM_MOUSEMOVE:    onMouse(uMsg, wParam, lParam); break;    
    case WM_DESTROY:    onDestroy(uMsg, wParam, lParam); break;
    
    case WM_CLOSE:					   
    {
      PostQuitMessage(0);			// send quit message
      return 0;					      // jump back
    }
    default:           
      onOther(uMsg, wParam, lParam); 
      break;
  } 
  
  return DefProc(hWnd, uMsg, wParam, lParam);
}

// generic control creation
HWND basic_window::createText(DWORD dwStyle, int x, int y, int w, int h, char *text)
{
  HWND control = CreateWindowEx(0, WC_STATIC, text,
    WS_CHILD | WS_VISIBLE | dwStyle, 
    x, y, w, h,
    hSelf, NULL, hInstance, NULL);
    
  HFONT fnt = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
  ::SendMessage(control, WM_SETFONT, (WPARAM)fnt, TRUE);
  
  return control;
}

HWND basic_window::createGroupBox(DWORD dwStyle, int x, int y, int w, int h, char *text)
{
  HWND control = CreateWindowEx(0, WC_BUTTON, text,
    WS_CHILD | WS_VISIBLE |  BS_GROUPBOX | dwStyle, 
    x, y, w, h,
    hSelf, NULL, hInstance, NULL);
    
  HFONT fnt = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
  ::SendMessage(control, WM_SETFONT, (WPARAM)fnt, TRUE);
  
  return control;
}

void basic_window::mouseMenu(HMENU menu)
{
  long mouse = GetMessagePos();
  short mouseX, mouseY;
  mouseX = GET_X_LPARAM(mouse);
  mouseY = GET_Y_LPARAM(mouse);
    
  TrackPopupMenu(menu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, mouseX, mouseY, 0, hSelf, NULL);
}