#ifndef CAMERA_H
#define CAMERA_H

#include "types.h"

class camera
{
  protected:
  
  int mode;
  camera *nested;
  
  private:
  
  vector *syncLocation;
  vector *syncRotation;

  float initRateMov;
  float initRatePan;
  float initRateRot;
  float rateMov;
  float ratePan;
  float rateRot;
  float rateMovScale;
  float ratePanScale;
  float rateRotScale;
  
  public:
 
  fvector location;
  fvector rotation;
  
  camera()
  {
    init(0, 1, 1, 1);
  }
  camera(camera *cam)
  {
    init(cam->mode, cam->initRateRot, cam->initRatePan, cam->initRateMov);
  }
  ~camera()
  {
    if (nested)
      delete nested;
  }
  
  void init(int modeN, float initRateRotA, float initRatePanA, float initRateMovA);
  void reset();
  
  void resetRates();
  void updateRates(float add);
  void updateRateMov(float add);
  void updateRatePan(float add);
  void updateRateRot(float add);

  int  getMode()           { return mode; }
  void setMode(int modeN); 
  
  void move(float amt);
  void panX(float amt);
  void panY(float amt);
  void rotX(float amt);
  void rotY(float amt);
  void rotZ(float amt);
  
  static fvector getLocation(long X, long Y, long Z);
  void translate(long X, long Y, long Z);

  void sync(vector *syncLocationA, vector *syncRotationA);
  void update();

  camera *getNested()                 { return nested; }
  void *setNested(camera *newNested)  { nested = newNested; }
  
  virtual void view() {};
  virtual camera *newCamera() { return new camera(this); }
};

#endif