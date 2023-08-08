#include "Arduino.h"
#include "lcd_function.h"
#include "init_setup.h"
#include <EEPROM.h>

LCD_FUNCTION lcd_2;

SETUP_SELECTOR::SETUP_SELECTOR(){init();}

int delay_ = 200;

int SETUP_SELECTOR::init_offline_selection(int BTN_1, int BTN_2, int Quick_BTN, int selector){
    lcd_2.text_center("OFFLINE MODE?",0);
    lcd_2.set_cursor(0, 0);
    
    if(digitalRead(BTN_1) == LOW){
      if(selector == 1){
          lcd_2.print_out("  YES     >NO", 1);
          selector = 0;
      }
      else{
          lcd_2.print_out(" >YES      NO", 1);
          selector = 1;
      }
    }
    else if(digitalRead(BTN_2) == LOW){
      if(selector == 1){
          lcd_2.print_out("  YES     >NO", 1);
          selector = 0;
      }
      else{
          lcd_2.print_out(" >YES      NO", 1);
          selector = 1;
      }
    }
    
    if(digitalRead(Quick_BTN) == LOW){
      lcd_2.clear_screen();
      if(selector == 0){
        lcd_2.text_center("DEVICE WILL", 0);
        lcd_2.text_center("RESTART", 1);  
        delay(3000);  
        lcd_2.clear_screen();
        ESP.restart();
      }
      else{
        //wifi_failed_connection = false;
        //offline_mode = true;
        EEPROM.write(3, 1);
        EEPROM.commit();
      }
    }

    delay(delay_);    
    return selector;
}


int SETUP_SELECTOR::init_quick_dispense_setup(int BTN_1, int BTN_2, int Quick_BTN, int selector){
    lcd_2.text_center("QUICK DISPENSE", 0);
    lcd_2.print_out(">L   R   MIXED ", 1);
    if(digitalRead(BTN_1) == LOW){
      if(selector == 1){
          lcd_2.print_out(">L   R   MIXED ", 1);
          selector = 2;
      }
      else if(selector == 2){
          lcd_2.print_out(" L  >R   MIXED ", 1);
          selector = 3;
      }
      else if(selector == 3){
          lcd_2.print_out(" L   R  >MIXED ", 1);
          selector = 1;
      }
      delay(delay_);
    }
    else if(digitalRead(BTN_2) == LOW){
      if(selector == 1){
          lcd_2.print_out(">L   R   MIXED ", 1);
          selector = 2;
      }
      else if(selector == 2){
          lcd_2.print_out(" L  >R   MIXED ", 1);
          selector = 3;
      }
      else if(selector == 3){
          lcd_2.print_out(" L   R  >MIXED ", 1);
          selector = 1;
      }
      delay(delay_);
    }

    if(digitalRead(Quick_BTN) == LOW){
      String container = "";
      
      lcd_2.clear_screen();
      
      if(selector == 1){container = "Mixed";}
      else if(selector == 2){container = "Left Container";}
      else if(selector == 3){container = "Right Container";}
      
      EEPROM.write(selector, 2);
      EEPROM.commit();

      lcd_2.text_center("QM is set to", 0);
      lcd_2.text_center(container.c_str(), 1);

    delay(delay_);
  }
  return selector;
}
