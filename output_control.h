#ifndef OUTPUT_CONTROL_H
#define OUTPUT_CONTROL_H

typedef enum {
  STARTED,
  ENDED,
  NOT_SET,
} OutputState;

void output_setup();
void output_start();
void output_stop();
void output_doWork();

#endif