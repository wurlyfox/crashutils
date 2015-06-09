#include "render_manager.h"

void render_manager::invalidate()
{
  render = true;
}

bool render_manager::draw()
{
  if (render || wait)
  {
    for (int lp = 0; lp < contextCount; lp++)
      renderContext[lp]->render();
      
    drawCount++;
    
    // handle contexts' plugins
    for (int lp = 0; lp < contextCount; lp++)
    {
      context_plugin *plugins = renderContext[lp]->getPlugins();
      if (plugins)
        plugins->postMessage(CPM_RENDER);
    }
    
    return true;
  }

  return false;
}

void render_manager::drawSync()
{
  wait = true;
  draw();
}