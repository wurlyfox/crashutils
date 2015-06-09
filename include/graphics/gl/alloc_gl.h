class alloc_gl : public alloc
{
  private:

  static prim_alloc_gl primBuffer;
  static geometry_gl geomBuffer;
  static texture_buffer_gl textureBuffer;
  static model_wld_gl worldBuffer[0x100];
  static model_obj_gl objectBuffer[0x100];
  static model_spr_gl spriteBuffer[0x100];
  static zone_gl zoneBuffer[MAX_ZONES];
  static model_gl modelBuffer[MAX_MODELS];
  static section_gl sectionBuffer[MAX_ZONES*MAX_SECTIONS];
  static collisionNode_gl collisionBuffer[MAX_ZONES*MAX_COLLISIONS];
  
  static unsigned long primAllocCount;
  static unsigned long geomCount;
  static unsigned long texBufCount;
  static unsigned long worldCount;
  static unsigned long objectCount;
  static unsigned long spriteCount;
  static unsigned long zoneCount;
  static unsigned long modelCount;
  static unsigned long sectionCount;
  static unsigned long collisionCount;
  
  public:
  
  prim_alloc_gl *allocPrimAlloc();
  geometry_gl *allocGeometry();
  texture_buffer_gl *allocTextureBuffer();
  model_wld_gl *allocWorld();
  model_obj_gl *allocObject();
  model_spr_gl *allocSprite();
  zone_gl *allocZone();
  model_gl *allocModel();
  section_gl *allocSection();
  collisionNode_gl *allocCollisionNode();

  void freePrimAlloc();
  void freeGeometry();
  void freeTextureBuffer();
  void freeWorld();
  void freeObject();
  void freeSprite();
  void freeZone();
  void freeModel();
  void freeSection();
  void freeCollisionNode();
}
  
 
  
  