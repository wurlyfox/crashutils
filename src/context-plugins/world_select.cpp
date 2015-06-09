#include "world_select.h"

void context_plugin_world_select::onPoint(int x, int y)
{
  // ensure all worlds are deselected
  int worldCount = geom->getWorldCount();
  for (int lp = 0; lp < worldCount; lp++)
  {
    model_wld *world = geom->getWorld(lp);
    world->deselect();
  }
  
  // select the world that contains the geometry we hover over
  if (parent->closestSelectable)
  {
    if (parent->closestSelectable->type == 2)
    {
      selectable *selected = parent->closestSelectable;
      
      int poly_vert_index = selected->vert_index % 3;
      int poly_index = selected->vert_index - poly_vert_index;
      
      model_wld *world = geom->getWorld(selected->item_index);
      world->select();
      
      //selectedWorld = world;
    }
  }
}