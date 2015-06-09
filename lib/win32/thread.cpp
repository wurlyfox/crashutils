#include "thread.h"

thread_win::thread_win()
{
  hThread = CreateThread(NULL, 0, threadRouter, (void*)this, 0, (PDWORD)&threadID);
  suspended = false;
}

thread_win::~thread_win()
{
  DWORD ExitCode;
  GetExitCodeThread(hThread, &ExitCode);
  TerminateThread(hThread, ExitCode);
}

DWORD WINAPI thread_win::threadRouter(void *params)
{
  thread_win *thisThread = (thread_win*)params;
  thisThread->run();
}

void thread_win::suspend()
{
  if (!suspended)
  {
    suspended = true;
    SuspendThread(hThread);
  }
}

void thread_win::resume()
{
  if (suspended)
  {
    ResumeThread(hThread);
    suspended = false;
  }
}