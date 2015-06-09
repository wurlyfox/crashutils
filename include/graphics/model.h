#ifndef MODEL_H
#define MODEL_H

#include "types.h"
//#include "model_wld.h"
#include "model_obj.h"
#include "model_spr.h"
#include "model_frg.h"

class model
{
  public:
  
  int type;
  
  // geometry  
  union
  {
    model_obj *object_model;
    model_spr *sprite_model;
    model_frg *fragment_model;
  };
  
  // model matrix, local light matrix, local color matrix, local color
  fvector trans;
  fangle rot;
  fvector scale;
  flightmatrix lightMatrix;
  fcolormatrix colorMatrix;
  fcolor backColor;
  
  // reserved for software vertex shading; specifically mutated copies of geometry 
  //vertex *vertices;
  //fpoint2 *texcoords;
  //fvector *normals;
  fcolor *colors;
  
  virtual void draw() {};
  
  // implement via gfx API or hardware vertex shaders 
  virtual void orient() {};           
  //virtual void light();
  //virtual void texShader();
  
  // software vertex shaders: implement as software shaders
  //virtual void calcVertices(); // per-vertex position shader
  //virtual void calcTexcoords();// per-vertex texture coord
  virtual void calcColors();     // per-vertex color shader   
};

#endif