# CrashUtils Crash Library Makefile

# sources
SRC = nsd.cpp nsf.cpp \
      chunk.cpp entry.cpp eid.cpp \
			r3000a.cpp \
      zone.cpp
            
OBJ = $(SRC:.cpp=.o)
OUT = ../libcrash.a

# include dirs.
INCLUDE = -I../../include/crash/ 

# C++ compiler flags (-g -O3 -Wall)
CCFLAGS = -g -w

# compiler
CC = g++

all: $(SRC) $(OUT)

.cpp.o:
	$(CC) $(INCLUDE) $(CCFLAGS) -c $< -o $@
  
$(OUT): $(OBJ)
	ar rcs $(OUT) $(OBJ)