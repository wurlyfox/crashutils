#ifndef CONTEXT_WGL_H
#define CONTEXT_WGL_H

#include "win32/controls/context_win.h"
#include "graphics/gl/context_gl.h"
#include "graphics/gl/scene_gl.h"

class context_wgl : public context_win, public context_gl
{    
  protected:
  
  HANDLE hRenderThread;
  DWORD dRenderThread;
  HANDLE hRenderStartEvent;
  HANDLE hRenderFinishEvent;
  HANDLE hCreateEvent;
  HANDLE hRenderMutex;
  bool done;
  
  HGLRC hglrc;
  HGLRC thglrc[5];
  
  bool sceneInited;
  bool shared;
  bool request;
  bool releaseRequest;
  context_wgl *shareRequest;
  bool reacquireRequest;
  
  int tid;
  static int tlsIndex;  
  static int contextID;
  static int threadID;
  
  LRESULT onClick(UINT uMsg, WPARAM wParam, LPARAM lParam);  
  LRESULT onMouse(UINT uMsg, WPARAM wParam, LPARAM lParam);  
  LRESULT onPaint(UINT uMsg, WPARAM wParam, LPARAM lParam);
  
  int baseLists;
  
  public:
  
  static bool active;
  bool current;
  bool created;
  
  context_wgl() : context_win()
  {    
    current = false;
    created = false;
    
    request = false;
    releaseRequest = false;
    shareRequest = 0;
    reacquireRequest = false;
    
    for (int lp=0; lp<5; lp++)
      thglrc[lp] = 0;
      
    shared = false;
  }
  
  void renderThread();

  void create(DWORD dwExStyle, int x, int y, int width, int height, int id);
  void destroy();
  bool wglCreateContext();
  bool wglDeleteContext();
 
  void render();
  void update();
  bool setActive();
  
  int getThreadID();
  
  bool handleRequests();
  void release();
  void share(context *src);
  void reacquire();
  
  void InitFonts();
  void KillFonts();
  void writeText(const char *text);
};

static DWORD WINAPI renderThreadRouter(void* params);

#endif