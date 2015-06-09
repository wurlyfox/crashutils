#include <math.h>
#include "camera.h"
  
void camera::init(int modeA, float initRateRotA, float initRatePanA, float initRateMovA)
{
  mode        = modeA;
  nested      = 0;
  
  initRateRot = initRateRotA;
  initRatePan = initRatePanA;
  initRateMov = initRateMovA;
  
  resetRates();
  reset();
}

void camera::reset()
{
  location = { 0, 0, 0 };
  rotation = { 0, 0, 0 };
  
  syncLocation = 0;
  syncRotation = 0;
  
  rateRot = initRateRot;
  ratePan = initRatePan;
  rateMov = initRateMov;
}

void camera::resetRates()
{
  rateMovScale = 1;
  ratePanScale = 1;
  rateRotScale = 1;
}
  
void camera::updateRates(float add)
{
  updateRateMov(add);
  updateRatePan(add);
  updateRateRot(add);
}

void camera::updateRateMov(float add)
{
  rateMovScale += add;
  rateMov = initRateMov * rateMovScale;
}

void camera::updateRatePan(float add)
{
  ratePanScale += add;
  ratePan = initRatePan * ratePanScale;
}

void camera::updateRateRot(float add)
{  
  rateRotScale += add;
  rateRot = initRateRot * rateRotScale;
}

void camera::setMode(int modeA)
{
  if (mode == 1 && modeA == 0 || mode == 0 && modeA == 1)
  {
    rotation.X = -rotation.X;
    rotation.Y = -rotation.Y;
  }
  
  mode = modeA;
}

void camera::move(float amt)
{  
  amt *= rateMov;
  if (mode == 0)
    location.Z -= amt;
  else if (mode == 1)
  {
    const double radian = 3.14159265d/180;

    fvector displacement;
    
    displacement.Y =  sin(rotation.Y*radian) * amt;
    displacement.X = -sin(rotation.X*radian) * cos(rotation.Y*radian) * amt;
    displacement.Z =  cos(rotation.X*radian) * cos(rotation.Y*radian) * amt;
    
    location.X -= displacement.X;
    location.Y -= displacement.Y;
    location.Z -= displacement.Z;
  }
  else if (mode == 2)
  {
	  /*
    camProg += (scaleAmt*8);
    
    unsigned short sectDepth = getHword(currentSection, 0x1E, true);
    unsigned long sectDepthV = sectDepth << 8;
    
    signed long progressV = (signed long)(camProg * 256) & 0xFFFFFF00;
    if      (progressV < 0) { progressV = 0; camProg = 0; }
    else if (progressV > sectDepthV-256) { progressV = sectDepthV-256; camProg = sectDepth-1; }
    
    ccamera pathCam;
    pathCam.trans.X = (long)(location.X * 0x200) * 256;
    pathCam.trans.Y = (long)(location.Y * 0x200) * 256; 
    pathCam.trans.Z = (long)(location.Z * 0x200) * 256; 
    pathCam.rot.X = (rotation.X / 360) * 0x1000;
    pathCam.rot.Y = (rotation.Y / 360) * 0x1000;
    pathCam.rot.Z = (rotation.Z / 360) * 0x1000;
    cameraCalculate(currentSection, ((unsigned long)(camProg * 256)) & 0xFFFFFF00, &pathCam);
    location.X = ((((float)pathCam.trans.X) / 256) / 0x200);
    location.Y = ((((float)pathCam.trans.Y) / 256) / 0x200);
    location.Z = ((((float)pathCam.trans.Z) / 256) / 0x200);
    rotation.X = ((((float)pathCam.rot.X) / 0x1000) * 360);
    rotation.Y = ((((float)pathCam.rot.Y) / 0x1000) * 360);
    rotation.Z = ((((float)pathCam.rot.Z) / 0x1000) * 360);
		*/
  }    
}

void camera::panX(float amt)
{
  float scaleAmt = ratePan * amt;
  
  if (mode == 0)
    location.X += scaleAmt;
}

void camera::panY(float amt)
{
  float scaleAmt = ratePan * amt;
  
  if (mode == 0)
    location.Y += scaleAmt;
}

void camera::rotX(float amt)
{
  float scaleAmt = rateRot * amt;
  
  rotation.X += scaleAmt;
}

void camera::rotY(float amt)
{
  float scaleAmt = rateRot * amt;
  
  rotation.Y += scaleAmt;
}

void camera::rotZ(float amt)
{
  float scaleAmt = rateRot * amt;
  
  rotation.Z += scaleAmt;
}

fvector camera::getLocation(long X, long Y, long Z)
{
  fvector trans;
  trans.X = (((float)X) / 0x1000) * 8;
  trans.Y = (((float)Y) / 0x1000) * 8;
  trans.Z = (((float)Z) / 0x1000) * 8;
}

void camera::translate(long X, long Y, long Z)
{
  location.X += (((float)X) / 0x1000) * 8;
  location.Y += (((float)Y) / 0x1000) * 8;
  location.Z += (((float)Z) / 0x1000) * 8;
}
  
void camera::sync(vector *syncLocationA, vector *syncRotationA)
{
  syncLocation = syncLocationA;
  syncRotation = syncRotationA;
}

void camera::update()
{
  if (syncLocation)
  {
    location.X = ((((float)syncLocation->X) / 256) / 0x200);
    location.Y = ((((float)syncLocation->Y) / 256) / 0x200);
    location.Z = ((((float)syncLocation->Z) / 256) / 0x200);
  }
  
  if (syncRotation)
  {
    rotation.X = ((((float)syncRotation->X) / 0x1000) * 360);
    rotation.Y = ((((float)syncRotation->Y) / 0x1000) * 360);
    rotation.Z = ((((float)syncRotation->Z) / 0x1000) * 360);
  }
}