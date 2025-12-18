#include "lcd_functions.h"
#include "hw_config.h"
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>

#define LCD_ROWS 4
#define LCD_COLUMNS 20

static const char* display[] = {
  "TEMP",
  "INPUT SPEED",
  "LINE SPEED"
};

static const char* messages[] = {
  "TEMPERATURE ADJUSTED",
  "",
  "INPUT SPEED SET",
  "LINE SPEED SET",
};

char currentValues[4][4] = {0};

const char* currentStateDisplay;

static LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, LCD_COLUMNS, LCD_ROWS); 

static LcdItem updatingItem = LCD_ITEM_NONE;
static bool messageActive = false;
static unsigned long messageStartTime = 0;
static const long messageDuration = 3000;

static int chosenRow = -1;
static bool textSeen = true;
static unsigned long previousMillis = 0;
static const long blinkInterval = 500;

void lcd_getState(const char* state) {
  currentStateDisplay = state;
}

void lcd_updateState(const char* currentStateDisplay) {
  char line[21], right[21], left[21];
  snprintf(line, sizeof(line), "--- %s ---", currentStateDisplay);
  int spaces = 20 - strlen(line);
  int leftSpaces = spaces / 2;
  int rightSpaces = spaces - leftSpaces;
  for(int i = 0; i < leftSpaces; i++) left[i] = ' ';
  left[leftSpaces] = '\0';
  memcpy(right, left, leftSpaces+1);
  if(rightSpaces > leftSpaces) right[rightSpaces - 1] = ' ';
  right[rightSpaces] = '\0';
  char result[21];
  result[0] = '\0';
  strcat(result, left);
  strcat(result, line);
  strcat(result, right);

  lcd.setCursor(0,0);
  lcd.print(result);
}

static void lcd_setupValueRows() {
  for(int i=0; i<3; i++) {
    lcd.setCursor(0,i+1);
    lcd.print(display[i]);
    lcd.print(": ");
    if(i == 0) {
      lcd.print("   /   ");
      lcd.print((char)223); //degree symbol
      lcd.print("C");
    }
    else {
      lcd.print("   %");
    }
  }
}

void lcd_setup() {
  lcd.init();
  lcd.backlight();
  lcd_updateState(currentStateDisplay);
  lcd_setupValueRows();
}

void lcd_showMessage(const char* message) {
  messageActive = true;
  messageStartTime = millis();

  char line[21], right[21], left[21];
  line[0] = '\0';
  strcat(line, message);
  int spaces = 20 - strlen(line);
  int leftSpaces = spaces / 2;
  int rightSpaces = spaces - leftSpaces;
  for(int i = 0; i < leftSpaces; i++) left[i] = ' ';
  left[leftSpaces] = '\0';
  memcpy(right, left, leftSpaces+1);
  if(rightSpaces > leftSpaces) right[rightSpaces-1] = ' ';
  right[rightSpaces] = '\0';
  char result[21];
  result[0] = '\0';
  strcat(result, left);
  strcat(result, line);
  strcat(result, right);

  lcd.setCursor(0,0);
  lcd.print(result);
}

void lcd_startHighlightRow(int row) {
  chosenRow = row;
  previousMillis = millis();
  textSeen = false;
  lcd.setCursor(0, chosenRow);
  lcd.print("                    ");
}

void lcd_endHighlightRow() {
  lcd.setCursor(0, chosenRow); 
  lcd.print(display[chosenRow - 1]);  
  chosenRow = -1;
  textSeen = true;
}

static int lcd_getValueCol(LcdItem item) {
  int col = 2;
  switch(item) {
    case LCD_ITEM_TEMPERATURE_CURRENT:
      return col + strlen(display[0]);

    case LCD_ITEM_TEMPERATURE_SET:
      return col + strlen(display[0]) + 4;

    case LCD_ITEM_BELT:
      return col + strlen(display[1]);

    case LCD_ITEM_DRIVE:
      return col + strlen(display[2]);

    default:
      return -1;
  }
}

void lcd_setValue(LcdItem item, const char* value) {
  snprintf(currentValues[item], sizeof(currentValues[item]), "%3s", value);
  int col = lcd_getValueCol(item);
  char val[4];
  snprintf(val, sizeof(val), "%3s", value); //rezervacija za tromestna števila
  lcd.setCursor(col, chosenRow);
  lcd.print(val);
}

void lcd_beginUpdate(LcdItem item) {
  updatingItem = item;
}

void lcd_endUpdate() { 
  if(updatingItem != LCD_ITEM_TEMPERATURE_CURRENT) lcd_showMessage(messages[updatingItem]);
  updatingItem = LCD_ITEM_NONE;
}

static void blink() {
  int row = chosenRow;
  int col = 0;

  if(updatingItem == LCD_ITEM_NONE) {
    lcd.setCursor(0,row);
    if(textSeen) {
      lcd.print("                    ");
    } else {
      lcd_setupValueRows();
      if(row == 1) {
        lcd.setCursor(lcd_getValueCol(LCD_ITEM_TEMPERATURE_CURRENT), row);
        lcd.print(currentValues[0]);
        lcd.setCursor(lcd_getValueCol(LCD_ITEM_TEMPERATURE_SET), row);
        lcd.print(currentValues[1]);
      }
      else if(row == 2) {
        lcd.setCursor(lcd_getValueCol(LCD_ITEM_BELT), row);
        lcd.print(currentValues[2]);
      }
      else {
        lcd.setCursor(lcd_getValueCol(LCD_ITEM_DRIVE), row);
        lcd.print(currentValues[3]);
      }
    }
    textSeen = !textSeen;
  }
}

void lcd_doWork() {
  unsigned long currentMillis = millis();

  if(messageActive && currentMillis - messageStartTime >= messageDuration) {
    messageActive = false;
    lcd_updateState(currentStateDisplay);
  }

  if(chosenRow != -1 && currentMillis - previousMillis >= blinkInterval) {
    previousMillis = currentMillis;
    blink();
  }
}


