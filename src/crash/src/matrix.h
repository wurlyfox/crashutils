#include "goolengine.h"
#include "objectengine.h"

void MulMatrix(csmatrix *left, csmatrix *right);
void ScaleMatrix(csmatrix *left, cvector *right);
void transform(cvector *input, cvector *trans, cangle *rot, cvector *scale, cvector *output);
void transform(cvector *input, csmatrix *matrix, cvector *output);

unsigned long apxDist(cvector *vectA, cvector *vectB);
unsigned long eucDist(cvector *vectA, cvector *vectB);
unsigned long eucDistXZ(cvector *vectA, cvector *vectB);

signed long angleXZ(cvector *vectA, cvector *vectB);
signed long angleXY(cvector *vectA, cvector *vectB);
