#include "crashutils.h"
#include "export.h"

void crashutils::messageRouter(csystem *src, int msg, param lparam, param rparam)
{
  csystem::messageRouter(src, msg, lparam, rparam);
  
  switch (msg)
  {
    case MSG_LOAD_NSD:     onLoadNSD((char*)lparam);        break;
    case MSG_SAVE_NSD:     /*...*/                          break;
    
    case MSG_INIT_NSD:     onInitNSD((NSF*)lparam, (NSD*)rparam); break;
    case MSG_GET_NSD:      onGetNSD((NSF**)lparam, (NSD**)rparam); break;
    
    case MSG_LOOKUP_ENTRY: onLookup((unsigned long)lparam, (entry**)rparam); break;
    case MSG_SELECT_ITEM:  onSelect((entry*)lparam, (int)rparam); break;
    case MSG_EXPORT_ITEM:  onExport((entry*)lparam, (char*)rparam); break;
  }
}

void crashutils::onLoadNSD(char *fileName)
{
  nsf = new NSF;
  nsd = new NSD;
  
  int nsdLen = strlen(fileName);

  fileName[nsdLen-1] = 'F';
  nsf->init(fileName);
      
  fileName[nsdLen-1] = 'D';
  nsd->init(fileName, nsf);
  
  postMessage(MSG_INIT_NSD, (param)nsf, (param)nsd);
}

void crashutils::onSaveNSD(char *fileName)
{
//...
}

void crashutils::onInitNSD(NSF *nsf, NSD *nsd)
{
  export_file::init(nsf, nsd);
}

void crashutils::onGetNSD(NSF **nsfA, NSD **nsdA)
{
  (*nsfA) = nsf;
  (*nsdA) = nsd;
}

void crashutils::onLookup(unsigned long EID, entry **result)
{
  unsigned long parentCID = nsd->lookupCID(EID);
  chunk *parentChunk      = nsf->lookupChunk(parentCID);
  *(result)               = lookupEntry(EID, parentChunk);
}

void crashutils::onSelect(entry *selectedEntry, int selItemIndex)
{
  //...
  switch (selectedEntry->type)
  {
    case 1:  postMessage(MSG_SELECT_SVTX, (param)selectedEntry, (param)selItemIndex); break;
    case 3:  postMessage(MSG_SELECT_WGEO, (param)selectedEntry);                      break;
    case 7:  postMessage(MSG_SELECT_ZDAT, (param)selectedEntry);                      break;
    case 11: postMessage(MSG_SELECT_GOOL, (param)selectedEntry);                      break;
  }
}

void crashutils::onExport(entry *exportEntry, char *fileName)
{
  switch (exportEntry->type)
  {
    case 1:  export_file::MD3(exportEntry, fileName); break;
    case 3:  export_file::COLLADA(exportEntry, fileName); break;
  }
}