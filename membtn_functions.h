#ifndef MEMBTN_FUNCTIONS_H
#define MEMBTN_FUNCTIONS_H

typedef enum {
  MEMBTN_EVENT_NOCHANGE,
  MEMBTN_EVENT_PRESS,
  MEMBTN_EVENT_LONGPRESS,
} MemBtnEvent; 

typedef enum {
  MEMBTN_NONE,
  MEMBTN_1,
  MEMBTN_2,
  MEMBTN_3,
  MEMBTN_4,
} MemBtnItem;

void membtn_setup();
void membtn_doWork();
MemBtnEvent membtn_getEvent(MemBtnItem* item);
void membtn_clearEvent();

#endif