#include "output_control.h"
#include "hw_config.h"
#include <Arduino.h>

#define VIBRO_ON HIGH
#define VIBRO_OFF LOW
#define FAN_ON HIGH
#define FAN_OFF LOW

#define VENT_DELAY 30000
#define VENT_STOPPED_DURATION 30000
#define VIBROMTR_DELAY 300000
#define VIBROMTR_RUNNING_DURATION 45000
#define VIBROMTR_STOPPED_DURATION 30000

OutputState stateChange = NOT_SET;
static unsigned long startTimeVent = 0;
static unsigned long stopTimeVent = 0;
static unsigned long startTimeMtr = 0;
static unsigned long stopTimeMtr = 0;
static unsigned long vibroMtrStartTime = 0;
static unsigned long vibroMtrStopTime = 0;

void output_setup() {
  pinMode(pcb_vent, OUTPUT);
  pinMode(ssr_vent, OUTPUT);
  pinMode(turbine, OUTPUT);
  pinMode(vibro_motor, OUTPUT);
}

void output_start() {
  digitalWrite(pcb_vent, HIGH);
  digitalWrite(ssr_vent, HIGH);
  stateChange = STARTED;
}

void output_doWork() {
  unsigned long now = millis();

  if(stateChange == STARTED) {
    startTimeVent = now;
    startTimeMtr = now;
    stateChange = NOT_SET;
  }
  else if(stateChange == ENDED) {
    stopTimeVent = now;
    stopTimeMtr = now;
    stateChange = NOT_SET;
  }

  //TURBINE
  if (startTimeVent && now - startTimeVent >= VENT_DELAY) {
    digitalWrite(turbine, FAN_ON);
    startTimeVent = 0;
  }
  if (stopTimeVent && now - stopTimeVent >= VENT_STOPPED_DURATION) {
    digitalWrite(turbine, FAN_OFF);
    stopTimeVent = 0;
  }

  //VIBROMOTOR - ON
  if (startTimeMtr && now - startTimeMtr >= VIBROMTR_DELAY) {
    digitalWrite(vibro_motor, VIBRO_ON);
    vibroMtrStartTime = now;
    startTimeMtr = 0;
  }

  //VIBROMOTOR - OFF
  if (stopTimeMtr && now - stopTimeMtr >= VIBROMTR_DELAY) {
    digitalWrite(vibro_motor, VIBRO_OFF);
    vibroMtrStopTime = now;
    stopTimeMtr = 0;
  }

  //VIBROMOTOR AFTER TURNED ON
  if (vibroMtrStartTime && now - vibroMtrStartTime >= VIBROMTR_RUNNING_DURATION) {
    digitalWrite(vibro_motor, VIBRO_OFF);
    vibroMtrStartTime = 0;
    startTimeMtr = now;  //new cycle
  }

  //VIBROMOTOR AFTER TURNED OFF
  if (vibroMtrStopTime && now - vibroMtrStopTime >= VIBROMTR_STOPPED_DURATION) {
    digitalWrite(vibro_motor, VIBRO_OFF);
    vibroMtrStopTime = 0;
  }
}

void output_stop() {
  digitalWrite(pcb_vent, LOW);
  digitalWrite(ssr_vent, LOW);
  stateChange = ENDED;
}