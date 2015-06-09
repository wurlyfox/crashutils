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
  protected:
  
  unsigned long freqHz, freqMs;
  float timeScale;
  
  unsigned long ticksCur, ticksPrev;
  
  public:
  
  void init(unsigned long Hz, float scale)
	{
    setFrameRate(Hz);
    timeScale = scale;
  
    initTime();
  }

  void setFrameRate(unsigned long Hz)
  {
    freqHz = Hz;
    freqMs = BASE_MICROSECONDS / freqHz;
  }

  void frameCap()
  {
    unsigned long ticksElapsed;
  
    unsigned long ticksPrevA = ticksPrev;
    while (true)
    {
      ticksCur = getTime(false);
      ticksElapsed = ticksCur - ticksPrevA;

      if (ticksElapsed > freqMs)
        break;
    }
  
    ticksPrev = ticksCur;    
  }

  virtual void initTime() {};

  virtual unsigned long getTime(bool scale) {};
  virtual unsigned long getElapsedTime(bool scale) {};

};

#endif