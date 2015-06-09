   ____               _     _   _ _   _ _     
  / ___|_ __ __ _ ___| |__ | | | | |_(_) |___ 
 | |   | '__/ _` / __| '_ \| | | | __| | / __|
 | |___| | | (_| \__ \ | | | |_| | |_| | \__ \
  \____|_|  \__,_|___/_| |_|\___/ \__|_|_|___/
                                              
         __      __               __  
        /  )      _/     -/      /  ) 
       (__/  .   /   .   /  .   (__/  
       

CONTENTS
========
1) Requirements
2) Quick Guide
3) Controls
4) Features
5) Bugs
6) Misc

1) REQUIREMENTS
==============
 > 256 MB ram, uses approx. 150 MB (future releases will implement paging the NSFs, thus requiring much less)
  

2) QUICK GUIDE
==============

Use File->Open to select an .NSD file on the Crash disc. Expand the 'chunk' categories and select the game data
to view by right clicking on an 'entry'. Currently supported entry types are those that end with the following
characters:

  C - GOOL executable (code and data for object behavior)
  W - WGEO model      (model for a portion of the level)
  T - TGEO model      (model for an object in the level)
  Z - ZDAT zone       (camera path 'sections', collision data, and objects in a smaller 'zone' of a larger 
                       collection of up to 8 visible, usually adjacent WGEO models present in the level)

Refer to userguide.txt for a more detailed guide.

3) CONTROLS
===========

Camera control mode
-------------------
A        - camera zoom/move forward
Z        - camera zoom/move backward
K        - camera rotate left
;        - camera rotate right
O        - camera rotate up
L        - camera rotate down

All modes
---------
Numpad 7 - camera roll left
Numpad 9 - camera roll right
+        - camera increase movement and rotation rate
-        - camera decrease movement and rotation rate
P        - camera reset to (0, 0, 0)

Model view mode only
--------------------
Numpad 4 - pan left
Numpad 6 - pan right
Numpad 8 - pan DOWN
Numpad 2 - pan UP

CrashPC control mode
--------------------
(corresponding to PSX controller buttons)
K        - pad left
;        - pad right
O        - pad up
L        - pad down
Z        - ><
X        - []
C        - ()
V        - /\
A        - L1
S        - R1
Q        - L2
W        - R2
Enter    - start
B        - select

Shift    - while holding shift, press any of the keys from camera control mode to adjust the camera
           in relation to the player

4) FEATURES
===========

1.) View game content 
  - View NSD/NSF file chunk/entry/item hierarchy
  - View 3D models (WGEO/TGEO models)
  - View level layout
  - View level zone information
  - GOOL disassembler
  
2.) Convert/export game content
  - WGEO model -> COLLADA model/TGA texture map

3.) Execute game content
  - Crash Bandicoot port runs in background
  - Spawn and control player object
  - Spawn other GOOL objects
  
    
5) BUGS
=======

CrashUtils is still somewhat early in development and therefore is likely to 'crash' on more than one occasion.
You can email me at wurlitzerfox@yahoo.com if you think you've found any major issues.
 

6) MISC
=======

CrashUtils 0.7.1.0 by WurlitzerFox; last build 6/8/2015