#include "polygon_select.h"

#include "comp_geom.h"

ftriangle *context_plugin_polygon_select::selections = 0;
int context_plugin_polygon_select::selectionCount = 0;

void context_plugin_polygon_select::onInit()
{
  parent = (context_plugin_select*)context_plugin::parent;
  
  //selectionCount = 0;
  if (!selections)
    selections = parent->selections->newTriangles(geom->primAlloc, 10);  
}

#ifdef GL_VERTEX_ARRAY_ENABLE

void context_plugin_polygon_select::onRender()
{
  int worldCount = geom->getWorldCount();
  for (int lp = 0; lp < worldCount; lp++)
  {
    model_wld *world = geom->getWorld(lp);
    
    fvolume worldRect;    
    worldRect.loc.X = world->bound.P1.X;
    worldRect.loc.Y = world->bound.P1.Y;
    worldRect.loc.Z = world->bound.P1.Z;
    worldRect.dim.W = world->bound.P2.X - world->bound.P1.X;
    worldRect.dim.H = world->bound.P2.Y - world->bound.P1.Y;
    worldRect.dim.D = world->bound.P2.Z - world->bound.P1.Z;
   
    dpoint worldRect3d[8];
    comp_geom::getVolumeVertices3d(&worldRect, worldRect3d);
    
    dpoint worldOutline2d[8];
    int outlinePointCount;
    comp_geom::projectOutline2d(parentContext, 
                                  worldRect3d, 8, 
                               worldOutline2d, outlinePointCount);
                                
    if (outlinePointCount)
    {
      for (int vlp = 0; vlp < world->polyCount*3; vlp++)
      {
        vertex *vert = &world->vertices[vlp];
        
        dpoint vertLoc = { vert->X+worldRect.loc.X, vert->Y+worldRect.loc.Y, vert->Z+worldRect.loc.Z };
        parent->recordSelectable(vertLoc, 2, lp, vlp);
      }
    }
  }
}

void context_plugin_polygon_select::onHover(int x, int y)
{
  if (parent->closestSelectable)
  {
    if (parent->closestSelectable->type == 2)
    {
      selectable *selected = parent->closestSelectable;
      
      int poly_vert_index = selected->vert_index % 3;
      int poly_index = selected->vert_index - poly_vert_index;
      
      
      model_wld *world = geom->getWorld(selected->item_index);
      
      float X = ((float)world->location.X / 0x1000) * 8;
      float Y = ((float)world->location.Y / 0x1000) * 8;
      float Z = ((float)world->location.Z / 0x1000) * 8;

      selections[0].A.X = world->vertices[poly_index+0].X + X;
      selections[0].A.Y = world->vertices[poly_index+0].Y + Y;
      selections[0].A.Z = world->vertices[poly_index+0].Z + Z;
      selections[0].B.X = world->vertices[poly_index+1].X + X;
      selections[0].B.Y = world->vertices[poly_index+1].Y + Y;
      selections[0].B.Z = world->vertices[poly_index+1].Z + Z;
      selections[0].C.X = world->vertices[poly_index+2].X + X;
      selections[0].C.Y = world->vertices[poly_index+2].Y + Y;
      selections[0].C.Z = world->vertices[poly_index+2].Z + Z;
      
      int sel = 1;
      unsigned long lowestZ = selected->z;
      
      if (selectable *cur = selected->next)
      {
        do
        {
          if (cur->z != lowestZ)
            break;
            
          poly_vert_index = cur->vert_index % 3;
          poly_index = cur->vert_index - poly_vert_index;
          
          selections[sel].A.X = world->vertices[poly_index+0].X + X;
          selections[sel].A.Y = world->vertices[poly_index+0].Y + Y;
          selections[sel].A.Z = world->vertices[poly_index+0].Z + Z;
          selections[sel].B.X = world->vertices[poly_index+1].X + X;
          selections[sel].B.Y = world->vertices[poly_index+1].Y + Y;
          selections[sel].B.Z = world->vertices[poly_index+1].Z + Z;
          selections[sel].C.X = world->vertices[poly_index+2].X + X;
          selections[sel].C.Y = world->vertices[poly_index+2].Y + Y;
          selections[sel].C.Z = world->vertices[poly_index+2].Z + Z;
               
          sel++;
        } while (cur = cur->next);
      }

      selectionCount = sel;    
    }
  }
  
  for (int lp=selectionCount; lp<10; lp++)
  {
    selections[lp].A.X = 0;
    selections[lp].A.Y = 0;
    selections[lp].A.Z = 0;
    selections[lp].B.X = 0;
    selections[lp].B.Y = 0;
    selections[lp].B.Z = 0;
    selections[lp].C.X = 0;
    selections[lp].C.Y = 0;
    selections[lp].C.Z = 0;
  }
}

#endif

