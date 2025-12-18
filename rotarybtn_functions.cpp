#include "rotarybtn_functions.h"
#include "hw_config.h"
#include <Arduino.h>

#define BTN_PRESSED HIGH
#define BTN_RELEASED LOW

static RotaryBtnEvent currentEvent = ROTBTN_EVENT_NOCHANGE;

static unsigned long lastDebounceTime = 0;
static const unsigned long debounceDelay = 50;
static int lastBtnState = BTN_RELEASED;
static int btnState = BTN_RELEASED;

static unsigned long lastEncoderDebounceTime = 0;
static int lastEncoded = 0;
static volatile int encoderPos = 0;
static int lastEncoderPos = 0;

void rotarybtn_setup() {
  pinMode(encoder_pin_A, INPUT_PULLUP);
  pinMode(encoder_pin_B, INPUT_PULLUP);
  pinMode(rotary_pushbutton, INPUT);
}

void rotarybtn_doWork() {
  //pushbutton
  int readingBtn = digitalRead(rotary_pushbutton);

  if(readingBtn != lastBtnState) {
    lastDebounceTime = millis();
  }

  if(millis() - lastDebounceTime > debounceDelay) {
    if(readingBtn != btnState) {
      btnState = readingBtn;
      if(btnState == BTN_PRESSED) currentEvent = ROTBTN_EVENT_PRESS;
    }
  }
  lastBtnState = btnState;

  //encoder
  if (millis() - lastEncoderDebounceTime < debounceDelay) return;
  lastEncoderDebounceTime = millis();

  int readingA = digitalRead(encoder_pin_A); 
  int readingB = digitalRead(encoder_pin_B); 
  int encoded = (readingA << 1) | readingB; 


  if(encoded != lastEncoded) {
    int sum = (lastEncoded << 2) | encoded;

    if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
      encoderPos--;
    }
    if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
      encoderPos++;
    }
    
    lastEncoded = encoded;
  }
}

RotaryBtnEvent rotarybtn_getEvent() {
  if (currentEvent != ROTBTN_EVENT_NOCHANGE) {
    RotaryBtnEvent e = currentEvent;
    currentEvent = ROTBTN_EVENT_NOCHANGE;
    return e;
  }
  return ROTBTN_EVENT_NOCHANGE;
}

void rotarybtn_clearEvent() {
  currentEvent = ROTBTN_EVENT_NOCHANGE;
}