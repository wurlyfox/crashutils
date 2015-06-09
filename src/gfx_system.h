#ifndef GFX_SYSTEM_H
#define GFX_SYSTEM_H

#include "system.h"
#include "message.h"

#include "graphics/render_manager.h"
#include "graphics/context.h"

#include "context-plugins/context_config.h"
#include "context-plugins/editor.h"

class context_editor : public context_plugin_editor
{
  private:
  
  csystem *parentSystem;
  
  void messageRouter(int msg, param lparam, param rparam) { messageRouter(0, msg, lparam, rparam); }
  void messageRouter(csystem *src, int msg, param lparam, param rparam);

  public:
  
  context_editor(csystem *p, context *ctxt) : context_plugin_editor(ctxt)
  {
    parentSystem = p;
  }
  
  void postMessage(csystem *src, int msg, param lparam, param rparam);
};

class gfx_system : public csystem
{
  private:
  
  render_manager *renderer;
  context *mainContext;
	context *overheadContext;
  scene *mainScene;
  scene *overheadScene;	
	
  context_editor *editor;
  context_editor *overheadEditor;
  
  void messageRouter(csystem *src, int msg, param lparam, param rparam);
  
	void onInitNSD(NSD *nsd, NSF *nsf);
  void onGetContext(context **returnContext, int id);
	
  public:
  
  gfx_system(csystem *par) 
  { 
    par->addChild(this);
    
    renderer = 0;
    mainContext = 0;
    overheadContext = 0;
    mainScene = 0;
    overheadScene = 0;
    
    editor = 0;
    overheadEditor = 0;
    postMessage(GSM_CREATE); 
  }
};

typedef struct
{
  entry *svtx;
	unsigned long frame;
	vector *trans;
	angle *rot;
	vector *scale;
	slightmatrix *light;
	scolormatrix *color;
	scolor *back;
	scolor *backIntensity;
} param_model_obj;

typedef struct
{
  unsigned long texEID;
	unsigned char *texInfoArray;
	unsigned long scale;
	vector *trans;
} param_model_spr;

typedef struct
{
  unsigned long texEID;
	unsigned long quadCount;
	squad2 *quads;
	unsigned char *texInfoArray;
	unsigned long scale;
	vector *trans;
} param_model_frg;

#endif