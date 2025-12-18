#include "settings_functions.h"
#include "hw_config.h"
#include <DueFlashStorage.h>
#include <CRC16.h>

DueFlashStorage dueFlashStorage;

// Flash locations for MEMORY
#define SETTING1_ADDR 0
#define SETTING2_ADDR (sizeof(Setting) + 2 + 1)//settings + CRC(2) + counter(1)
#define SETTING3_ADDR (2 * (sizeof(Setting) + 2 + 1))
#define SETTING4_ADDR (3 * (sizeof(Setting) + 2 + 1))

static int settings_locationToAddr(int location) {
    switch (location) {
        case 1: return SETTING1_ADDR;
        case 2: return SETTING2_ADDR;
        case 3: return SETTING3_ADDR;
        case 4: return SETTING4_ADDR;
        default: return -1;
    }
}

static int settings_counterCheck(int location) {
  int addr = settings_locationToAddr(location);
  byte* ptr = dueFlashStorage.readAddress(addr + sizeof(Setting) + 2);
  byte counter = *ptr;

  if(counter == 0xFF) {
    counter = 0;
  }

  if(counter >= 4) {
    Serial.print("Memory position #");
    Serial.print(location);
    Serial.println(" full");
    return -1;
  }

  return counter;
}

SettingResult settings_load(int settingLocation, Setting* setting) {
  int addr = settings_locationToAddr(settingLocation);
  byte* b = dueFlashStorage.readAddress(addr);
  uint16_t storedCRC = b[sizeof(Setting)] | (b[sizeof(Setting) + 1] << 8);
  
  Setting settingFromFlash;
  memcpy(&settingFromFlash, b, sizeof(Setting));
  *setting = settingFromFlash;

  CRC16 crcCalc;
  crcCalc.add(b, sizeof(Setting));
  uint16_t calcCRC = crcCalc.getCRC();

  if(calcCRC == storedCRC) {
      return SETTINGS_RES_OK;
  } 
  else {
    return SETTINGS_RES_CRC_ERR;
  }
}

SettingResult settings_save(int settingLocation, Setting setting) {
  int counter = settings_counterCheck(settingLocation);
  if(counter == -1) return SETTINGS_RES_FULL_ERR;
  int addr = settings_locationToAddr(settingLocation);
  byte b2[sizeof(Setting)]; 
  memcpy(b2, &setting, sizeof(Setting));

  CRC16 crcCalc;
  crcCalc.add(b2, sizeof(Setting));
  uint16_t crcValue = crcCalc.getCRC();
  byte crcBytes[2];
  crcBytes[0] = crcValue & 0xFF; // low byte
  crcBytes[1] = (crcValue >> 8) & 0xFF; // high byte

  dueFlashStorage.write(addr, b2, sizeof(Setting));
  dueFlashStorage.write(addr + sizeof(Setting), crcBytes, 2);
  counter++;
  byte counterByte = (byte)counter;
  dueFlashStorage.write(addr + sizeof(Setting) + 2, &counterByte, 1);
  return SETTINGS_RES_OK;
}
