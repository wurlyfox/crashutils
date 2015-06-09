#include "crashutils_win.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{ 
  crashutils *app = new crashutils_win();
  app->start();
	return true;
}