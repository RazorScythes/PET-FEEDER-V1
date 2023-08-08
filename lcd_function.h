#ifndef LCD_FUNCTION_H
#define LCD_FUNCTION_H

#include "Arduino.h"

class LCD_FUNCTION{
    public:
        LCD_FUNCTION();        
        void SETUP();
        void text_center(const char* str, int line);
        void set_cursor(uint8_t  start, uint8_t  line);
        void create_char(uint8_t  digit, byte  custom[]);
        void print_out(String str, int line);
        void menu_selector(String str, int line);
        void write_custom_text(byte custom, int start, int line);
        void write_custom_text_center(byte custom, String str, int start, int line);
        void write_text(String str, int start, int line);
        void clear_screen();
};

#endif
