# CrashPC Graphics Library Makefile

# sources
SRC = render_manager.cpp context.cpp context_plugin.cpp \
      scene.cpp \
	    camera.cpp geometry.cpp model.cpp \
	    zone.cpp \
	    prim_alloc.cpp texture_buffer.cpp \
      model_wld.cpp model_obj.cpp model_spr.cpp model_frg.cpp \
	    model_msc.cpp \
			$(SRCGL) $(SRCMISC)
			
SRCGL = gl/context_gl.cpp gl/scene_gl.cpp \
        gl/camera_gl.cpp gl/geometry_gl.cpp gl/model_gl.cpp \
				gl/zone_gl.cpp \
				gl/texture_buffer_gl.cpp \
				gl/model_wld_gl.cpp gl/model_obj_gl.cpp gl/model_spr_gl.cpp \
				gl/model_frg_gl.cpp gl/model_msc_gl.cpp \
				gl/immediate.cpp
            
SRCMISC = image/texture_buffer_img.cpp \
          collada/model_wld_collada.cpp \
          md3/model_obj_md3.cpp md3/texture_buffer_md3.cpp
          
LIB = -lopengl32 -lglu32

OBJ = $(SRC:.cpp=.o)
OUT = ../libgfx.a

# include dirs.
INCLUDE = -I../../include/ -I../../include/graphics/ 

# C++ compiler flags (-g -O3 -Wall)
CCFLAGS = -g -w

# compiler
CC = g++

all: $(SRC) $(OUT)

.cpp.o:
	$(CC) $(INCLUDE) $(LIB) $(CCFLAGS) -c $< -o $@
  
$(OUT): $(OBJ)
	ar rcs $(OUT) $(OBJ)
  


