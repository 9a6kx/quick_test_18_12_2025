#include "drive_functions.h"
#include "hw_config.h"
#include <Arduino.h>

#define MIN_PWM 204 
#define MAX_PWM 255

static int currentSpeed = 0;
static MotorDir currentDir = RIGHT;

void drive_setup() {
   pinMode(pwm_L_source, OUTPUT);
    pinMode(pwm_R_source, OUTPUT);
    pinMode(pwm_L_enable, OUTPUT);
    pinMode(pwm_R_enable, OUTPUT);

    analogWrite(pwm_L_source, 0);
    analogWrite(pwm_R_source, 0);
    digitalWrite(pwm_L_enable, HIGH); 
    digitalWrite(pwm_R_enable, HIGH);
}

static int drive_percentToPWM(int speed) {
  int pwmValue = map(speed, 80, 120, MIN_PWM, MAX_PWM);
  return pwmValue;
}

void drive_setSpeed(int speed) {
  int pwmValue = drive_percentToPWM(speed);
  if(currentDir == RIGHT) {
    analogWrite(pwm_R_source, pwmValue);
    analogWrite(pwm_L_source, 0);
  }
  else {
    analogWrite(pwm_L_source, pwmValue);
    analogWrite(pwm_R_source, 0);
  }
  currentSpeed = speed;
}

void drive_start() {
  drive_setSpeed(currentSpeed);
}

void drive_stop() {
  analogWrite(pwm_L_source, 0);
  analogWrite(pwm_R_source, 0);
  currentSpeed = 0;
}

int drive_getSpeed() {
  return currentSpeed;
}