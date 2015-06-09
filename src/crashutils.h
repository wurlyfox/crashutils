#ifndef CRASHUTILS_H
#define CRASHUTILS_H

#include <string.h>

#include "system.h"
#include "message.h"

#include "crash/nsf.h"
#include "crash/nsd.h"

class crashutils : public csystem
{
  protected:
  
  NSF *nsf;
  NSD *nsd;
  
  void messageRouter(csystem *src, int msg, param lparam, param rparam);

	private:
		
  // message handlers
  void onLoadNSD(char *fileName);
  void onSaveNSD(char *fileName);
  
  void onInitNSD(NSF *nsf, NSD *nsd);
  void onGetNSD(NSF **nsfA, NSD **nsdA);
  
	void onLookup(unsigned long EID, entry **result);
  void onSelect(entry *selectedEntry, int selItemIndex);
  void onExport(entry *exportEntry, char *filename);
	
  public:
  
  crashutils() {};
  crashutils(char *fileName) { postMessage(MSG_LOAD_NSD, (param)fileName); }
  
  ~crashutils() {};
	
	void start() { postMessage(MSG_START); }
};
  
#endif