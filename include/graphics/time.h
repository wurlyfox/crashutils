#ifndef TIME_H
#define TIME_H

  //33.8688 MHz = 33868800 cycles/sec
//       ^-1  = 0.0000000295256991 secs/cycle
//  *1000000  = 0.0295256991 microseconds/cycle
//        *8  = 0.2362055933 microseconds/8 cycles
//     *4096  = 967.49811036 microseconds/crash counter increment
//       ^-1  = 0.0010335938 increments/microsecond
//                ^ use as scale value to initTime

//3.75 nanoseconds = 1/8th tick psx; rate of counter increment

#define BASE_MICROSECONDS 1000000

class time
{
  private:
 
  unsigned long frameRateHz, frameRateMs;

  unsigned long ticks_init, ticks_cur, ticks_prev;
  float time_scale;
  
  //LARGE_INTEGER time_freq;
  //LARGE_INTEGER time_init, time_query;

  public:
  
  void initTime(unsigned long Hz, float scale);
  void setFrameRate(unsigned long Hz);

  unsigned long getTime();
  unsigned long getElapsedTime(bool scale);

  void frameCap();
};

#endif