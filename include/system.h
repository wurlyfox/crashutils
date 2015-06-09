#ifndef SYSTEM_CR_H
#define SYSTEM_CR_H

// generic base class providing the framework for a 
// communicative software system; 

#define SYSTEM_MAX_CLIENT 10

#ifndef TYPE_PARAM
#define TYPE_PARAM
typedef unsigned long param;
#endif

class csystem
{
  protected:

  csystem *parent;
  csystem *sibling;
  csystem *children;
  
  virtual void messageRouter(csystem *src, int msg, param lparam, param rparam);
  
  public:
  
	csystem();
  virtual ~csystem();

  void addChild(csystem *child);
  void removeChild(csystem *child);
  csystem *getChild(int index);
  
  virtual void postMessage(int msg, param lparam=0, param rparam=0, bool filter=false);
  virtual void postMessage(csystem *src, int msg, param lparam=0, param rparam=0);
};

#endif