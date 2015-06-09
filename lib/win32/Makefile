# CrashUtils Win32 Wrapper Framework/Library Makefile

# sources
SRC = window.cpp basic_window.cpp dialog.cpp mdi_window.cpp mdi_child_window.cpp \
      time.cpp thread.cpp \
      raw_controls/raw_control.cpp raw_controls/check.cpp raw_controls/drop.cpp \
      raw_controls/edit.cpp raw_controls/hexedit.cpp raw_controls/list.cpp \
      raw_controls/radio.cpp raw_controls/textedit.cpp \
      raw_controls/tree.cpp \
      controls/colorbox.cpp controls/context_win.cpp controls/surface.cpp
      
OBJ = $(SRC:.cpp=.o)
OUT = ../libwin32.a

# include dirs.
INCLUDE = -I../../include/win32/ 

# C++ compiler flags (-g -O3 -Wall)
CCFLAGS = -g -w

# compiler
CC = g++

all: $(SRC) $(OUT)

.cpp.o:
	$(CC) $(INCLUDE) $(CCFLAGS) -c $< -o $@
  
$(OUT): $(OBJ)
	ar rcs $(OUT) $(OBJ)
  


