#ifndef DRIVE_FUNCTIONS_H
#define DRIVE_FUNCTIONS_H

typedef enum MotorDir {
  LEFT,
  RIGHT
};

void drive_setup();
void drive_setSpeed(int speed);
void drive_start();
void drive_stop();
int drive_getSpeed();

#endif