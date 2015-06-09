#ifndef TYPES_H
#define TYPES_H

#define GL_VERTEX_ARRAY_ENABLE

#define PI 3.14159265d
#define MAX_POLYGONS   0X1000
#define MAX_QUADS      0X200
#define MAX_VERTICES   0X2000
#define MAX_ZONES      0x40
#define MAX_MODELS     96      //max 8 wgeo models, 16 tgeo modelds
#define MAX_WGEO       8       //max 8 wgeo models
#define MAX_TGEO       16      //max 16 tgeo models
#define MAX_SECTIONS   8
#define MAX_COLLISIONS 0x4000

typedef struct
{
  float X;
  float Y;
  float Z;
  float Rf;
  float Gf;
  float Bf;
} vertex;

typedef struct
{
  int X;
  int Y;
  int Z;
} vector, point;

typedef struct
{
  int X;
  int Y;
} vector2, point2;

typedef struct
{
  float X;
  float Y;
  float Z;
} fvector, fpoint;

typedef struct
{
  double X;
  double Y;
  double Z;
} dvector, dpoint;

typedef struct
{
  float X;
	float Y;
} fvector2, fpoint2;

typedef struct
{
  double X;
  double Y;
} dvector2, dpoint2;

typedef struct
{
  signed short X;
  signed short Y;
  signed short Z;
} svector, spoint;

typedef struct
{
  signed short X;
  signed short Y;
} svector2, spoint2;

typedef struct
{
  spoint A;
  spoint B;
  spoint C;
  spoint D;
} squad;

typedef struct
{
  spoint2 A;
  spoint2 B;
  spoint2 C;
  spoint2 D;
} squad2;

typedef struct
{
  signed long Y;
  signed long X;
  signed long Z;
} angle;

typedef struct
{
  signed short Y;
  signed short X;
  signed short Z;
} sangle;

typedef struct
{
  float Y;
  float X;
  float Z;
} fangle;

typedef struct
{
  fpoint A;
  fpoint B;
} fline;

typedef struct
{
  fpoint2 A;
  fpoint2 B;
} fline2;

typedef struct
{
  fpoint A;
  fpoint B;
  fpoint C;
} ftriangle;

typedef struct
{
  fpoint2 A;
  fpoint2 B;
  fpoint2 C;
} ftriangle2;

typedef struct
{
  fpoint A;
  fpoint B;
  fpoint C;
  fpoint D;
} fquad;

typedef struct
{
  fpoint2 A;
  fpoint2 B;
  fpoint2 C;
  fpoint2 D;
} fquad2;

typedef struct
{
  unsigned long R;
  unsigned long G;
  unsigned long B;
} color;

typedef struct
{
  signed short R;
  signed short G;
  signed short B;
} scolor;

typedef struct
{
  float R;
  float G;
  float B;
} fcolor;

typedef struct
{
  vector V1;
  vector V2;
  vector V3;
} matrix, lightmatrix;

typedef struct
{
  svector V1;
  svector V2;
  svector V3;
} smatrix, slightmatrix;

typedef struct
{
  scolor C1;
  scolor C2;
  scolor C3;
} scolormatrix;

typedef struct
{
  fvector V1;
  fvector V2;
  fvector V3;
} fmatrix, flightmatrix;

typedef struct
{
  fcolor C1;
  fcolor C2;
  fcolor C3;
} fcolormatrix;

typedef struct
{
  unsigned short W;
  unsigned short H;
  unsigned short D;
} dimension;

typedef struct
{
  float W;
  float H;
  float D;
} fdimension;

typedef struct
{
  point P1;
  point P2;
} region;

typedef struct
{
  fpoint P1;
  fpoint P2;
} fregion;

typedef struct
{
  point loc;
  dimension dim;
} volume;

typedef struct
{
  fpoint loc;
  fdimension dim;
} fvolume;

typedef struct
{
  union
  {
    fquad2 coords;
    void *info;
  };
} texture;

typedef struct
{
  vertex *vertexA;
  vertex *vertexB;
} line;

typedef struct
{
  vertex *vertexA;
  vertex *vertexB;
  vertex *vertexC;
  fvector *normalA;
  fvector *normalB;
  fvector *normalC;
  fcolor color;
  texture *tex;
  bool textured;  
} polygon;

typedef struct
{
  vertex *vertexA;
  vertex *vertexB;
  vertex *vertexC;
  vertex *vertexD;
  texture *tex;
  bool textured;
} quad;

#endif