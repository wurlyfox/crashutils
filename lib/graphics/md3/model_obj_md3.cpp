#include <math.h>

#include "graphics/md3/model_obj_md3.h"

void model_obj_md3::exportMD3(entry *svtx, texture_buffer_md3 *texbuf, prim_alloc *prims, char *filename)
{
  int frameCount          =                   svtx->itemCount;
  unsigned char *buffer   =                 svtx->itemData[0];
  vertCount               =          getWord(buffer, 0, true);
  unsigned long tgeoEID   =          getWord(buffer, 4, true);
  unsigned long parentCID =           nsd->lookupCID(tgeoEID);
  chunk *parentChunk      =       nsf->lookupChunk(parentCID);
  entry *tgeo             = lookupEntry(tgeoEID, parentChunk);
  
  loadPolygons(tgeo, texbuf, prims);
  
  md3_header header;
  md3_frame frames[frameCount];
  md3_surface surface;  
  md3_triangle md3_triangles[polyCount];
  md3_texcoord md3_texcoords[vertCount];
  md3_vertex md3_vertices[frameCount][vertCount];

  sprintf(header.ident, "IDP3");
  header.version = 15;
  sprintf(header.name, "%s", filename);
  header.flags = 0;
  header.num_frames = frameCount;
  header.num_tags = 0;
  header.num_surfaces = 1;
  header.num_skins = 0;
  
  sprintf(surface.ident, "IDP3");
  getEIDstring(surface.name, svtx->EID);
  surface.flags = 0;
  surface.num_frames = frameCount;
  surface.num_shaders = 1;
  surface.num_verts = vertCount;
  surface.num_triangles = polyCount;
  
  for (int frm = 0; frm < frameCount; frm++)
  {
    loadVertices(svtx, frm, prims);
    
    for (int vrt = 0; vrt < vertCount; vrt++)
    {
      svector *coord = &md3_vertices[frm][vrt].coord;
      coord->X = vertices[vrt].X * 64;
      coord->Y = vertices[vrt].Y * 64;
      coord->Z = vertices[vrt].Z * 64;
      
      md3_normal *normal = &md3_vertices[frm][vrt].normal;
      normal->z = atan2(normals[vrt].Y, normals[vrt].X) * 255 / (2 * PI);
      normal->a = acos(normals[vrt].Z) * 255 / (2 * PI);
    }
    
    frames[frm].min_bounds = bound.P1;
    frames[frm].max_bounds = bound.P2;
    frames[frm].local_origin = { 0, 0, 0 }; //or is this the other point value in the svtx data
    
    float A = bound.P2.X - bound.P1.X;
    float B = bound.P2.Y - bound.P1.Y;
    float C = bound.P2.Z - bound.P2.Z;
    float radius = sqrt((A*A)+(B*B)+(C*C))/2;
    frames[frm].radius = radius;
    
    sprintf(frames[frm].name, "Frame %i", frm);
  }
  
  
  int it = 0;
  while (filename[it++] != 0);
  while (filename[--it] != '.');
  filename[it+1] = 't';
  filename[it+2] = 'g';
  filename[it+3] = 'a';
  filename[it+4] = 0;
  texbuf->exportTGA((const char*)filename);
  
  bakePolygonTextures(texbuf);

  for (int ply = 0; ply < polyCount; ply++)
  {
    unsigned long indexA = (unsigned long)(polygons[ply].vertexA - vertices);
    unsigned long indexB = (unsigned long)(polygons[ply].vertexB - vertices);
    unsigned long indexC = (unsigned long)(polygons[ply].vertexC - vertices);

    md3_triangles[ply].indexes[0] = indexA;
    md3_triangles[ply].indexes[1] = indexB;
    md3_triangles[ply].indexes[2] = indexC;
    
    md3_texcoords[ply].s = polygons[ply].tex->coords.A.X;
    md3_texcoords[ply].t = polygons[ply].tex->coords.A.Y;
  }

  md3_shader shader;
  sprintf(shader.name, "shader");
  shader.index = 0;
  
  unsigned long md3_start = 0;
  header.ofs_frames = md3_start + sizeof(header);
  header.ofs_tags = header.ofs_frames + sizeof(frames);
  header.ofs_surfaces = header.ofs_tags + 0; //0 = no tags
  
  unsigned long surface_start = header.ofs_surfaces;
  surface.ofs_shaders = surface_start + sizeof(surface);
  surface.ofs_triangles = surface.ofs_shaders + sizeof(shader);  
  surface.ofs_st = surface.ofs_triangles + sizeof(md3_triangles);
  surface.ofs_xyznormal = surface.ofs_st + sizeof(md3_texcoords);
  surface.ofs_end  = surface.ofs_xyznormal + sizeof(md3_vertices);
  
  header.ofs_eof = surface.ofs_end;

  
  it = 0;
  while (filename[it++] != 0);
  while (filename[--it] != '.');
  filename[it+1] = 'm';
  filename[it+2] = 'd';
  filename[it+3] = '3';
  filename[it+4] = 0;
  
  
  FILE *md3 = fopen((const char*)filename, "w+b");
  
  fwrite(&header, sizeof(md3_header), 1, md3);
  fwrite(&frames, sizeof(md3_frame), frameCount, md3);
  //fwrite(tags, sizeof(md3_tag), tagCount, md3);
  fwrite(&surface, sizeof(md3_surface), 1, md3);
  
  fwrite(&shader, sizeof(md3_shader), 1, md3);
  fwrite(&md3_triangles, sizeof(md3_triangle), polyCount, md3);
  fwrite(&md3_texcoords, sizeof(md3_texcoordquad), polyCount, md3);
  fwrite(&md3_vertices, sizeof(md3_vertex), vertCount * frameCount, md3);
  
  fclose(md3);
      
  it = 0;
  while (filename[it++] != 0);
  while (filename[--it] != '.');
  filename[it+1] = 't';
  filename[it+2] = 'g';
  filename[it+3] = 'a';
  filename[it+4] = 0;
  texbuf->exportTGA((const char*)filename);

}

void model_obj_md3::bakePolygonTextures(texture_buffer_md3 *texbuf)
{
  for (int lp = 0; lp < polyTexCount; lp++)
  {
    texture *tex = polygons[lp].tex;
    float Rf = polygons[lp].color.R;
    float Gf = polygons[lp].color.G;
    float Bf = polygons[lp].color.B;
    polygons[lp].tex = texbuf->bakeBlock(tex, Rf, Gf, Bf);
  }
  
  for (int lp = 0; lp < polyNonCount; lp++)
  {
    int index = polyTexCount + lp;
    
    texture *tex = polygons[index].tex;
    float Rf = polygons[index].color.R;
    float Gf = polygons[index].color.G;
    float Bf = polygons[index].color.B;
    polygons[index].tex = texbuf->colorBlock(Rf, Gf, Bf);
  }
  
  texbuf->rebuildBlocks();
  
  for (int lp = 0; lp < polyCount; lp++)
  {
    texture *tex = polygons[lp].tex;
    texbuf->updateCoords(tex);
  }
}    