#ifndef INTEG_H
#define INTEG_H

void crashSystemDraw();
entry *crashSystemPage(unsigned long EID);
unsigned long crashSystemControl();
unsigned long crashSystemTime();
void crashSystemZoneWorldModels(entry *zone);
void crashSystemObjectModel(entry *svtx, int frame,
                           cvector *trans, cangle *rot, cvector *scale,
                           cslightmatrix *light, cscolormatrix *color, cscolor *back, cscolor *backIntensity);
void crashSystemSpriteModel(unsigned long texEID, unsigned char *texInfoArray, int scale, cvector *trans);
void crashSystemFragmentModel(unsigned long texEID, int quadCount, csquad *quads,
                              unsigned char *texInfoArray, int scale, cvector *trans);
void crashSystemViewMatrix();
void crashSystemCameraUpdate();
void crashSystemHandleObjects();

#endif