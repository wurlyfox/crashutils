#ifndef CONTROL_H
#define CONTROL_H

// psx controller emulator

#define VK_RETURN 0x0D
#define VK_SHIFT  0x10

#define VK_LEFT   0x25
#define VK_UP     0x26
#define VK_RIGHT  0x27
#define VK_DOWN   0x28
#define VK_Z      0x5A
#define VK_X      0x58
#define VK_C      0x43
#define VK_V      0x56
#define VK_A      0x41
#define VK_Q      0x51
#define VK_S      0x53
#define VK_W      0x57

#define VK_SEMICOLON 0xBA

class control
{
  private:

  bool *keys;
  bool active;
  
  public:
  
  control();

  void setKeyboardBuffer(bool *_keys);

  unsigned long readControls();
 
  bool isActive();
  bool toggleActive(); 
};

#endif