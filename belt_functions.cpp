#include "belt_functions.h"
#include "hw_config.h"
#include <Arduino.h>

#define USE_DUERS485DMA
#define USE_RS485_SERIAL1 
#include <DUERS485DMA.h>
#include <DUE-ModbusDMA.h>

#define MIN_REGISTER 0
#define MAX_REGISTER 3000

DUERS485DMAClass RS485_SERIAL1(RS485_SERIAL1_CONFIGS);

bool change = false;
bool ok = false;
unsigned long startTime = 0;

ModbusRTUClientClass modbusClient(RS485_SERIAL1);

void belt_setup() {
  modbusClient.begin(MODBUS_SLAVE_ADDR, MODBUS_BAUD_RATE);
  pinMode(rs485_RX_TX, OUTPUT);
}

static int belt_speedToRegValue(int speed) {
  int regValue = map(speed, 80, 120, MIN_REGISTER, MAX_REGISTER);
  return regValue;
}

void belt_setSpeed(int speed) {
  int regValue = belt_speedToRegValue(speed);
  digitalWrite(rs485_RX_TX, HIGH);
  ok = modbusClient.holdingRegisterWrite(MODBUS_SLAVE_ADDR, MODBUS_SET_FREQUENCY, regValue);
  change = true;
}

void belt_start() {
  digitalWrite(rs485_RX_TX, HIGH);
  ok = modbusClient.holdingRegisterWrite(MODBUS_SLAVE_ADDR, MODBUS_RUN_STOP, INSTRUCTION_RUN);
  change = true;
}

void belt_stop() {
  digitalWrite(rs485_RX_TX, HIGH);
  ok = modbusClient.holdingRegisterWrite(MODBUS_SLAVE_ADDR, MODBUS_RUN_STOP, INSTRUCTION_STOP);
  change = true;
}

int belt_getSpeed() {
  digitalWrite(rs485_RX_TX, HIGH);
  int speed = modbusClient.holdingRegisterRead(MODBUS_SLAVE_ADDR, MODBUS_READ_FREQUENCY);
  change = true;
  return speed;
}

void belt_doWork() {
  if(change) {
    startTime = millis();
    change = false;
  }
  //delay
  if(startTime && (millis()-startTime) >= 5) {
    if(!ok) Serial.println("Modbus error");
    else {
      digitalWrite(rs485_RX_TX, LOW);
      startTime = 0;
      ok = false;
    }
  }
}

