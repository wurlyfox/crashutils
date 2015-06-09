#include "raw_controls/hexedit.h"

hexEdit::hexEdit(int editID, int x, int y, int row, int col, int pl, int pr, int pt, int pb, HWND _parent)
{
  ID     =  editID;
  parent = _parent;
  HINSTANCE hInstance = GetModuleHandle(NULL);

  rows = row;
  columns = col;
  
  padLeft = pl;
  padRight = pr;
  padTop = pt;
  padBottom = pb;
  
  width = padLeft + (columns*(18+padLeft+padRight)) + 20;
  height = padTop  + (rows*(18+padTop+padBottom));
    		
  region.left = x;
  region.top = y;
  region.right = x+width;
  region.bottom = y+height;
  
  hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, "STATIC", "", 
    WS_CHILD | WS_VISIBLE | ES_MULTILINE, 
    x, y, width, height, parent, (HMENU)ID, hInstance, NULL);

  scrwnd = CreateWindowEx(WS_EX_LEFT, "SCROLLBAR", "",
    WS_CHILD | WS_VISIBLE | SBS_VERT,
    x+width, y, 15, height, parent, (HMENU)ID+1, hInstance, NULL);
  
  if (!hwnd)
  {
    MessageBox(NULL,"HexEdit Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
  }
  else
  {
    font = CreateFont(        9,  /* Height Of Font */
  	                          0,  /* Width Of Font */
					                    0,  /* Angle Of Escapement */
                              0,  /* Orientation Angle */
                              0,  /* Font Weight */
                          FALSE,  /* Italic */
                          FALSE,  /* Underline */
                          FALSE,  /* Strikeout */
                   ANSI_CHARSET,  /* Character Set Identifier */
            CLIP_DEFAULT_PRECIS,  /* Clipping Precision */
		              OUT_TT_PRECIS,  /* Output Precision */
            ANTIALIASED_QUALITY,  /* Output Quality */
      FF_DONTCARE|DEFAULT_PITCH,  /* Family And Pitch */	   
                    "Fixedsys"); /* Font Name */
                    
    line = 0;
    selectionCount = 0;
    
    SendMessage(hwnd, WM_SETFONT, (WPARAM)font, FALSE);
  }
}

void hexEdit::setSource(unsigned char *src, int len)
{
  data       = src;
  dataLength = len;
  
  lineCount  = dataLength / columns;
  
  SetScrollRange(scrwnd, SB_CTL, 0, lineCount, FALSE);
}

void hexEdit::occupy()
{
  for (int h = 0; h < rows; h++)
  {
    int offset = (h + line);
    for (int w = 0; w < columns; w++)
	    sprintf(strings[w + (h*columns)], "%02x", data[(offset*columns)+w]);  
  }
}  

void hexEdit::addSelection(unsigned short pos, unsigned short len, unsigned long color)
{
  selections[selectionCount].pos = pos;
  selections[selectionCount].len = len;
  selections[selectionCount].color = color;
  
  selectionCount++;
}
  
void hexEdit::changeSelection(int selection, unsigned short pos, unsigned short len, unsigned long color)
{
  if (selection < selectionCount)
  {
    selections[selection].pos = pos;
    selections[selection].len = len;
    selections[selection].color = color;
  }
}

void hexEdit::scroll(WPARAM wParam)
{
  WORD code = LOWORD(wParam);
  
  int oldLine = line;
  
  switch (code)
  {
    case SB_TOP:
      line = 0;
    break;

    case SB_BOTTOM:
      line = lineCount;
    break;
        
    case SB_THUMBPOSITION: case SB_THUMBTRACK:
    {
      SCROLLINFO si = { sizeof(si), SIF_TRACKPOS };
      GetScrollInfo(scrwnd, SB_CTL, &si);
      line = si.nTrackPos;
    }
    break;
        
    case SB_LINEUP:
      line--;
    break;
 
    case SB_LINEDOWN:
      line++;
    break;    
    
    case SB_PAGEUP:
      line-=rows;
    break;    
     
    case SB_PAGEDOWN:
      line+=rows;
    break;
  }
  
  if       (line < 0)        { line = 0; }
  else if (line > lineCount) { line = lineCount; }
 
  if (line != oldLine)
  {
    SetScrollPos(scrwnd, SB_CTL, line, TRUE);
    occupy();
    invalidate();
  }
}


#define getHexX(column) padLeft + (column*(18+padLeft+padRight))
#define getHexY(row)    padTop  + (row   *(18+padTop +padBottom))

void hexEdit::paint()
{
  PAINTSTRUCT ps;  
  HDC hDC = BeginPaint(hwnd, &ps);
  
  SelectObject(hDC, font);
  
  RECT box;
  HBRUSH black = (HBRUSH)GetStockObject(BLACK_BRUSH);
  
  for (int h = 0; h < rows; h++)
  {
    box.top = (h*(18+padTop+padBottom));
    int y = padTop + box.top;
    box.bottom = (padTop + box.top) + (18+padBottom);
    
    for (int w = 0; w < columns; w++)
    {
      box.left = (w*(18+padLeft+padRight));
      int x = padLeft + box.left;
      box.right = (padLeft + box.left) + (18+padRight);
      
      TextOut(hDC, x, y, strings[w + (h*columns)], 2);
      FrameRect(hDC, &box, black);
    }
  }
  
  long old = GetBkColor(hDC);
  for (int lp = 0; lp < selectionCount; lp++)
  {
    
    int start = selections[lp].pos;
    int end   = selections[lp].pos+selections[lp].len;
    int startRow = (start/columns - line);
    int endRow = (end/columns - line);
    if (startRow >=0 && startRow < rows && endRow >= startRow && endRow < rows)
    {
      //int endRow   = (end/columns) - line;
      //int startCol = start % columns;
      //int endCol   = end   % columns;
      //int selRows = endRow - startRow
      SetBkColor(hDC, selections[lp].color);
      
      for (int dlp = start; dlp < end; dlp++)
      {
        int screenCol = dlp % columns;
        int screenRow = (dlp / columns) - line;
        int x = getHexX(screenCol);
        int y = getHexY(screenRow);
        TextOut(hDC, x, y, strings[dlp], 2);        
      }
    }
  }
  SetBkColor(hDC, old);
  
  EndPaint(hwnd, &ps);
} 
	  
void hexEdit::invalidate()
{
  InvalidateRect(parent, &region, false);
}