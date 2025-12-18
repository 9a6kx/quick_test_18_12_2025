#ifndef BINFULL_FUNCTIONS_H
#define BINFULL_FUNCTIONS_H

typedef enum {
  BINFULL,
  NOT_BINFULL,
} BinState;

BinState binfull_getState();

#endif