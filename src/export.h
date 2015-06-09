#ifndef EXPORT_H
#define EXPORT_H

#include "crash/nsf.h"
#include "crash/nsd.h"

#include "graphics/md3/model_obj_md3.h"
#include "graphics/collada/model_wld_collada.h"

class export_file
{
  private:
  static NSF *nsf;
  static NSD *nsd;
  
  public:
  static void init(NSF *nsfA, NSD *nsdA) { nsf = nsfA; nsd = nsdA; }
  
  static void MD3(entry *svtx, char *fileName);
  static void COLLADA(entry *wgeo, char *fileName);
};

#endif