#ifndef HEATER_FUNCTIONS_H
#define HEATER_FUNCTIONS_H

void heater_setup();
void heater_start();
void heater_end();
void heater_doWork();
void heater_setTempDesired(int temp);
int heater_getTempDesired();
int heater_getTempMeasured();

#endif