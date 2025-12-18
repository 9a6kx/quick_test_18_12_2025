#include "membtn_functions.h"
#include "hw_config.h"
#include <Arduino.h>

#define BTN_PRESSED HIGH
#define BTN_RELEASED LOW

static MemBtnEvent currentEvent = MEMBTN_EVENT_NOCHANGE;
static MemBtnItem currentItem = MEMBTN_NONE; 

static unsigned int previousReading[4] = {BTN_RELEASED, BTN_RELEASED, BTN_RELEASED, BTN_RELEASED};
static unsigned long pressStartTime[4] = {0};
static unsigned long previousMillis[4] = {0};
static unsigned long lastDebounceTime[4] = {0};
static const unsigned long debounceDelay = 50;
static const unsigned long longPressDuration = 2000;
static const unsigned long interval = 500; 

void membtn_setup() {
  pinMode(memory_key_1, INPUT);
  pinMode(memory_key_2, INPUT);
  pinMode(memory_key_3, INPUT);
  pinMode(memory_key_4, INPUT);
}

static int membtn_itemToPin(MemBtnItem item) {
  switch (item) {
    case MEMBTN_1: return memory_key_1;
    case MEMBTN_2: return memory_key_2;
    case MEMBTN_3: return memory_key_3;
    case MEMBTN_4: return memory_key_4;
    default: return -1;
  }
}

void membtn_doWork() {
  for(int i=0; i<4; i++) {
    MemBtnItem btn = (MemBtnItem)(i+1);
    int reading = digitalRead(membtn_itemToPin(btn));

    if (reading != previousReading[i]) {
      lastDebounceTime[i] = millis();
      previousReading[i] = reading;
    }

    if (reading == BTN_PRESSED && pressStartTime[i] == 0 && millis() - lastDebounceTime[i] > debounceDelay) {
      pressStartTime[i] = millis();
      lastDebounceTime[i] = millis();
    }

    if(pressStartTime[i] != 0 && currentEvent == MEMBTN_EVENT_NOCHANGE) {
      if (millis() - pressStartTime[i] >= longPressDuration) {
        currentEvent = MEMBTN_EVENT_LONGPRESS;
        currentItem = btn;
        pressStartTime[i] = 0;
      }
      else if (reading == BTN_PRESSED) {
        currentEvent = MEMBTN_EVENT_PRESS;
        currentItem = btn;
        pressStartTime[i] = 0;
      }
    }
  }
}

MemBtnEvent membtn_getEvent(MemBtnItem* item) {
  if (currentEvent != MEMBTN_EVENT_NOCHANGE) {
    *item = currentItem;
    MemBtnEvent e = currentEvent;
    currentEvent = MEMBTN_EVENT_NOCHANGE;
    currentItem = MEMBTN_NONE;
    return e;
  }
  *item = MEMBTN_NONE;
  return MEMBTN_EVENT_NOCHANGE;
}

void membtn_clearEvent() {
  currentEvent = MEMBTN_EVENT_NOCHANGE;
  currentItem = MEMBTN_NONE;
}
