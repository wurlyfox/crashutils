#include <iostream>
#include <stdio.h>
#include <gl/gl.h>

#include "context_wgl.h"

using namespace std;

int context_wgl::tlsIndex = -1;
int context_wgl::contextID = 0;
int context_wgl::threadID = 0;

bool context_wgl::active = false;

void context_wgl::create(DWORD dwExStyle, int x, int y, int width, int height, int id)
{
  context_win::create(dwExStyle, x, y, width, height, id);

  done = false;
    
  hCreateEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
  hRenderStartEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
  hRenderFinishEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
  hRenderMutex = CreateMutex(NULL, FALSE, NULL);
  hRenderThread = CreateThread(NULL, 0, renderThreadRouter, (void*)this, 0, &dRenderThread);
    
  WaitForSingleObject(hCreateEvent, 5000);
  CloseHandle(hCreateEvent); 

  if (!active)
  {
    setActive();
    active = true;
  }
  
  if (root)
    root->postMessage(CPM_INIT);
}

void context_wgl::destroy()
{
  done = true; 
  
  context_win::destroy();
}

void context_wgl::render()
{
  ResumeThread(hRenderThread);
  //SetEvent(hRenderStartEvent);
  if (shared) 
  {
    WaitForSingleObject(hRenderFinishEvent, 500); 
  }
}

void context_wgl::update()
{
  SwapBuffers();
}

LRESULT context_wgl::onClick(UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
  SetFocus();
  
  int x = GET_X_LPARAM(lParam); 
  int y = GET_Y_LPARAM(lParam); 

  if (root)
    root->postMessage(CPM_POINT, (param)x, (param)y);
}  

LRESULT context_wgl::onMouse(UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
  int x = GET_X_LPARAM(lParam); 
  int y = GET_Y_LPARAM(lParam); 

  if (root)
    root->postMessage(CPM_HOVER, (param)x, (param)y);
}  


LRESULT context_wgl::onPaint(UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
  render();
}  

bool context_wgl::wglCreateContext()
{
  // configure device context for opengl
  opengl();
  
  // try to create a rendering context 
  if (!(hglrc = ::wglCreateContext(hDC)))	
  {
    destroy();				
    MessageBox(NULL,"Can't Create GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);

    return false;
  }
 
  // set thread local context
  tid = getThreadID();
  thglrc[tid-1] = hglrc;
  
  // before we set active we have to create all the 
  // thread-specific contexts and share lists with all
  for (int lp = 0; lp < 5; lp++)
  {
    if (lp != tid-1)
    {
      if (!thglrc[lp])
      {
        if (!(thglrc[lp] = ::wglCreateContext(hDC)))
          MessageBox(NULL,"Can't Create Thread-Specific GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
        else
        {
          if (!wglShareLists(hglrc, thglrc[lp]))
            MessageBox(NULL,"Can't Share GL Display Lists.","ERROR",MB_OK|MB_ICONEXCLAMATION);
        }
      }
      else
      {
        if (!wglShareLists(hglrc, thglrc[lp]))
          MessageBox(NULL,"Can't Share GL Display Lists.","ERROR",MB_OK|MB_ICONEXCLAMATION);
      }
    }
  }
    
  if (!wglMakeCurrent(hDC, thglrc[tid-1]))
  {
    MessageBox(NULL, "Can't Activate GL Rendering Context.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
  
    return false;
  }
      
  current = true;
  
  // set viewport & appropriate perspective for rc
  RECT rcClient;
  GetClientRect(&rcClient);
  setViewport(rcClient.right, rcClient.bottom);
 
  // initialize the scene for this context 
  // (includes rebinding the textures)
  if (contextScene && !releaseRequest)
    contextScene->init();
    
  SetEvent(hCreateEvent);
  
  return true;
}

bool context_wgl::wglDeleteContext()
{
  current = false;

  if (hglrc)
  {
    if (!wglMakeCurrent(NULL, NULL))
    {
      MessageBox(NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
      return false;
    }
    if (!::wglDeleteContext(hglrc))
    {  
      MessageBox(NULL, "Release of Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
      return false;
    }      
  }
  else
    return false;
    
  return true;
}
    
// need this so the crash system thread can access the same display
// lists and texture/vram as the draw thread; else, any attempts to 
// load models in the context scene would fail at transferring
// pixel data for newly allocated textures to vram with GlTexSubImage2D

bool context_wgl::setActive()
{  
  unsigned long tidA = getThreadID();
  
  if (!(thglrc[tidA-1]))
    return false;
      
  if (!wglMakeCurrent(hDC, thglrc[tidA-1]))
  {
    MessageBox(NULL, "Can't Activate GL Rendering Context.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
  
    return false;
  }
  
  current = true;
  
  if (contextScene)
    contextScene->init();
    
  return true;
}

int context_wgl::getThreadID()
{  
  unsigned long tidA;
  if (tlsIndex == -1)
  {
    tlsIndex = TlsAlloc();
    TlsFree(tlsIndex);
    tlsIndex = TlsAlloc();
  }
  
  int *loc;  
  if (!(loc = (int*)TlsGetValue(tlsIndex)))
  {
    tidA = ++threadID;
      
    TlsSetValue(tlsIndex, &tidA);
  }
  else
    tidA = *loc;
    
  // because tls apparently does not zero the memory
  // leaving garbage pointers
  if (tidA < 0 || tidA > 10)
  {
    tidA = ++threadID;
    TlsSetValue(tlsIndex, &tidA);
  }
  
  return tidA;
}


static DWORD WINAPI renderThreadRouter(void* params)
{
  context_wgl *thisContext = (context_wgl*)params;
  thisContext->renderThread();
}
    
void context_wgl::renderThread()
{
  wglCreateContext();
    
  while (!done)
  {
    //WaitForSingleObject(hRenderStartEvent, INFINITE);
    SuspendThread(hRenderThread);
    if (!request || !handleRequests())
    {    
      if (contextScene->isReady())
      {
      //WaitForSingleObject(hDrawMutex, INFINITE);
      if (shared) { WaitForSingleObject(hRenderMutex, 500); }
      //cout << "Enter render in thread " << tid << endl;
      draw();
      update();
      //cout << "Exit render in thread " << tid << endl;
      if (shared) { ReleaseMutex(hRenderMutex); }

      }
    }
    if (shared) { SetEvent(hRenderFinishEvent); }
  }
  
  wglDeleteContext();
}

bool context_wgl::handleRequests()
{
  if (releaseRequest)
  {
    if (!wglMakeCurrent(NULL, NULL))
      MessageBox(NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
   
    current = false;
    releaseRequest = false;
  }
  if (shareRequest)
  {
    if (!current && !shareRequest->current)
    {
      if (!wglShareLists(hglrc, shareRequest->hglrc))
        MessageBox(NULL,"Can't Share GL Display Lists.","ERROR",MB_OK|MB_ICONEXCLAMATION);
        
      shareRequest->reacquire();
      reacquireRequest = true;
      
      shareRequest = 0;
    }
  }
  if (reacquireRequest)
  { 
    if (!wglMakeCurrent(hDC, hglrc))
      MessageBox(NULL, "Reacquiring Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);

    if (contextScene)
    {
      contextScene->init();
    }
           
    current = true;
    reacquireRequest = false;
    
    shared = true;
  }
  
  request = releaseRequest || shareRequest || reacquireRequest;
  return request;
}

void context_wgl::release()
{
  releaseRequest = true;
  
  request = true;
}

void context_wgl::share(context *src)
{
  context_wgl *source = (context_wgl*)src;
  
  release();
  source->release();
  shareRequest = source;
  
  request = true;
}

void context_wgl::reacquire()
{
  reacquireRequest = true;
  
  request = true;
}

void context_wgl::InitFonts()
{
  HFONT   font;                       // Windows Font ID
  HFONT   oldfont;                    // Used For Good House Keeping
  baseLists = glGenLists(96);   
  	
  font = CreateFont(      -15,  /* Height Of Font */
  	                        0,  /* Width Of Font */
					        0,  /* Angle Of Escapement */
                            0,  /* Orientation Angle */
                      FW_BOLD,  /* Font Weight */
                        FALSE,  /* Italic */
                        FALSE,  /* Underline */
                        FALSE,  /* Strikeout */
                 ANSI_CHARSET,  /* Character Set Identifier */
          CLIP_DEFAULT_PRECIS,  /* Clipping Precision */
		        OUT_TT_PRECIS,  /* Output Precision */
          ANTIALIASED_QUALITY,  /* Output Quality */
    FF_DONTCARE|DEFAULT_PITCH,  /* Family And Pitch */	   
                    "Fixedsys"); /* Font Name */
				
  oldfont = (HFONT)SelectObject(hDC, font);       // Selects The Font We Want
  wglUseFontBitmaps(hDC, 32, 96, baseLists);      // Builds 96 Characters Starting At Character 32
  SelectObject(hDC, oldfont);                     // Selects The Font We Want
  DeleteObject(font);                             // Delete The Font
}  

void context_wgl::KillFonts()                   
{
  glDeleteLists(baseLists, 96);
}

void context_wgl::writeText(const char *text)
{
  glPushAttrib(GL_LIST_BIT);                           // Pushes The Display List Bits 
  glListBase(baseLists - 32);                          // Sets The Base Character to 32
  glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);   // Draws The Display List Text
  glPopAttrib();                                       // Pops The Display List Bits
}
