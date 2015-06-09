void collisionVectors(cvector *vectA, cvector *vectB)
{
  if ((vectB->X == 0) &&
      (vectB->Y == 0) &&
      (vectB->Z == 0))
    return 0;
  
  unsigned char *currentZoneHeader = currentZone->itemData[0];
  
  unsigned long retval = 0;
  
  do
  {
    // number of neighbor zones
    unsigned long neighborCount = getWord(currentZoneHeader, 0x210, true);
    
    // counter/iterator
    unsigned long count = 0;
    if (neighborCount > 0)
    {
      do
      {
        unsigned long neighborEID = getWord(currentZoneHeader, 0x214+(count*4), true);
        entry *neighborZone       = goolSystemPage(neighborEID);
        
        unsigned char *neighborCollisions = neighborZone->itemData[1];
        
        // get zone location and dimensions
        signed long zoneX   = getWord(neighborCollisions,    0, true) << 8;
        signed long zoneY   = getWord(neighborCollisions,    4, true) << 8;
        signed long zoneZ   = getWord(neighborCollisions,    8, true) << 8;
        unsigned long zoneW = getWord(neighborCollisions,  0xC, true) << 8;
        unsigned long zoneH = getWord(neighborCollisions, 0x10, true) << 8;
        unsigned long zoneD = getWord(neighborCollisions, 0x14, true) << 8;
        
        // if vector A lies within the zone
        if (vectA->X >= zoneX &&
            vectA->Y >= zoneY &&
            vectA->Z >= zoneZ &&
            vectA->X < (zoneX + zoneW) &&
            vectA->Y < (zoneY + zoneH) &&
            vectA->Z < (zoneZ + zoneD))
        {
          cspace zoneSpace;
          zoneSpace.xyz1.X = zoneX;
          zoneSpace.xyz1.Y = zoneY;
          zoneSpace.xyz1.Z = zoneZ;
          zoneSpace.xyz2.X = zoneX + zoneW;
          zoneSpace.xyz2.Y = zoneY + zoneH;
          zoneSpace.xyz2.Z = zoneZ + zoneD;
          
          unsigned short rootNode = getHword(neighborCollisions, 0x1E, true);
          unsigned short level    = 0;
          
          retval = findNode(neighborCollisions, rootNode, zoneSpace, vectA, vectB, level);

          break;
        }
      } while (count < neighborCount);
      
      if (count == neighborCount) { return true; }
    }
  } while (!retval);

  return retval;
}  

void findNode(unsigned char *zoneCollisions, unsigned short node, cspace zoneSpace, cvector *vectA, cvector *vectB, unsigned short level)
{
  // we want to search the given 'node's -child- nodes
  unsigned short *childNodes;
  
  // if the given node a non-leaf node (has children)
  // or the given node is an empty node 
  // (i.e. refers to an non-occupied, non-solid, empty region of space)
  if ((node & 1) == 0)
  {
    // if the node is a non-leaf node
    if (node & 0xFFFF)
      childNodes = &zoneCollisions[node & 0xFFFF]; //it can be subdivided into its children
    else
      childNodes = 0;                              //else it can be subdivided no further
  }  
 
  // if the given node is a non-leaf node
  if (((node & 1) == 0) && (node & 0xFFFF))
  {
    // we'd like to determine the dimensions of its child nodes
    cspace childSpace;

    // firstly, we'd like to determine the width, height, and depth

    // get the max levels/depths (number of recursive subdivisions) of the tree 
    // for each dimension
    unsigned short depthW = getHword(zoneCollisions, 0x1E, true);
    unsigned short depthH = getHword(zoneCollisions, 0x20, true);
    unsigned short depthD = getHword(zoneCollisions, 0x22, true);
    
    // if the current level does not exceed the maximum level for the X dimension
    if (level < depthW)               // then the node's corresponding space (zoneSpace) 
      childSpace.W = zoneSpace.W / 2; // can be subdivided (halved) further by width..
    else                              // otherwise, the node's space
      childSpace.W = zoneSpace.W;     // can not be subdivided further by width
    
    // if the current level does not exceed the maximum level for the Y dimension
    if (level < depthH)               // then the node's corresponding space (zoneSpace) 
      childSpace.H = zoneSpace.H / 2; // can also be subdivided (halved) further by height..
    else                              // otherwise, the node's space
      childSpace.H = zoneSpace.H;     // can not be subdivided further by height
     
    // if the current level does not exceed the maximum level for the Z dimension
    if (level < depthD)               // then the node's corresponding space (zoneSpace) 
      childSpace.D = zoneSpace.D / 2; // can also be subdivided (halved) further by depth..
    else                              // otherwise, the node's space
      childSpace.D = zoneSpace.D;     // can not be subdivided further by depth
     
    // then we determine the (location) x, y, and z of each of its child nodes
    unsigned long countX = 0;
    unsigned long countY = 0;
    unsigned long countZ = 0;
    unsigned short child = 0;
    do
    {
      if ((depthW == 0) && (countX > 0))
        break;
        
      do
      {
        if ((depthH == 0) && (countY > 0))
          break;
          
        do
        {
          if ((depthD == 0) && (countZ > 0))
            break;
                      
          childSpace.X = zoneSpace.X;
          if (countX != 0)
            childSpace.X += childSpace.W;
            
          childSpace.Y = zoneSpace.Y;
          if (countY != 0)
            childSpace.Y += childSpace.H;
            
          childSpace.Z = zoneSpace.Z;
          if (countZ != 0)
            childSpace.Z += childSpace.D;
              
          // if vectA lies within the current child node/subdivision
          if ((childSpace.X < vectA->X) && 
              (childSpace.Y < vectA->Y) && 
              (childSpace.Z < vectA->Z) &&
              ((childSpace.X + childSpace.W) > vectA->X) &&
              ((childSpace.Y + childSpace.H) > vectA->Y) &&
              ((childSpace.Z + childSpace.D) > vectA->Z))
          {
            unsigned long childNode = childNodes[count];
            
            unsigned long retval = findNode(zoneCollisions, childNode, childSpace, vectA, vectB, level+1);
            if (retval != 0) { return retval; }
          }
          
          child++;
        } while (++countZ < 2);
      } while (++countY < 2);        
    } while (++countX < 2);
    
    return 0; //retval;
  }
  // else the given node is an empty node 
  // (i.e. refers to an non-occupied, non-solid, empty region of space)
  // we have to interpolate the ray one unit further than the closest face
  // that it intersects to get the location of the next potentially solid
  // node at its direction
  else if ((node & 1) == 0)
  { 
    signed long xval, yval, zval;
    
    // let R be a ray starting at origin A (vectA/pointA) 
    // and pointing in direction B (vectB)

    // R = A + Bt
    
    // (we can find each point on the ray R for each 
    // corresponding value of t)

    // we also know that, at this point, the origin point A is
    // within the box/space of the containing node
    
    // we want to find which of the containing node's faces 
    // that this ray will -first- intersect with (considering
    // this is collision testing, we typically desire that
    // the object on this ray path is stopped at the first 
    // intersection pt, such that it could not travel any
    // further to reach those inevitable points of 
    // intersection with faces further away)
    // then we can move the object at origin pt.A (vectA)
    // in the given direction B (vectB) exactly up to the 
    // closest face
    
    // since we know that the origin point A is within the box
    // space of the containing node, then based on the ray's 
    // direction B, right off the bat we can eliminate 3 of the 
    // 6 node/cube faces: those aligned with the respective x,
    // y, and z axes that are 'behind' the origin pt A, since
    // these could not possibly be reached at the ray's direction
    // thus we consider only the offsets (locations) of the 
    // 3 faces that the ray point towards, each in their
    // separate respective axis
    
    if (vectB->X <= 0)                    //if negative in the x direction
      xval = zoneSpace->X;                //we may hit the left of the node
    else                                  //else if positive 
      xval = zoneSpace->X + zoneSpace->W; //we may hit the right of the node
      
    if (vectB->Y <= 0)                    //if negative in the y direction
      yval = zoneSpace->Y;                //we may hit the top of the node
    else                                  //else if positive 
      yval = zoneSpace->Y + zoneSpace->H; //we may hit the bottom of the node
      
    if (vectB->Z <= 0)                    //if negative in the z direction
      zval = zoneSpace->Z;                //we may hit the back of the node
    else                                  //else if positive 
      zval = zoneSpace->Z + zoneSpace->D; //we may hit the front of the node
      
    // note that, given the offsets in the respective axes of
    // each face, we have the respective equations of their 
    // planes:
    //
    // X = xval
    // Y = yval
    // Z = zval
    //
    // then we can solve for t for where each 
    // component of the ray intersects the respective
    // axis aligned plane:
    //
    // R = A + Bt =
    // Rx = Ax + Bxt
    // Ry = Ay + Byt
    // Rz = Az + Bzt
    //
    // (we want to find t when the component R* is at
    //  the location/offset of the plane/face in axis '*';
    //  that is, what is t for when the ray intersects the
    //  face/plane at for each respective component)
    //
    // xval = Ax + Bxt
    // Bxt = xval - Ax
    // tX = (xval - Ax)/Bx
    //
    // similarly for y and z
    // tY = (yval - Ay)/By
    // tZ = (zval - Az)/Bz
    //
    // since for -larger- t, we get a *point* -further- along 
    // the rays path as a result, the -smallest- t from these 3 
    // calculations determines the -closest- *point* along the
    // rays path that is also a point on the respective -closest-
    // face; thus we determine the min:
    
    unsigned long min = 0x7FFFFFFF;
    if (vectB->X != 0)
    {
      long tX = CTCVC(xval - vectA->X)/vectB->X;
      
      if (tX < min)
        min = tX;
    }
    
    if (vectB->Y != 0)
    {
      long tY = CTCVC(yval - vectA->Y)/vectB->Y;
      
      if (tY < min)
        min = tY;
    }
    
    if (vectB->Z != 0)
    {
      long tZ = CTCVC(zval - vectA->Z)/vectB->Z;
      
      if (tZ < min)
        min = tZ;
    }
    
    // after the following line, min will refer to one point 
    // -further- than the point of the closest plane/edge of 
    // the node (this is to move vectA into the realm of an
    // adjacent, possibly solid node, at the specified dir. of 
    // vectB)
    
    min += CTCVC(1);  
                                        
    // now we move the object at given pt A (vectA) in 
    // direction B (vectB), i.e. along ray R, until it 
    // reaches one point further than the point on the closest 
    // face (plane) at t = min, 
    
    vectA->X += CTGVC(min * vectB->X);
    vectA->Y += CTGVC(min * vectB->Y);
    vectA->Z += CTGVC(min * vectB->Z);
    
    return 0;
  }
  // else the node is a leaf node, so we use its value
  else
  {
    // previously, we were within a non-solid node and after
    // travelling in the direction of the ray, we crossed over the
    // closest face; the crossing took place such that we are now
    // one unit outside of/away from that node and within the 
    // current, solid (adjacent) node. since we know we are within
    // a solid node, now we reverse trace, or trace the ray back 
    // in the opposite direction, to see which of the faces that 
    // was crossed (but now in terms of this node).
    
    // by vector subtraction:
    //
    // A - B = C
    //
    // we get point C, a point that approximates our previous
    // position within the non-solid node such that we can then
    // find the dist between that point and -now- the faces of the
    // solid cube we are within
    
    // 
  
  }
}
  
    
     