#include <stdio.h>
#include <string.h>

#include "graphics/collada/model_wld_collada.h"

void model_wld_collada::exportDAE(entry *wgeo, texture_buffer_img *texbuf, prim_alloc *prims, const char *filename)
{
  load(wgeo, texbuf, prims);
  
  char EIDString[6];
  getEIDstring(EIDString, wgeo->EID);

  char tgaName[200];
  sprintf(tgaName, "%s\0", filename);
  
  int ext = ((unsigned long)strrchr(tgaName, '.')-(unsigned long)tgaName);

  if ((ext > 0) ||
      (ext = (strlen(tgaName))))
  {
    tgaName[ext+0] = '.';
    tgaName[ext+1] = 't';
    tgaName[ext+2] = 'g';
    tgaName[ext+3] = 'a';
    tgaName[ext+4] = 0;
  }
  
  FILE *dae = fopen(filename, "w+");
  
  fprintf(dae, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
  fprintf(dae, "<COLLADA xmlns=\"http://www.collada.org/2005/11/COLLADASchema\" version=\"1.4.1\">\n");
  
  fprintf(dae, "\t<library_images>\n");
  fprintf(dae, "\t\t<image"
                   " id=\"%s-map-image\""
                   " name=\"%s-map-image\">\n",
                   EIDString, EIDString);
  fprintf(dae, "\t\t\t<init_from>%s</init_from>\n", tgaName);
  fprintf(dae, "\t\t</image>\n");
  fprintf(dae, "\t</library_images>\n");
          
  fprintf(dae, "\t<library_materials>\n");
  fprintf(dae, "\t\t<material id=\"Material-material\" name=\"Material\"/>\n");
  fprintf(dae, "\t</library_materials>\n");
                                    
  fprintf(dae, "\t<library_geometries>\n");
  fprintf(dae, "\t\t<geometry id=\"%s-mesh\"" 
                            " name=\"%s\">\n", 
                            EIDString, EIDString);
  fprintf(dae, "\t\t\t<mesh>\n");
  
  mesh_source sources[] = { {    /*vertCount*/polyCount*3, "positions", 3, (float*)&vertices[0], "XYZ", 3, 0 },
                             //{   vertCount,  "normals", 3, (float*)&vertices[0], "XYZ", 0, 3 },
                            {    /*vertCount*/polyCount*3,    "colors", 3, (float*)&vertices[0], "RGB", 3, 3 },
                            {                 polyCount*3,       "map", 2,                    0,  "ST", 2, 0 } };
                           
  for (int src=0; src<3; src++)
  {
    fprintf(dae, "\t\t\t\t<source"
                         " id=\"%s-mesh-%s\">\n", 
                         EIDString, sources[src].name);                    
    fprintf(dae, "\t\t\t\t\t<float_array"
                           " id=\"%s-mesh-%s-array\""
                           " count=\"%i\">", 
                           EIDString, sources[src].name, sources[src].count*sources[src].stride);
  
    /*if (src != 2)
    {
      float *source = sources[src].source;
      for (int lp=0; lp<sources[src].count; lp++)
      {
        int base = (lp*6) + sources[src].offset;
        fprintf(dae, "%f %f %f ", source[base+1], source[base+0], source[base+2]);  
      }
    } */
    
    if (src == 0)
    {
      for (int lp=0; lp<polyCount; lp++)
      {
        vertex *coords = &vertices[(lp*3)];
        fprintf(dae, "%f %f %f ", coords->X, coords->Y, coords->Z);
        coords = &vertices[(lp*3)+1];
        fprintf(dae, "%f %f %f ", coords->X, coords->Y, coords->Z);
        coords = &vertices[(lp*3)+2];
        fprintf(dae, "%f %f %f ", coords->X, coords->Y, coords->Z);
      }
    }
    else if (src == 1)
    {
      for (int lp=0; lp<polyCount; lp++)
      {
        vertex *colors = &vertices[(lp*3)];
        fprintf(dae, "%f %f %f ", colors->Rf, colors->Gf, colors->Bf);
        colors = &vertices[(lp*3)+1];
        fprintf(dae, "%f %f %f ", colors->Rf, colors->Gf, colors->Bf);
        colors = &vertices[(lp*3)+2];
        fprintf(dae, "%f %f %f ", colors->Rf, colors->Gf, colors->Bf);
      }
    }   
    else
    {
      for (int lp=0; lp<polyTexCount; lp++)
      {
        fpoint2 *coords = &texcoords[(lp*3)];
        fprintf(dae, "%f %f ", coords->X, coords->Y);
        coords = &texcoords[(lp*3)+1];
        fprintf(dae, "%f %f ", coords->X, coords->Y);
        coords = &texcoords[(lp*3)+2];
        fprintf(dae, "%f %f ", coords->X, coords->Y);
      }
      for (int lp=0; lp<polyNonCount; lp++)
      {
        fprintf(dae, "0 0 0 0 0 0 ");
      } 
    }
    
    fprintf(dae, "</float_array>\n");
    fprintf(dae, "\t\t\t\t\t<technique_common>\n");
    fprintf(dae, "\t\t\t\t\t\t<accessor"
                             " source=\"#%s-mesh-%s-array\""
                             " count=\"%i\""
                             " stride=\"%i\">\n",
                             EIDString, sources[src].name, sources[src].count, sources[src].stride);
    
    for (int lp=0; lp<sources[src].paramCount; lp++)
    {
      fprintf(dae, "\t\t\t\t\t\t\t<param" 
                                 " name=\"%c\""
                                 " type=\"float\"/>\n",
                                 sources[src].params[lp]);
    }

    fprintf(dae, "\t\t\t\t\t\t</accessor>\n");
    fprintf(dae, "\t\t\t\t\t</technique_common>\n");
    fprintf(dae, "\t\t\t\t</source>\n");
  } 
  
  fprintf(dae, "\t\t\t\t<vertices id=\"%s-mesh-vertices\">\n", EIDString);
  fprintf(dae, "\t\t\t\t\t<input" 
                         " semantic=\"POSITION\""
                         " source=\"#%s-mesh-positions\"/>\n",
                         EIDString);
  fprintf(dae, "\t\t\t\t\t<input" 
                         " semantic=\"COLOR\""
                         " source=\"#%s-mesh-colors\"/>\n",
                         EIDString);         
  fprintf(dae, "\t\t\t\t</vertices>\n");
  
  fprintf(dae, "\t\t\t\t<triangles" 
                       " material=\"Material\""
                       " count=\"%i\">\n",
                       polyCount);
  fprintf(dae, "\t\t\t\t\t<input"
                         " semantic=\"VERTEX\""
                         " source=\"#%s-mesh-vertices\""
                         " offset=\"0\"/>\n",
                         EIDString);
  fprintf(dae, "\t\t\t\t\t<input"
                         " semantic=\"TEXCOORD\""
                         " source=\"#%s-mesh-map\""
                         " offset=\"0\""
                         " set=\"0\"/>\n",
                         EIDString);
  
  fprintf(dae, "\t\t\t\t\t<p>");
  for (int ply=0; ply<polyCount; ply++)
  { 
    //unsigned long indexA = (unsigned long)(polygons[ply].vertexA - vertices);
    //unsigned long indexB = (unsigned long)(polygons[ply].vertexB - vertices);
    //unsigned long indexC = (unsigned long)(polygons[ply].vertexC - vertices);
    fprintf(dae, "%i %i %i ", (ply*3), (ply*3)+1, (ply*3)+2);
  }
  fprintf(dae, "</p>\n");
  
  fprintf(dae, "\t\t\t\t</triangles>\n");
  fprintf(dae, "\t\t\t</mesh>\n");
  fprintf(dae, "\t\t</geometry>\n");
  fprintf(dae, "\t</library_geometries>\n");
  
  fprintf(dae, "\t<library_visual_scenes>\n");
  fprintf(dae, "\t\t<visual_scene id=\"Scene\" name=\"Scene\">\n");
  fprintf(dae, "\t\t\t<node"
                     " id=\"%s\""
                     " type=\"NODE\">\n",
                     EIDString);
  fprintf(dae, "\t\t\t\t<instance_geometry" 
                       " url=\"#%s-mesh\">\n",
                       EIDString);
  fprintf(dae, "\t\t\t\t\t<bind_material>\n");
  fprintf(dae, "\t\t\t\t\t\t<technique_common>\n");
  fprintf(dae, "\t\t\t\t\t\t\t<instance_material"
                             " symbol=\"Material\""
                             " target=\"#Material-material\">\n");
  fprintf(dae, "\t\t\t\t\t\t\t\t<bind_vertex_input"
                               " semantic=\"UVMap\""
                               " input_semantic=\"TEXCOORD\""
                               " input_set=\"0\"/>\n");
  fprintf(dae, "\t\t\t\t\t\t\t</instance_material>\n");
  fprintf(dae, "\t\t\t\t\t\t</technique_common>\n");
  fprintf(dae, "\t\t\t\t\t</bind_material>\n");
  fprintf(dae, "\t\t\t\t</instance_geometry>\n");
  fprintf(dae, "\t\t\t</node>\n");
  fprintf(dae, "\t\t</visual_scene>\n");
  fprintf(dae, "\t</library_visual_scenes>\n");
  fprintf(dae, "\t<scene>\n");
  fprintf(dae, "\t\t<instance_visual_scene url=\"#Scene\"/>\n");
  fprintf(dae, "\t</scene>\n");
  
  fprintf(dae, "</COLLADA>");      
  
  fclose(dae);

  texbuf->exportTGA((const char*)tgaName);  
}
  
                