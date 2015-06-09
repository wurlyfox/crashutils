#ifndef TGA_H
#define TGA_H

#pragma pack(push, 1)

struct tga_colormap_spec
{
  unsigned short first_entry_index;
  unsigned short length;
  unsigned char bpp;
};

struct tga_image_spec
{
  unsigned short x_origin;
  unsigned short y_origin;
  unsigned short width;
  unsigned short height;
  unsigned char bpp;
  unsigned char descriptor;
};
  
struct tga_header
{
  unsigned char id_length;
  unsigned char colormap_type;
  unsigned char image_type;
  tga_colormap_spec colormap_spec;
  tga_image_spec image_spec;
};
#pragma pack(pop)

#endif