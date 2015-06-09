#include "context.h"

void context::render()
{
  if (contextScene->isReady())
  {
    draw();
    update();
  }
}

void context::setViewport(int width, int height)
{
  viewportWidth = width;
  viewportHeight = height;
}

void context::getViewport(int &width, int &height)
{
  width = viewportWidth; 
  height = viewportHeight; 
}

void context::usePlugins(context_plugin *newRoot)
{ 
  root = newRoot; 
	root->setContext(this);
  root->onInit();
}