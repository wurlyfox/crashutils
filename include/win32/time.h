#ifndef TIME_WIN32_H
#define TIME_WIN32_H

#include <windows.h>
#include "../time.h"

class time_win : public time
{
  private:
  
  LARGE_INTEGER start, query, elapsed, freq;
  
  public:
  
  void initTime();

  unsigned long getTime(bool scale);
  unsigned long getElapsedTime(bool scale);
};

#endif