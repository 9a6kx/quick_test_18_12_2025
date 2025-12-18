#ifndef KEYBTN_FUNCTIONS_H
#define KEYBTN_FUNCTIONS_H

typedef enum{
  KEYBTN_ACTIVE,
  KEYBTN_INACTIVE,
} KeybtnState;

void keybtn_setup();
KeybtnState keybtn_getState();

#endif