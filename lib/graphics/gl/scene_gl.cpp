#include <gl\gl.h>
#include <gl\glu.h>

#include "graphics/gl/scene_gl.h"

scene_gl::scene_gl(NSD *_nsd, NSF *_nsf) : scene(_nsd, _nsf)
{
  inited = false;
  
  geom = new geometry_gl(_nsd, _nsf);
  cam  = new camera_gl;
    
  for (int lp = 0; lp < MAX_ZONES; lp++)
    zoneBuffer[lp] = new zone_gl;
  for (int lp = 0; lp < MAX_MODELS; lp++)
    modelBuffer[lp] = new model_gl; 
}

scene_gl::scene_gl(scene *shared, unsigned long flags) : scene(shared, flags)
{    
  if (!(flags & SCENE_SCENEFLAGS_SHAREDGEOM))
  {
    //hold up on this...
    //geom = new geometry_gl(shared->nsd, shared->nsf);
  }
  
  if (!(flags & SCENE_SCENEFLAGS_SHAREDCAM))
  {
    cam = new camera_gl;
    cam->reset();
  }
  
  if (!(flags & SCENE_SCENEFLAGS_SHAREDZONES))
  {
    for (int lp = 0; lp < MAX_ZONES; lp++)
      zoneBuffer[lp] = new zone_gl;
  }
  
  if (!(flags & SCENE_SCENEFLAGS_SHAREDMODELS))
  {
    for (int lp = 0; lp < MAX_MODELS; lp++)
      modelBuffer[lp] = new model_gl; 
  }
} 
  
scene_gl::~scene_gl()
{
  delete geom;
  delete cam;
  
  for (int lp = 0; lp < MAX_ZONES; lp++)
    delete zoneBuffer[lp];
  for (int lp = 0; lp < MAX_MODELS; lp++)
    delete modelBuffer[lp]; 
}

void scene_gl::init()
{
  LightAmbient[0] = 1.0;
  LightAmbient[1] = 1.0;
  LightAmbient[2] = 1.0;
  LightAmbient[3] = 1.0;
  
  //glShadeModel(GL_SMOOTH);				                  // Enable Smooth Shading
  glClearColor(0.0f, 0.0f, 0.0f, 0.5f);		            // Black Background
  glClearDepth(1.0f);					                        // Depth Buffer Setup
  glEnable(GL_DEPTH_TEST);				                    // Enables Depth Testing
  glDepthFunc(GL_LEQUAL);				                      // The Type Of Depth Testing To Do
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);	// not GL_NICEST
  
  glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
  glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);
  glEnable(GL_LIGHT1);
  
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  
  geom->textureBuffer->init();
  
  *viewFlags = SCENE_VIEWFLAGS_ZONEVIEWBOUNDS;// | 
               //SCENE_VIEWFLAGS_ZONEVIEWSECTIONS;

  inited = true;              
}

void scene_gl::draw()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
  glLoadIdentity();					                    

  glMatrixMode(GL_MODELVIEW);
  
  if (*viewFlags & SCENE_VIEWFLAGS_VIEWWIREFRAME)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  else
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
  cam->view();
  
  scene::draw();
}