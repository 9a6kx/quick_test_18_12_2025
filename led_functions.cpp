#include "hw_config.h"
#include "led_functions.h"
#include <Arduino.h> 

static unsigned long blinkStartTime[4] = {0};
static unsigned long previousMillis[4] = {0};
static const unsigned long blinkDuration = 3000;
static const unsigned long interval = 500; 

static int led_idToPin(LedId id) {
  switch (id) {
    case LED_RUNNING: return running_led;
    case LED_STOPPED: return stopped_led;
    case LED_BINFULL: return bin_full_led;
    case LED_MEMORY1: return memory_1_led;
    case LED_MEMORY2: return memory_2_led;
    case LED_MEMORY3: return memory_3_led;
    case LED_MEMORY4: return memory_4_led;
    default: return -1;
  }
}

void led_setup() {
  pinMode(led_idToPin(LED_RUNNING), OUTPUT);
  pinMode(led_idToPin(LED_STOPPED), OUTPUT);
  pinMode(led_idToPin(LED_BINFULL), OUTPUT);
  pinMode(led_idToPin(LED_MEMORY1), OUTPUT);
  pinMode(led_idToPin(LED_MEMORY2), OUTPUT);
  pinMode(led_idToPin(LED_MEMORY3), OUTPUT);
  pinMode(led_idToPin(LED_MEMORY4), OUTPUT);
}

void led_off(LedId id) {
  int pin = led_idToPin(id);
  if (pin >= 0) digitalWrite(pin, LOW);
}

void led_on(LedId id) {
  int pin = led_idToPin(id);
  if (pin >= 0) digitalWrite(pin, HIGH);
}

static void flash(LedId id) {
  int pin = led_idToPin(id);
  digitalWrite(pin, !digitalRead(pin));
}

void led_flash(LedId id) {
  int i = id - LED_MEMORY1;
  if (i >= 0 && i < 4) {
    blinkStartTime[i] = millis();
    previousMillis[i] = millis();
  }
}

void led_doWork() {
  unsigned long currentMillis = millis();
  
  for (int i = 0; i < 4; i++) {
    if (blinkStartTime[i] != 0) {
      if (currentMillis - blinkStartTime[i] >= blinkDuration) {
        blinkStartTime[i] = 0;
        previousMillis[i] = 0;
        led_off((LedId)(LED_MEMORY1 + i));
      } 
      else if (currentMillis - previousMillis[i] >= interval) {
        previousMillis[i] = currentMillis;
        flash((LedId)(LED_MEMORY1 + i));
      }
    }
  }
}