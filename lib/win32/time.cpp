#include "time.h"

void time_win::initTime()
{
  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&start);
  start.QuadPart *= BASE_MICROSECONDS;
}

unsigned long time_win::getTime(bool scale)
{
  QueryPerformanceCounter(&query);
  query.QuadPart *= BASE_MICROSECONDS;
  query.QuadPart /= freq.QuadPart;
  
  unsigned long ticksTime = query.QuadPart;
  
  if (scale)
    ticksTime = (unsigned long)((float)ticksTime * timeScale);
  
  return ticksTime;
}

unsigned long time_win::getElapsedTime(bool scale)
{
  QueryPerformanceCounter(&query);
  query.QuadPart *= BASE_MICROSECONDS;
  query.QuadPart = (query.QuadPart - start.QuadPart);
  query.QuadPart /= freq.QuadPart;
  
  unsigned long elapsedTime = query.QuadPart;
  
  if (scale)
    elapsedTime = (unsigned long)((float)elapsedTime * timeScale);
    
  return elapsedTime;
}