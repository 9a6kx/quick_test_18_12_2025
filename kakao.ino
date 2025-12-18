#include "hw_config.h"
#include "binfull_functions.h"
#include "led_functions.h"
#include "lcd_functions.h"
#include "keybtn_functions.h"
#include "rotarybtn_functions.h"
#include "membtn_functions.h"
#include "drive_functions.h"
#include "output_control.h"
#include "settings_functions.h"
#include "belt_functions.h"
#include "heater_functions.h"

typedef enum {
  STOPPED,
  RUNNING,
  BIN_FULL
} State;

typedef enum {
  MENU_INACTIVE,
  MENU_SELECT_ROW,
  MENU_EDIT_VALUE,
} MenuState;

static const char* stateText[] = {
  "STOPPED",
  "RUNNING",
  "BIN FULL"
};

const int DEFAULT_DRIVE_SPEED = 100;
const int DEFAULT_BELT_SPEED = 100;     
const int DEFAULT_TEMP = 100;  

const int TEMP_MIN = 0;     
const int TEMP_MAX = 300;   
const int SPEED_MIN = 80;
const int SPEED_MAX = 120;
const int BELT_MIN = 80;
const int BELT_MAX = 120;

State machineState = STOPPED;
State nextMachineState;

MenuState menuState = MENU_INACTIVE;

int value;
int selectedRow = 2;
int minRow = 2;
int maxRow = 4;

//BINFULL
void binfull_Enter() {
  led_on(LED_BINFULL); 
  lcd_updateState("BIN FULL");             
  belt_stop();                             
  heater_end();                     
  drive_stop(); 
}

State binfull_doWork() {
  BinState bin = binfull_getState();
  if(bin == NOT_BINFULL) return STOPPED;
  return BIN_FULL;
}

void binfull_Exit() {
  led_off(LED_BINFULL); 
}

//STOPPED
void stopped_Enter() {
  led_on(LED_STOPPED);
  lcd_updateState(stateText[STOPPED]); 
}

State stopped_doWork() {
  lcd_doWork();
  led_doWork();
  membtn_doWork();
  rotarybtn_doWork();

  //ENCODER
  RotaryBtnEvent rotaryEvent = rotarybtn_getEvent();
  switch(menuState) {

    case MENU_INACTIVE:
      if(rotaryEvent == ROTBTN_EVENT_PRESS) {
        menuState = MENU_SELECT_ROW;
        lcd_startHighlightRow(selectedRow);
      }
    break;

    case MENU_SELECT_ROW:

      switch(rotaryEvent) {

      case ROTBTN_EVENT_UP:
        selectedRow++;
        if(selectedRow > maxRow) selectedRow = minRow;
        lcd_startHighlightRow(selectedRow);
        break;
    
      case ROTBTN_EVENT_DOWN:
        selectedRow--;
        if(selectedRow < minRow) selectedRow = maxRow;
        lcd_startHighlightRow(selectedRow);
        break;
    
      case ROTBTN_EVENT_PRESS:
        menuState = MENU_EDIT_VALUE;
        lcd_endHighlightRow();
        switch(selectedRow) {
          case 2: 
            value = heater_getTempDesired();
            break;
          case 3: 
            value = drive_getSpeed();        
            break;
          case 4: 
            value = belt_getSpeed();         
            break;
        }
        break;
    }
    break;

    case MENU_EDIT_VALUE:
      if(rotaryEvent != ROTBTN_EVENT_PRESS) {
        lcd_beginUpdate(LcdItem(selectedRow));
        
        if(rotaryEvent == ROTBTN_EVENT_UP) value++;
        else if(rotaryEvent == ROTBTN_EVENT_DOWN) value--; 

        if(selectedRow == 2) value = constrain(value, TEMP_MIN, TEMP_MAX);
        if(selectedRow == 3) value = constrain(value, SPEED_MIN, SPEED_MAX);
        if(selectedRow == 4) value = constrain(value, BELT_MIN, BELT_MAX);
        
        char val[4];
        sprintf(val, "%d", value);
        lcd_setValue(LcdItem(selectedRow), val);
      }

      else {
        menuState = MENU_INACTIVE;
        switch(selectedRow) {
          case 2: 
            heater_setTempDesired(value); 
            break;
          case 3: 
            drive_setSpeed(value); 
            break;
          case 4: 
            belt_setSpeed(value); 
            break;      
        }
      }
      rotarybtn_clearEvent();  
  }    

  //MEMORY BUTTONS
  MemBtnItem item;
  MemBtnEvent memEvent = membtn_getEvent(&item);
  if(MENU_INACTIVE && memEvent != MEMBTN_EVENT_NOCHANGE) {
    if(memEvent == MEMBTN_EVENT_PRESS) {
      
      String message = "Memory #" + String(item) + " loaded";
      const char* msg = message.c_str(); 
      lcd_showMessage(msg);

      Setting setting;
      SettingResult result = settings_load(item, &setting);
      if(result == SETTINGS_RES_CRC_ERR) {
        Serial.println("Memory full error");
      }
      else {
        belt_setSpeed(setting.beltSpeed);
        drive_setSpeed(setting.driveSpeed);
        heater_setTempDesired(setting.temp);

        String belt = String(setting.beltSpeed);
        String drive = String(setting.driveSpeed);
        String temp = String(setting.temp);
        lcd_setValue(LCD_ITEM_BELT, belt.c_str());
        lcd_setValue(LCD_ITEM_DRIVE, drive.c_str());
        lcd_setValue(LCD_ITEM_TEMPERATURE_SET, temp.c_str());
      }
    }

    else {
      
      led_flash(LedId(item + 2));
      String message = "Memorised #" + String(item);
      const char* msg = message.c_str(); 
      lcd_showMessage(msg);

      Setting currentSetting;
      currentSetting.temp = heater_getTempDesired();
      currentSetting.beltSpeed = belt_getSpeed();
      currentSetting.driveSpeed = drive_getSpeed();
      SettingResult result = settings_save(item, currentSetting);
      if(result == SETTINGS_RES_FULL_ERR) {
        Serial.println("Memory full error");
      }
    }
  }
  membtn_clearEvent();

  BinState bin = binfull_getState();
  if(bin == BINFULL) return BIN_FULL;
  KeybtnState state = keybtn_getState();
  if(state == KEYBTN_ACTIVE) return RUNNING;
  return STOPPED;
}

void stopped_Exit() {
  led_off(LED_STOPPED);
}

//RUNNING
void running_Enter() {
  led_on(LED_RUNNING);
  lcd_updateState(stateText[RUNNING]); 
  output_start();
  belt_start();
  heater_start();
  drive_start();
}

State running_doWork() {
  belt_doWork();
  heater_doWork();

  //current temp
  int currentTemp = heater_getTempMeasured();
  char val[4];
  sprintf(val, "%d", currentTemp);
  lcd_setValue(LCD_ITEM_TEMPERATURE_CURRENT ,val);

  KeybtnState state = keybtn_getState();
  if(state == KEYBTN_INACTIVE) return STOPPED;
  BinState bin = binfull_getState();
  if(bin == BINFULL) return BIN_FULL;
  return RUNNING;

}

void running_Exit() {
  led_off(LED_RUNNING);
  output_stop();
  belt_stop();
  heater_end();
  drive_stop();
}

void setup()
{
    // Serijska komunikacija za debug
    Serial.begin(9600);

    lcd_setup();
    led_setup();
    keybtn_setup();
    membtn_setup();
    belt_setup();
    drive_setup();
    rotarybtn_setup();
    heater_setup();
    output_setup();

    drive_setSpeed(DEFAULT_DRIVE_SPEED);
    belt_setSpeed(DEFAULT_BELT_SPEED);
    heater_setTempDesired(DEFAULT_TEMP);

    machineState = STOPPED;
}

void loop()
{
  output_doWork();

  switch(machineState) {
    case STOPPED:
      nextMachineState = stopped_doWork();
      break;
    case RUNNING:
      nextMachineState = running_doWork();
      break;
    case BIN_FULL:
      nextMachineState = binfull_doWork();
      break;
  }

  if(nextMachineState != machineState) {
    switch(machineState) {
      case STOPPED:
        stopped_Exit();
        break;
      case RUNNING:
        running_Exit();
        break;
      case BIN_FULL:
        binfull_Exit();
        break;
    }
    switch(nextMachineState) {
      case STOPPED:
        stopped_Enter();
        break;
      case RUNNING:
        running_Enter();
        break;
      case BIN_FULL:
        binfull_Enter();
        break;
    }
    machineState = nextMachineState;
  }
}
