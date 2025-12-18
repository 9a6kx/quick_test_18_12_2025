#ifndef ROTARYBTN_FUNCTIONS_H
#define ROTARYBTN_FUNCTIONS_H

typedef enum {
  ROTBTN_EVENT_NOCHANGE,
  ROTBTN_EVENT_UP,
  ROTBTN_EVENT_DOWN,
  ROTBTN_EVENT_PRESS,
} RotaryBtnEvent;

void rotarybtn_setup();
void rotarybtn_doWork();
RotaryBtnEvent rotarybtn_getEvent();
void rotarybtn_clearEvent();

#endif