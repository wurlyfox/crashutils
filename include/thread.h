#ifndef THREAD_H
#define THREAD_H

// for operating system-specific threads: 
//   - subclass thread and implement according to OS API
//   - specify a call to ext->run() within implementation of run()
//
//   - threads can now be created by subclassing thread, provided that
//     1) the subclass constructor calls create with a new instance of
//        the appropriate OS specific implementation (also a subclass 
//        of thread)
//

class thread
{
  protected:
    
  thread *ext;
  void create(thread *impl)
  {
    ext      = impl;
    ext->ext = this;
  }
  void destroy()
  {
    delete ext;
  }
  public:
  
  virtual void run() {}; 

  virtual void suspend() { ext->suspend(); }
  virtual void resume() { ext->resume(); }

};

#endif
