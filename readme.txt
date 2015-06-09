   ____               _     _   _ _   _ _     
  / ___|_ __ __ _ ___| |__ | | | | |_(_) |___ 
 | |   | '__/ _` / __| '_ \| | | | __| | / __|
 | |___| | | (_| \__ \ | | | |_| | |_| | \__ \
  \____|_|  \__,_|___/_| |_|\___/ \__|_|_|___/
                                              
         __      __               __  
        /  )      _/     -/      /  ) 
       (__/  .   /   .   /  .   (__/  
       
               =Source Code=

CONTENTS
========
1) Directory structure/project configuration
2) Compiling and linking
3) Contributing
4) Misc

1) DIRECTORY STRUCTURE
======================

./archive    - Archived source for previous builds
./bin        - Binary executable and .dlls
./doc        - Project documentation
./include    - Library headers
./lib        - Library sources
./src        - Project source code
./Makefile   - Project Makefile
./readme.txt - This file

Project documentation
---------------------   
./doc/requirements.txt  - Feature requirements/development progress tracking
./doc/crashpc.txt       - Crash Bandicoot C++ port (CrashPC) progress tracking 
./doc/sysinteg.txt      - Systems integration                                
./doc/editor.txt        - Level editor configuration, structure, and current features
./doc/rationale.txt     - Project design rationale
./doc/crash             - Crash Bandicoot disassembly guide & reverse engineering docs.

Misc/incomplete project documentation
-------------------------------------
./doc/uml.png           - UML diagram
./doc/gfxapi.txt        - CrashPC graphics API doc
./doc/refactor.txt      - Code refactoring procedures for transition to 0.7.1.0

Library sources
---------------
./lib/*.cpp             - Source code for CrashUtils-specific class libraries 
./lib/crash             - Source code for crash library
./lib/graphics          - Source code for CrashPC graphics library
./lib/win32             - Source code for CrashUtils Win32 Window framework 
./lib/unix              - Unix-specific wrapper class implementations

Library headers
---------------
./include/*.h           - Headers for CrashUtils-specific class libraries
./include/crash         - Headers for crash library
./include/formats       - Headers for various file formats
./include/graphics      - Headers for CrashPC graphics library
./include/win32         - Headers for CrashUtils Win32 Window framework
./include/unix          - Unix-specific wrapper class definitions

Project source
--------------
-modules
./src/crash             - Crash Bandicoot C++ port (CrashPC)
./src/context-plugins   - Level/Zone editor
./src/win32             - Win32 Windows, Controls, Dialogs, etc.

-subsystems
./src/crashutils.cpp    - Main system
./src/gfx_system.cpp    - Graphics system
./src/win_system.cpp    - (Win32) Windows system
./src/crash_system.cpp  - CrashPC system
./src/console_system.cpp- Message monitoring system
./src/message.h         - Message definitions
./src/export.cpp        - File export features

-main sources
./src/winmain.cpp       - Entry point for Win32
./src/crashutils_win.cpp- Main system configured for Win32
./src/context_wgl.cpp   - Windows/OpenGL context control

2) COMPILING AND LINKING
========================

MinGW32 v4.8.1 is currently used for compiling and linking
CrashUtils 0.7.1.0 and its associated libraries. The  
application's Makefile is located in the project root directory.
Makefiles for each library are located in their source directories.
A Makefile for compiling each library is located in the ./lib 
directory. 

Simply 'cd <root dir> && make' to generate the application; the
resulting binary will be created in the ./bin directory if compiling
and linking procedures are successful.


3) CONTRIBUTING
===============

For anyone wishing to contribute to this project, I would first recommend 
viewing the 'Feature requirements/development progress tracking' document
(./doc/requirements.txt) for a list of aspects; those aspects requiring 
implementation are listed with a status of 'Non-existent'. Each aspect 
includes its designated component. Implementation for an aspect should
be done in the source file associated with its designated component.

I would then recommend following these steps:

  1) read the 'Systems integration' documentation (./doc/sysinteg.txt)
  2) view the CrashUtils library 'system' header file (./include/system.h)
  3) analyze source code for the subsystems (listed above)
  4) trace the ensuing sequence of interactions initiated from WinMain 
  
...for the following purposes:

  1) to understand that the subsystems interact
  2) to understand how the subsystems interact
     - key idea is to recognize that when one subsystem 'posts' a message,
       all other subsystems receive it
  3) to become familiarized with the general sequence of interactions
     between subsystems
  
I would then recommend reading API documentation for the various libraries-
if there were any; this documentation has yet to be written. Then, for the
time being, I recommend analyzing the source code for the relevant libraries
and the modules that utilize them, for an idea on how to use them in your
implementation.

Email me at wurlitzerfox@yahoo.com with your implementation and I can add
you to the source repo as a collaborator. Also, please feel free to 
challenge my design choices. I am still quite inexperienced as a software 
engineer and would greatly appreciate suggestions of more organized and 
efficient solutions.

If you really know what you're doing, you can also attempt to port the 
remaining unimplemented functions listed in ./doc/crashpc.txt.

4) MISC
=======

-After releasing this source code, my next course of action is to continue 
porting Crash to C++ (i.e. the remaining unimplemented functions listed in 
./doc/crashpc.txt)

-If there are any issues with compiling/linking please email me with your 
compiler/linker output and any additional details at wurlitzerfox@yahoo.com.

-The same goes for bug reporting, as listed in the application readme. If
you think you've found any significant runtime errors or defects please 
email me [with a detailed description of the error(s)/defect(s) and, if 
possible, what is known or believed to have triggered them.]

-Happy coding :3