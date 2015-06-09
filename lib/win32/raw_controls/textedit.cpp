#include "raw_controls/textedit.h"

textEdit::textEdit(int editID, int x, int y, int w, int h, HWND _parent)
{
  
  ID     =  editID;
  parent = _parent;
  HINSTANCE hInstance = GetModuleHandle(NULL);
  
  width = w;
  height = h;
  
  int bulletAreaW = 20;
  int lineCounterW = 40;
  int headW = bulletAreaW+lineCounterW;
  int editW = width - headW;
  
  linenoRegion.left = x+bulletAreaW;
  linenoRegion.top  = y;
  linenoRegion.right = x+headW;
  linenoRegion.bottom = y+h;
  bulletRegion.left = x;
  bulletRegion.top = y;
  bulletRegion.right = x+bulletAreaW;
  bulletRegion.bottom = y+h;
  curBullet = { 0, 0, 0, 0 };
  
  hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", 
    WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL, 
    x+headW, y, editW, h, parent, (HMENU)ID, hInstance, NULL);
  
  lineHwnd = CreateWindowEx(WS_EX_CLIENTEDGE, "STATIC", "",
    WS_CHILD | WS_VISIBLE,
	  x, y, headW, h, parent, (HMENU)ID+1, hInstance, NULL);
			
  
  if (!hwnd || !lineHwnd)
  {
    MessageBox(NULL,"TextEdit Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
    lineCount = -1;   //error
  }
  else
  {
    lineCount = 0;  
    
    bulColor = CreateSolidBrush(RGB(255, 0, 0));
    selected = -1;
  }


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
				
  SendMessage(hwnd, WM_SETFONT, (WPARAM)font, FALSE);
  SendMessage(lineHwnd, WM_SETFONT, (WPARAM)font, FALSE);
}

void textEdit::setLines(char **srcLines, int count)
{
  lineCount = count;
  
  for (int lp = 0; lp < count; lp++)
  {
    strcpy(lines[lp], srcLines[lp]);
  }
}

void textEdit::clear()
{
  //for (int lp = 0; lp < lineCount; lp++)
  //lines[lp] = "\0";
	
  lineCount = 0;   
}

void textEdit::occupy(int count)
{
  clear();
  
  char temp[MAX_LINES*MAX_LINELENGTH];
	long tempoffset=0;
	
	char *dest;
	char *src;
	char *srcEnd;
  do
  {
    dest = temp+tempoffset;
	 
    src    = lines[lineCount];
 	  srcEnd = strchr(src, '\0');
	  
	  int term = srcEnd - src;
		
	  strncpy(dest, lines[lineCount], term);
	  tempoffset += term;
	  
	  lineCount++;
	  	  
	} while (lineCount < count);

	dest = temp+tempoffset;
	*(dest) = '\0';
	
	SetDlgItemText(parent, ID, temp);
}  

void textEdit::handle(WPARAM wParam)
{
  WORD code = HIWORD(wParam);
  WORD idFrom = LOWORD(wParam);
	  
  if (code == EN_VSCROLL && idFrom == ID)
  {
    //topLine = SendMessage(hwnd, EM_GETFIRSTVISIBLELINE, 0, 0);    
    updateIndices();
  }
}

//since windows is stupid and does not issue an EN_VSCROLL message
//when the thumb of a scrollbar is clicked/dragged, we have to force
//updating the current line/redrawing the line indices whenever the 
//edit control itself is redrawn (which coincidentally happens whenever
//the thumb is dragged)
void textEdit::onCtlColorEdit(LPARAM lParam)
{
  if ((HWND)lParam == hwnd)
    updateIndices();
}
    
void textEdit::updateIndices()
{
  topLine = GetScrollPos(hwnd, SB_VERT);
  invalidate();
}

void textEdit::invalidate()
{
  InvalidateRect(parent, &linenoRegion, false);
  InvalidateRect(parent, &bulletRegion, false);
}

void textEdit::selectLine(int line)
{
  selected = line;
  invalidate();
}

void textEdit::paint()
{
  char tempIndexStr[10];	
  int visibleLines = height/15-1;
  
  PAINTSTRUCT ps;
  
	HDC hDC = BeginPaint(lineHwnd, &ps);
  
	SelectObject(hDC, font);
  
	for (int lp = 0; lp < visibleLines; lp++)
	{
	  sprintf(tempIndexStr, "%04x:",(topLine+lp)*4);
	  int len = strlen(tempIndexStr); 
	 
    if ((topLine+lp) == selected) 
    { 
      FillRect(hDC, &curBullet, (HBRUSH)(COLOR_WINDOW));
      curBullet = { 6, 4+(lp*15), 14, 12+(lp*15) }; 
      FillRect(hDC, &curBullet, bulColor);
    }
    
    TextOut(hDC, 20, lp*15, tempIndexStr, len);
	}
  
  if (selected < topLine || selected > topLine+visibleLines)
  {
    FillRect(hDC, &curBullet, (HBRUSH)(COLOR_WINDOW));
    curBullet = { 0, 0, 0, 0 }; 
  }
  
	EndPaint(lineHwnd, &ps);
}  