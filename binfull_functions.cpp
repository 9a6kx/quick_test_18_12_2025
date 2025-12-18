#include "binfull_functions.h"
#include "hw_config.h"
#include <Arduino.h>

BinState binfull_getState() {
  if(bin_full_led == HIGH) {
    return BINFULL;
  }
  else {
    return NOT_BINFULL;
  }
}