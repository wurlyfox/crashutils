#ifndef IMMEDIATE_H
#define IMMEDIATE_H

#include "graphics/types.h"
//#include "../../../src/crash/src/goolengine.h"   //for cspace (can't we cast this to a space?)

//void drawSelection();
void drawPath(vector loc, point P1, point P2, int scale);
void drawRect3(region reg, point loc, int scale);
void drawRect3(region reg, int scale);
void drawRect3f(fregion reg);
//void drawCSpace(cspace *space);

#endif






