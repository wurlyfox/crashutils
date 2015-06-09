#include "time.h"

void time_unix::initTime()
{
  ticksStart = getTime();
}

unsigned long time::getTime(bool scale)
{
  timeval tv;
  gettimeofday(&tv, 0);
  ticksCur = (tv.tv_sec * BASE_MICROSECONDS) + tv.tv_usec;
  
  if (scale)
    return (unsigned long)((float)ticksCur * timeScale);
  else
    return ticksCur;
}

unsigned long time::getElapsedTime(bool scale)
{
  timeval tv;
  gettimeofday(&tv, 0);
  ticksCur = (tv.tv_sec * BASE_MICROSECONDS) + tv.tv_usec;
  
  unsigned long elapsed = ticksCur - ticksStart;
  
  if (scale)
    elapsed = (unsigned long)((float)elapsed * timeScale);
    
  return elapsed;
}
    
    