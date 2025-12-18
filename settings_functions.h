#ifndef SETTINGS_FUNCTIONS_H
#define SETTINGS_FUNCTIONS_H

typedef enum {
  SETTINGS_RES_OK,
  SETTINGS_RES_CRC_ERR,
  SETTINGS_RES_FULL_ERR,
} SettingResult;

typedef struct {
  int temp;
  int beltSpeed;
  int driveSpeed;
} Setting;

SettingResult settings_load(int settingLocation, Setting* setting);
SettingResult settings_save(int settingLocation, Setting setting);

#endif