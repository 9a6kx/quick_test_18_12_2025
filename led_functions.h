#ifndef LED_FUNCTIONS_H
#define LED_FUNCTIONS_H

typedef enum {
  LED_RUNNING,
  LED_STOPPED,
  LED_BINFULL,
  LED_MEMORY1,
  LED_MEMORY2,
  LED_MEMORY3,
  LED_MEMORY4,
  NUM_LEDS,
} LedId;

void led_setup();
void led_off(LedId id);
void led_on(LedId id);
void led_flash(LedId id);
void led_doWork();

#endif