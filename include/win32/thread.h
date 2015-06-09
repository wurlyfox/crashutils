#ifndef THREAD_WIN_H
#define THREAD_WIN_H

#include <windows.h>
#include "../thread.h"

class thread_win : public thread
{
  private:
  
  HANDLE hThread;
  int threadID;
  
  bool suspended;
  
  protected:
  static DWORD WINAPI threadRouter(void *params);
  
  virtual void run() { ext->run(); }
  
  public:
  
  thread_win();
  ~thread_win();
  
  void suspend();
  void resume();
};

#endif