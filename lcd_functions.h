#ifndef LCD_FUNCTIONS_H
#define LCD_FUNCTIONS_H

typedef enum {
  LCD_ITEM_TEMPERATURE_CURRENT,
  LCD_ITEM_TEMPERATURE_SET,
  LCD_ITEM_BELT,
  LCD_ITEM_DRIVE,
  LCD_ITEM_NONE,
} LcdItem;

void lcd_setup();
void lcd_updateState(const char* state); //First row changes based on the machine state
void lcd_showMessage(const char* message); //MEMORISED in MEMORY LOADED
void lcd_startHighlightRow(int row);
void lcd_endHighlightRow();
void lcd_setValue(LcdItem item, const char* value);
void lcd_beginUpdate(LcdItem item);
void lcd_endUpdate();
void lcd_doWork();

#endif