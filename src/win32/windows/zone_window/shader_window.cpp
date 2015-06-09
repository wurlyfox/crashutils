#include "shader_window.h"

shader_window::~shader_window()
{
  delete depth;
  delete clearHeight;
  
  delete farColor;
  delete clearColor;
    
  for (int lp=0; lp<6; lp++)
    delete effects[lp];
}

LRESULT shader_window::onCreate(UINT uMsg, WPARAM wParam, LPARAM lParam)
{                        
  createGroupBox(0, 20,  20, 160, 285, "Shader/effect");
  
  char text[6][30] = { "None",
                       "Ripple/vibration",
                       "Lightning",
                       "Depth cuing (mode 1)",
                       "Depth cuing (mode 2)",
                       "Depth cuing (mode 3)" };
                  
  raw_group_createparams params = {   1,  6, 
                                      IDC_RADIONONE, 
                                      30, 40, 140, 12,
                                       0,  0,  20, 20,
                                       (char*)text, 30 };
                                      
  raw_control::createGroup(effects, params, hSelf);
  
  createText(0, 30, 240, 50, 14, "Far color");
  addChild(farColor = new colorbox);
  farColor->create(0, 150, 240, IDC_COLORBOXFARCOLOR);
  
  createText(0, 30, 270, 50, 14, "Depth");
  depth = new edit(IDC_EDITDEPTH, 100, 270, 65, 18, hSelf);
  depth->addSpin(IDC_SPINDEPTH, -2147483648, 2147483647);
  
  createGroupBox(0, 210, 20, 160, 285, "GPU parameters");
  
  createText(0, 220, 80, 105, 12, "VRAM fill/clear color");
  addChild(clearColor = new colorbox);
  clearColor->create(0, 340, 80, IDC_COLORBOXCLEARCOLOR);
  
  createText(0, 220, 180, 105, 12, "VRAM fill/clear height");
  clearHeight = new edit(IDC_EDITCLEARHEIGHT, 330, 180, 25, 14, hSelf);
  clearHeight->addSpin(IDC_SPINCLEARHEIGHT, -32768, 32767);
  
  created = true;
}

LRESULT shader_window::onNotify(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  depth->handle(0, lParam);
  clearHeight->handle(0, lParam);
}

LRESULT shader_window::onCommand(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  depth->handle(wParam, 0);
  clearHeight->handle(wParam, 0);
}

void shader_window::onExternal(int msg, param lparam, param rparam)
{
  if (msg == WSM_SELECT_WINDOW_SHADER) { select(); }
}

void shader_window::onSelectZone(entry *zone)
{
  readCrashZoneHeader(zone, header);

  effects[0]->useFlag(&header.flags, -5, -9, -10, -11);
  effects[1]->useFlag(&header.flags, 9);
  effects[2]->useFlag(&header.flags, 10, RADIO_USEFIELD);
  effects[3]->useFlag(&header.flags,  5, RADIO_USEFIELD);
  effects[4]->useFlag(&header.flags,  5, 10, RADIO_USEFIELD);
  effects[5]->useFlag(&header.flags, 11);
  effects[2]->useField(5, 10);
  effects[3]->useField(5, 10);
  effects[4]->useField(5, 10);
  
  farColor->setColor(RGB(header.farColorR, header.farColorG, header.farColorB));
  clearColor->setColor(RGB(header.vramFillR, header.vramFillG, header.vramFillB));
  
  depth->useValue((long*)&header.visibilityDepth, 0x1000);
  clearHeight->useValue((short*)&header.vramFillHeight);  
  
  //parent->mainScene->
  //location[0].setValue(
}