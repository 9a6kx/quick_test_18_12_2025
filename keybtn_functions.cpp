#include "keybtn_functions.h"
#include "hw_config.h"
#include <Arduino.h> 

#define BTN_ON HIGH
#define BTN_OFF LOW

static KeybtnState lastButtonState = KEYBTN_INACTIVE;
static KeybtnState currentButtonState = KEYBTN_INACTIVE;
static int lastReading = BTN_OFF;

static unsigned long lastDebounceTime = 0;
static const unsigned long debounceDelay = 50;

void keybtn_setup() {
  pinMode(start_stop_key_pushbutton, INPUT);
}

KeybtnState keybtn_getState() {
  int reading = digitalRead(start_stop_key_pushbutton);

  if (reading != lastReading) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading == BTN_ON)
      currentButtonState = KEYBTN_ACTIVE;
    else
      currentButtonState = KEYBTN_INACTIVE;
  }

  lastReading = reading;
  lastButtonState = currentButtonState;

  return currentButtonState;
}