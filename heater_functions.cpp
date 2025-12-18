#include "heater_functions.h"
#include "hw_config.h"
#include <Arduino.h>
#include <Adafruit_MCP9600.h>
#include <PID_v2.h>

const int windowSize = 5000;
unsigned long windowStartTime;

Adafruit_MCP9600 mcp;
double setPoint, input, output;
double Kp = 2, Ki = 5, Kd = 1;
PID_v2 heater(Kp, Ki, Kd, PID::Direct);

void heater_setup() {
  pinMode(ssr_1_1, OUTPUT);
  pinMode(ssr_1_2, OUTPUT);
  pinMode(ssr_1_3, OUTPUT);

  digitalWrite(ssr_1_1, LOW);
  digitalWrite(ssr_1_2, LOW);
  digitalWrite(ssr_1_3, LOW);
}

void heater_start() {
  if (!mcp.begin(MCP9600_I2C_ADDRESS)) {
    Serial.println("MCP9600 not found");
    while (1);
  }

  mcp.setADCresolution(MCP9600_ADCRESOLUTION_18);
  mcp.setThermocoupleType(MCP9600_TYPE_K);
  mcp.setFilterCoefficient(3);
  mcp.enable(true);

  heater.Start(input, output, setPoint);
  heater.SetOutputLimits(0, windowSize);  

  windowStartTime = millis();
}

void heater_end() {
  heater_setTempDesired(0);
  digitalWrite(ssr_1_1, LOW);
  digitalWrite(ssr_1_2, LOW);
  digitalWrite(ssr_1_3, LOW);
  mcp.enable(false);
}

void heater_doWork() {
  input = heater_getTempMeasured();
  output = heater.Run(input); 

  // anti-windup
  if(output > windowSize) output = windowSize;
  if(output < 0) output = 0;

  if ((millis() - windowStartTime) > windowSize) {
    windowStartTime += windowSize;
  }
  if ((millis() - windowStartTime) < output) {
    digitalWrite(ssr_1_1, HIGH);
    digitalWrite(ssr_1_2, HIGH);
    digitalWrite(ssr_1_3, HIGH);
  }
  else {
    digitalWrite(ssr_1_1, LOW);
    digitalWrite(ssr_1_2, LOW);
    digitalWrite(ssr_1_3, LOW);
  }
}

void heater_setTempDesired(int temp) {
  setPoint = temp;
  heater.Setpoint(temp);
}

int heater_getTempDesired() {
  return setPoint;
}

int heater_getTempMeasured() {
  int t = mcp.readThermocouple();
  return t;
}
