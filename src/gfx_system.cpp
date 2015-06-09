#include "gfx_system.h"

void context_editor::postMessage(csystem *src, int msg, param lparam=0, param rparam=0)
{
  messageRouter(src, msg, lparam, rparam);
}

void context_editor::messageRouter(csystem *src, int msg, param lparam, param rparam)
{
  context_plugin_editor::messageRouter(msg, lparam, rparam);
  
  if (src != parentSystem)
    parentSystem->postMessage((csystem*)this, msg, lparam, rparam);
}

void gfx_system::messageRouter(csystem *src, int msg, param lparam, param rparam)
{
  csystem::messageRouter(src, msg, lparam, rparam);

  switch (msg)
  {
    // crashutils global messages
    case MSG_INIT_NSD: onInitNSD((NSD*)lparam, (NSF*)rparam); break;
  
    // graphics system local messages
    case GSM_INVALIDATE: if (renderer) { renderer->invalidate(); } break;
    case GSM_RENDER:     if (renderer) { renderer->draw(); }       break;
    case GSM_CLEAR_MODELS: if (renderer) { mainScene->clearModels(); } break;
    case GSM_GET_CONTEXT: onGetContext((context**)lparam, (int)rparam); break;
    case GSM_ALLOW_THREAD: mainContext->setActive(); break;
    case GSM_OBJMODEL:
    {
      param_model_obj *params = (param_model_obj*)lparam;
      mainScene->addModel(params->svtx, params->frame,
                         params->trans, params->rot, params->scale,
                         params->light, params->color,
                         params->back,  params->backIntensity);
    }
    break;
    case GSM_SPRMODEL:
    {
      param_model_spr *params = (param_model_spr*)lparam;
      mainScene->addModel(params->texEID, params->texInfoArray, 
                          params->scale,  params->trans);
    }
    break;
    case GSM_FRGMODEL:
    {
      param_model_frg *params = (param_model_frg*)lparam;
      mainScene->addModel(params->texEID, params->quadCount,
                          params->quads,  params->texInfoArray,
                          params->scale,  params->trans);
    }
  }
  
  if (editor)
  {
    context_editor *srcEditor = (context_editor*)src;
    if ((srcEditor != editor) && (srcEditor != overheadEditor))
      editor->postMessage(this, msg, lparam, rparam);
  }
}
  
void gfx_system::onInitNSD(NSD *nsd, NSF *nsf)
{
  // instantiate renderer
  renderer = new render_manager;
  
  // factory is responsible for creating the appropriate context 
  // and scene types
  postMessage(MSG_NEW_CONTEXT, (param)&mainContext);
  postMessage(MSG_NEW_CONTEXT, (param)&overheadContext);
  
  postMessage(MSG_NEW_SCENE, (param)&mainScene, 0);
  postMessage(MSG_NEW_SCENE_SHARED, (param)&overheadScene, (param)mainScene);
  
	// if factory does not create the appropriate context and scene types
	// default to normal types
  if (!mainContext)     { mainContext     = new context(); }
  if (!overheadContext) { overheadContext = new context(); }
  if (!mainScene)       { mainScene       = new scene(nsd, nsf); }
  if (!overheadScene)   { overheadScene   = new scene(mainScene, SCENE_SCENEFLAGS_SHAREDALLBUTCAM); }
     
	// set context scenes and add contexts to the renderer
  mainContext->setScene(mainScene);
  overheadContext->setScene(overheadScene);
  renderer->addContext(mainContext);
  renderer->addContext(overheadContext);
  
	// create an editor for both contexts
  editor = new context_editor(this, mainContext);
  overheadEditor = new context_editor(this, overheadContext);
  mainContext->usePlugins((context_plugin*)editor);
  overheadContext->usePlugins((context_plugin*)overheadEditor);
 
  // share contexts
  mainContext->share(overheadContext);
  
  camera *sceneCamera = mainScene->getCamera();
  sceneCamera->init(1, 0.8f, 0.04f, 0.08f);
  overheadContext->setProjection(1);
  
  overheadEditor->postMessage(this, CPM_CAMERA_SYNC, (param)mainContext);
}

void gfx_system::onGetContext(context **returnContext, int id)
{
  if      (id == 0) { *returnContext = mainContext; }
  else if (id == 1) { *returnContext = overheadContext; }
}

