#ifndef HEXEDIT_H
#define HEXEDIT_H

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>

#define MAX_HEXWIDTH   16
#define MAX_HEXHEIGHT  80

typedef struct
{
  long color;
  unsigned short pos;
  unsigned short len;
} hexSelection;
  
class hexEdit
{
  private:
  
  HWND hwnd;
  HWND scrwnd;
  HWND parent;
  HFONT font;
  int ID;
  
  unsigned char *data;
  int dataLength;
  
  int line;
  int lineCount;
  
  RECT region;
  int width, height;
  int rows, columns;
  int padLeft, padRight, padTop, padBottom;
  
  int selectionCount;
  hexSelection selections[20];
  
  public:
  hexEdit(int editID, int x, int y, int row, int col, int pl, int pr, int pt, int pb, HWND _parent);
  char strings[MAX_HEXHEIGHT*MAX_HEXWIDTH][2];

  void setSource(unsigned char *src, int len);
  void occupy();
  void addSelection(unsigned short pos, unsigned short len, unsigned long color);
  void changeSelection(int selection, unsigned short pos, unsigned short len, unsigned long color);
  void scroll(WPARAM wParam);
  void paint();
  void invalidate();
};

#endif