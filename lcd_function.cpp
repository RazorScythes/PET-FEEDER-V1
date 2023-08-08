#include "Arduino.h"
#include "lcd_function.h"
#include <LiquidCrystal_I2C.h>

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch 

LiquidCrystal_I2C lcd_screen(0x27, lcdColumns, lcdRows); 

LCD_FUNCTION::LCD_FUNCTION(){init();}

void LCD_FUNCTION::SETUP(){
  // initialize LCD
  lcd_screen.init();
  // turn on LCD backlight                      
  lcd_screen.backlight();
}

void LCD_FUNCTION::text_center(const char* str, int line){
  String text = String(str);
  int strlen_ = String(str).length();
  int col = int((16 - strlen_) / 2);

  for(int i = 0; i < col; i++){
    text = " " + text + " ";
  }

  lcd_screen.setCursor(0,line);
  lcd_screen.print(text);
}

void LCD_FUNCTION::menu_selector(String str, int line){
  String text = str;
  int strlen_ = str.length();
  int col = int((14 - strlen_) / 2);

  for(int i = 0; i < col; i++){
    text = " " + text + " ";
  }

  if(strlen_ % 2 == 0) text = "<" + text + ">";
  else text = "<" + text + " >";
  
  lcd_screen.setCursor(0,line);
  lcd_screen.print(text);
}

void LCD_FUNCTION::set_cursor(byte start, byte line){
  lcd_screen.setCursor(start, line);
}

void LCD_FUNCTION::print_out(String str, int line){
  lcd_screen.setCursor(0, line);
  lcd_screen.print(str);
}

void LCD_FUNCTION::write_custom_text(byte custom, int start, int line){
  lcd_screen.setCursor(start, line);
  lcd_screen.write(custom);
}

void LCD_FUNCTION::write_custom_text_center(byte custom, String str, int start, int line){
  String text = String(str);
  int strlen_ = String(str).length();
  int col = int(((16 - strlen_) - 1) / 2);

  lcd_screen.setCursor(col,line);
  lcd_screen.write(custom);
  lcd_screen.setCursor(col+1,line);
  lcd_screen.print(text);
}

void LCD_FUNCTION::write_text(String str, int start, int line){
  lcd_screen.setCursor(start, line);
  lcd_screen.print(str);
}

void LCD_FUNCTION::clear_screen(){
  lcd_screen.clear();
}

void LCD_FUNCTION::create_char(uint8_t  digit, byte  custom[]){
  lcd_screen.createChar(digit, custom);
}
