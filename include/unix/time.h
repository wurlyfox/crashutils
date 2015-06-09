#ifndef TIME_UNIX_H
#define TIME_UNIX_H

#include <sys/time.h>

class time_unix
{
  private:

  unsigned long ticksStart;

  public:
  
  void initTime();

  unsigned long getTime(bool scale);
  unsigned long getElapsedTime(bool scale);
};

#endif