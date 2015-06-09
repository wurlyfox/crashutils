#include "control.h"

control::control()
{
  keys   = 0;
  active = false;
}

void control::setKeyboardBuffer(bool *_keys)
{
  keys = _keys;
}

unsigned long control::readControls()
{
  if (!active) { return 0; }
  
  unsigned short ctrlBth = 0;
  if (!keys[VK_SHIFT])
  {
    ctrlBth = (keys['Q']          <<  0)  | 
              (keys['W']          <<  1)  | 
              (keys['A']          <<  2)  | 
              (keys['S']          <<  3)  |
              (keys['V']          <<  4)  |
              (keys['C']          <<  5)  |
              (keys['Z']          <<  6)  |
              (keys['X']          <<  7)  |
              (keys['B']          <<  8)  |
              (keys[VK_RETURN]    << 11)  | 
              (keys['O']          << 12)  |
              (keys[VK_SEMICOLON] << 13)  |
              (keys['L']          << 14)  |
              (keys['K']          << 15);
  }
                             
  unsigned long controls = (ctrlBth << 16) | ctrlBth;
  
  return controls;
}

bool control::toggleActive()
{
  active = !active;
}

bool control::isActive()
{
  return active;
}

