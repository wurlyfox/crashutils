#include "export.h"

NSF *export_file::nsf = 0;
NSD *export_file::nsd = 0;

void export_file::MD3(entry *svtx, char *fileName)
{
  model_obj_md3 *temp_model = new model_obj_md3;
  texture_buffer_md3 *temp_texbuf = new texture_buffer_md3(nsd, nsf);
  prim_alloc *temp_prims = new prim_alloc();
  
  temp_model->init(nsf, nsd);
  temp_model->exportMD3(svtx, temp_texbuf, temp_prims, fileName);

  delete temp_model;
  delete temp_texbuf;
  delete temp_prims;
}

void export_file::COLLADA(entry *wgeo, char *fileName)
{
  model_wld_collada *temp_model = new model_wld_collada;
  texture_buffer_img *temp_texbuf = new texture_buffer_img(nsd, nsf);
  prim_alloc *temp_prims = new prim_alloc();
        
  temp_model->exportDAE(wgeo, temp_texbuf, temp_prims, fileName);
        
  delete temp_model;
  delete temp_texbuf;
  delete temp_prims;
}
