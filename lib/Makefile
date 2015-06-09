# CrashUtils libraries Makefile

# include dirs.
INCLUDE = -I../include/ 

# C++ compiler flags (-g -O3 -Wall)
CCFLAGS = -g -w

# compiler
CC = g++

LIB = libwin32.a libgfx.a libcrash.a
OBJ = system.o

all: $(OBJ) $(LIB)

libwin32.a:
	cd win32 && $(MAKE)

libgfx.a:
	cd graphics && $(MAKE)
	
libcrash.a:
	cd crash && $(MAKE)
	
system.o: system.cpp
	$(CC) $(INCLUDE) $(CCFLAGS) -c $< -o $@

