NOTE: these docs are *mostly deprecated; they are not intended to be 'user-friendly'. if you can understand my 
unconventional pseudo-c style code and possibly foreign naming schemes then they may prove to be useful. a lot 
of the docs date back to a time when i obviously knew much less about the game structure and as a result are 
more difficult to follow.

./audio               - pseudo-code for few spu-related routines
./engine              - pseudo-code for the object, camera, and level engines. also for object process management
./gool                - pseudo-code for the gool interpreter
./gool/gool.txt       - description of intepreter routine and simpler opcodes/operations
./gool/type***.txt    - psuedo-code for more complex opcodes/operations
./gool/code/*.txt     - pseudo-code for various gool executables
./init                - pseudo-code for initialization routines
./misc                - various docs
./pagesys             - pseudo-code for the paging system
./subsystems          - pseudo-code for subsystem dependent routines
./video               - pseudo-code for gte & gpu related/model related routines
./flow.txt            - outlines the game's general flow of execution
./subroutinedescs.txt - descriptions of many of the game's subroutines
./memorymap.txt       - documents contents of the game's memory locations
./main.txt            - pseudo-code for the main game loop and game initialization routines
./titlesandmap.txt    - psuedo-code for the title screen routine
./coordinates.txt     - descriptions of/formats of various coordinate types
./protodiffs.txt      - (UPDATE: 6/6/2015 *NEW*) mapping of routines in final build to routines in prototype build
./README.txt          - this document


of particular interest are:

./main.txt            - see the main game loop
./gool/code/*.txt     - dissects game objects; we can somewhat see how crash, boxes, crabs, and fruits work
./memorymap.txt       - for general hacking start here

deprecated terms
----------------
chunkinfo - page structure (pages specific data from nsf chunks on disc to memory) 
entrylist - entry hash table
cidlist   - pageMap

