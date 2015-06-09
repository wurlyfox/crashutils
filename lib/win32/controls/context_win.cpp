#include "controls/context_win.h"

void context_win::create(DWORD dwExStyle, int x, int y, int width, int height, int id)
{
  basic_control::create(dwExStyle, x, y, width, height, id);
  
  if (!(hDC = GetDC(hSelf)))   
  {
    destroy();
    MessageBox(NULL,"Can't Create A Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);				
  }
}    
    
void context_win::destroy()
{
  if (hDC)
  {
    if (!ReleaseDC(hSelf, hDC))
      MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
  }
  
  basic_control::destroy();
}

bool context_win::opengl()
{
  int PixelFormat;
  
  static PIXELFORMATDESCRIPTOR pfd = // pfd Tells Windows How We Want Things To Be
  {
		sizeof(PIXELFORMATDESCRIPTOR),	  // Size Of This Pixel Format Descriptor
		1,							                  // Version Number
		PFD_DRAW_TO_WINDOW |			        // Format Must Support Window
		PFD_SUPPORT_OPENGL |			        // Format Must Support OpenGL
		PFD_DOUBLEBUFFER,				          // Must Support Double Buffering
		PFD_TYPE_RGBA,					          // Request An RGBA Format
		16,					                      // Select Our Color Depth  (WE ALWAYS WANT 16 BIT RESOLUTION)
		0, 0, 0, 0, 0, 0,				          // Color Bits Ignored
		0,							                  // No Alpha Buffer
		0,							                  // Shift Bit Ignored
		0,							                  // No Accumulation Buffer
		0, 0, 0, 0,						            // Accumulation Bits Ignored
		16,							                  // 16Bit Z-Buffer (Depth Buffer)  
		0,							                  // No Stencil Buffer
		0,							                  // No Auxiliary Buffer
		PFD_MAIN_PLANE,					          // Main Drawing Layer
		0,							                  // Reserved
	  0, 0, 0							              // Layer Masks Ignored
  };
  
  if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd))) // Did Windows Find A Matching Pixel Format? 
  {
    destroy();	
    MessageBox(NULL,"Can't Find A Suitable PixelFormat.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
  
    return false;
  }

  if (!SetPixelFormat(hDC, PixelFormat, &pfd))
  {
    destroy();
    MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
  
    return false;
  }
  
  return true;
}

void context_win::SwapBuffers()
{
  ::SwapBuffers(hDC);
}

LRESULT context_win::onClick(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  SetFocus();
}