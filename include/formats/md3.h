#ifndef MD3_H
#define MD3_H

#include "graphics/types.h"

typedef fvector vec3_t;

struct md3_header
{
  char ident[4];
  unsigned long version;
  char name[64];
  unsigned long flags;
  unsigned long num_frames;
  unsigned long num_tags;
  unsigned long num_surfaces;
  unsigned long num_skins;
  unsigned long ofs_frames;
  unsigned long ofs_tags;
  unsigned long ofs_surfaces;
  unsigned long ofs_eof;
};

struct md3_frame
{
  vec3_t min_bounds;
  vec3_t max_bounds;
  vec3_t local_origin;
  float radius;
  char name[16];
};

struct md3_tag
{
  char name[64];
  fvector origin;
  fmatrix axis;
};

struct md3_surface
{
  char ident[4];
  char name[64];
  unsigned long flags;
  unsigned long num_frames;
  unsigned long num_shaders;
  unsigned long num_verts;
  unsigned long num_triangles;
  unsigned long ofs_triangles;
  unsigned long ofs_shaders;
  unsigned long ofs_st;
  unsigned long ofs_xyznormal;
  unsigned long ofs_end;
};

struct md3_shader
{
  char name[64];
  unsigned long index;
};

struct md3_triangle
{
  unsigned long indexes[3];
};

struct md3_texcoord
{
  float s;
  float t;
};

struct md3_texcoordquad
{
  md3_texcoord coord[4];
};  

struct md3_normal
{
  unsigned char z;
  unsigned char a;
};

struct md3_vertex
{
  svector coord;
  md3_normal normal;
};

#endif