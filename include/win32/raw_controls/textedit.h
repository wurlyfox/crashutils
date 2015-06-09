#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>

#define MAX_LINES 0x2000
#define MAX_LINELENGTH 80

class textEdit
{
  private:
  
  HWND hwnd;
  HWND lineHwnd;
  HWND parent;
  
  HFONT font;
  HBRUSH bulColor;
  
  int ID;
  int lineCount;
  int selected;
  
  RECT linenoRegion;
  RECT bulletRegion;
  RECT curBullet;
  int width, height;
  int topLine, botLine;
  
  public:
  textEdit(int editID, int x, int y, int w, int h, HWND _parent);
  char lines[MAX_LINES][MAX_LINELENGTH];

  void setLines(char **srcLines, int count);
  void selectLine(int line);
  void updateIndices();
  void onCtlColorEdit(LPARAM lParam);
  
  void invalidate();
  
  void occupy(int count);
  void clear();
  
  void handle(WPARAM wParam);
  void paint();
};

#endif