//#include <LiquidCrystal_I2C.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <Servo.h>
#include <WiFi.h>
#include <EEPROM.h>
#include <DNSServer.h>
#include <WiFiManager.h>

#define EEPROM_SIZE 512

//#include "init_setup.h"
#include "lcd_function.h"

Servo myservo;
Servo myservo2;

LCD_FUNCTION lcd;
//SETUP_SELECTOR init_setup;

#define BTN_1       25
#define BTN_2       26
#define BTN_3       4
#define BTN_4       16
#define Quick_BTN   33
#define BUZZER      17

//wifi config manager
bool wifi_connected = false;
bool wifi_failed_connection = false;
//bool offline_mode = false;

//// set the LCD number of columns and rows
//int lcdColumns = 16;
//int lcdRows = 2;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
//LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);


byte Check[8] = {
    B00000,
    B00000,
    B00001,
    B00011,
    B10110,
    B11100,
    B01000,
    B00000
};

byte Cross[8] = {
    B00000,
    B00000,
    B10001,
    B01010,
    B00100,
    B01010,
    B10001,
    B00000
};

byte Wifi[8] = {
    B00000,
    B11100,
    B00010,
    B11001,
    B00101,
    B10101,
    B00000,
    B00000
};

String ssid_, ip_;

int Monday[9] = {11, 12, 13, 14, 15, 16, 17, 18, 19};
/*
    EEPROM STATE
    1   = AUTOMATICALLY RESTART IF WIFI CONNECTION IS FAILED
    2   = QUICK DISPENSE STATE
    3   = WIFI STATUS
    4   = SCHEDULER STATUS
    5   = DISABLE SCHEDULER
    6   = QUICK DISPENSE DELAY
    7   = 
    8   =
    9   =
    10  =

    MONDAY
    11 - MORNING
    12 - MORNING
    13 - MORNING REFERENCE TIME
    14 - NOON
    15 - NOON
    16 - NOON REFERENCE TIME
    17 - EVENING
    18 - EVENING
    19 - EVENING REFERENCE TIME

    TUESDAY
    20 - MORNING
    21 - MORNING
    22 - MORNING REFERENCE TIME
    23 - NOON
    24 - NOON
    25 - NOON REFERENCE TIME
    26 - EVENING
    27 - EVENING
    28 - EVENING REFERENCE TIME

    WEDNESDAY
    29 - MORNING
    30 - MORNING
    31 - MORNING REFERENCE TIME
    32 - NOON
    33 - NOON
    34 - NOON REFERENCE TIME
    35 - EVENING
    36 - EVENING
    37 - EVENING REFERENCE TIME

    THURSDAY
    38 - MORNING
    39 - MORNING
    40 - MORNING REFERENCE TIME
    41 - NOON
    42 - NOON
    43 - NOON REFERENCE TIME
    44 - EVENING
    45 - EVENING
    46 - EVENING REFERENCE TIME

    FRIDAY
    47 - MORNING
    48 - MORNING
    49 - MORNING REFERENCE TIME
    50 - NOON
    51 - NOON
    52 - NOON REFERENCE TIME
    53 - EVENING
    54 - EVENING
    55 - EVENING REFERENCE TIME

    SATURDAY
    56 - MORNING
    57 - MORNING
    58 - MORNING REFERENCE TIME
    59 - NOON
    60 - NOON
    61 - NOON REFERENCE TIME
    62 - EVENING
    63 - EVENING
    64 - EVENING REFERENCE TIME

    SUNDAY
    65 - MORNING
    66 - MORNING
    67 - MORNING REFERENCE TIME
    68 - NOON
    69 - NOON
    70 - NOON REFERENCE TIME
    71 - EVENING
    72 - EVENING
    73 - EVENING REFERENCE TIME
*/

void connection_failed() {
  wifi_failed_connection = true;

  lcd.clear_screen();

  lcd.text_center("--WiFi Status--", 0);
  lcd.text_center("Not Connected", 1);

  if (EEPROM.read(1) == true) {
    delay(5000);
    lcd.text_center("DEVICE WILL", 0);
    lcd.text_center("RESTART", 1);
    delay(3000);
    lcd.clear_screen();
    ESP.restart();
  }
}

void setup() {
  myservo.write(0);
  myservo2.write(180);
  Serial.begin(115200);

  EEPROM.begin(EEPROM_SIZE);

  WiFiManager wifiManager;

  pinMode(BTN_1, INPUT_PULLUP);
  pinMode(BTN_2, INPUT_PULLUP);
  pinMode(BTN_3, INPUT_PULLUP);
  pinMode(BTN_4, INPUT_PULLUP);
  pinMode(Quick_BTN, INPUT_PULLUP);

  //playScale(BUZZER, 0);

  myservo2.attach(13);
  myservo.attach(14);

  myservo.write(0);
  myservo2.write(80);
  
  //EEPROM_Reset();
  //EEPROM_Read();
  // initialize LCD
  //  lcd.init();
  //  // turn on LCD backlight
  //  lcd.backlight();
  lcd.SETUP();

  //Creating Custom Characters
  lcd.create_char(0, Check);
  lcd.create_char(1, Cross);
  lcd.create_char(2, Wifi);
  
  lcd.clear_screen();
  
  lcd.text_center("PET FEEDER v1", 0);
  lcd.text_center("BY: JAMES", 1);

  delay(3000);

  lcd.clear_screen();
  
  lcd.text_center("STARTING MODULES", 0);
  lcd.text_center("SD Card", 1);
  
  delay(2000);

  lcd.clear_screen();
  if (!SD.begin(5)) {
    Serial.println("Card Mount Failed");

    lcd.text_center("STARTING MODULES", 0);
    lcd.write_custom_text_center(byte(1), "Mount Failed", 0, 1);
  }
  else {
    lcd.text_center("STARTING MODULES", 0);
    lcd.write_custom_text_center(byte(0), "SD Card", 0, 1);
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");

    lcd.text_center("STARTING MODULES", 0);
    lcd.write_custom_text_center(byte(1), "No SD Card", 0, 1);
  }

  delay(2000);

  //EEPROM.write(3, 0);
  EEPROM.write(4, 0);
  EEPROM.write(5, 0);
  EEPROM.commit();
  if (EEPROM.read(3) == 1) {
    lcd.clear_screen();
    if (EEPROM.read(255) == 1) {

      lcd.text_center("--Access Point--", 0);
      lcd.text_center("RazorScythe", 1);

      wifiManager.setTimeout(180); // 3 mins
      if (!wifiManager.startConfigPortal("RazorScythe")) {
        connection_failed();
      }
      else {
        wifi_connected = true;

        lcd.clear_screen();

        lcd.text_center("STARTING MODULES", 0);
        lcd.text_center("WiFi Connected", 1);

        EEPROM.write(255, 0);
        EEPROM.commit();
      }
    }
    else {
      wifiManager.setTimeout(20);

      lcd.text_center("STARTING MODULES", 0);
      lcd.write_custom_text_center(byte(2), " Connecting", 0, 1);
  
      if (!wifiManager.autoConnect("RazorScythe")) {
        connection_failed();
      }
      else {
        wifi_connected = true;

        lcd.clear_screen();

        lcd.text_center("STARTING MODULES", 0);
        lcd.text_center("WiFi Connected", 1);
      }
    }
    delay(5000);
  }

  ssid_       = "SSID: " + String(WiFi.SSID());
  ip_         = "IP: " + WiFi.localIP().toString();

  lcd.clear_screen();
  //  text_center(ssid_.c_str(), 0);
  //  text_center(ip_.c_str(), 1);

  //  File file;
  //  file = SD.open("/Infrared/NEC/EDC.txt");
  //  if(!file){
  //    Serial.println("Failed to open file for reading");
  //    return;
  //  }
  //
  //  Serial.println("Read from file: ");
  //  String text = "";
  //  while(file.available()){
  //    char ch = file.read();
  //    text += ch;
  //    if(ch == '\n')
  //    {
  //      Serial.print(text);
  //      //Serial.println(strtol(text.c_str(), NULL, 16));
  //      text = "";
  //    }
  //  }
  //  file.close();
  
//  EEPROM.write(0, 2);
//  EEPROM.commit();
}

/*
    BTN 1 = RIGHT
    BTN 2 = LEFT
    BTN 3 = DOWN
    BTN 4 = UP
*/

int menuDelay = 200;

//OPTIONS
const int option_len = 2;
String option_list[option_len] = {"DISABLED", "ENABLED"};
const int option_len2 = 3;
String option_list2[option_len2] = {"LEFT", "RIGHT", "BOTH"};

// WIFI FAILED SETUP VARIABLE
int wifi_failed_selector = 1;
bool wifi_failed_exe_once;
bool offline_mode = false;

// QUICK DISPENSE VARIABLE
bool quick_dispence_exe_once;
int quick_dispense_selector = 1;
const int qd_list_len = 3;
String qd_list[qd_list_len] = {"LEFT", "RIGHT", "MIXED"};
bool quick_dispense_triggered;

// QUICK DISPENSE DELAY VARIABLE
bool portion_exe_once;
int portion_selector = 1;
const int portion_list_len = 3;
String portion_list[portion_list_len] = {"SMALL", "MEDIUM", "LARGE"};

// SCHEDULER VARIABLE
bool scheduler_exe_once;
bool schedule_later;
int scheduler_selector = 1;
const int scheduler_list_len = 3;
String scheduler_list[scheduler_list_len] = {"SET NOW", "LATER", "DISABLE"};

// SCHEDULER NOW VARIABLE
bool schedule_now;
bool schedule_now_exe_once;
int schedule_now_selector = 1;
const int schedule_now_list_len = 8;
String schedule_now_list[schedule_now_list_len] = {"MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY", "SUNDAY", "WEEK"};

//SET SCHEDULE VARIABLE
int set_schedule_ctr = 0;
bool set_schedule;
bool set_schedule_exe_once;
int set_schedule_selector = 1;
int type_of_day_selector = 1;
const int type_of_day_len = 3;
String type_of_day[type_of_day_len] = {"MORNING", "NOON", "EVENING"};
int default_time[type_of_day_len] = {6, 12, 19};

int time_selector = 1;
String reference_time = "AM";
int hours = 0;
int minutes = 0;


// MENU VARIABLE
bool menu_select, wifi_config, qd_config;
int menu_ctr = 0;

bool menu_select_exe_once;
int menu_selector = 1;
const int menu_list_len = 5;
String menu_list[menu_list_len] = {"SETTINGS", "WIFI CONFIG", "QD CONFIG", "MOBILE CONFIG" "RESTART"};


// WIFI CONFIG VARIABLE 
bool wifi_config_exe_once, wifi_config_select, wifi_config_select_once;
int wifi_config_selector = 1;
int wifi_config_selection = 1;
const int wifi_config_len = 2;
String wifi_config_list[wifi_config_len] = {"WIFI", "CAPTIVE PORTAL"};

//QD CONFIG VARIABLE
bool qd_config_exe_once, qd_config_select, qd_config_select_once, portion_selected, clean_bottle_selected;
int qd_config_selector = 1;
int qd_config_selection = 1;
const int qd_config_len = 2;
String qd_config_list[qd_config_len] = {"SET PORTION", "CLEAN BOTTLE"};

void loop() {
  /*
   * WIFI FAILED STATE TRIGGERS
   */
  if(wifi_failed_connection && !offline_mode){
      if(!wifi_failed_exe_once){
        lcd.text_center("OFFLINE MODE?",0);
        lcd.set_cursor(0, 0);
        lcd.menu_selector("YES", 1);
        wifi_failed_exe_once = true;
      }
      
      if(digitalRead(BTN_1) == LOW){
        if(wifi_failed_selector == 1){
            wifi_failed_selector = 0;
            lcd.menu_selector("NO", 1);
        }
        else{
            wifi_failed_selector = 1;
            lcd.menu_selector("YES", 1);
        }
        delay(menuDelay);     
      }
      else if(digitalRead(BTN_2) == LOW){
        if(wifi_failed_selector == 1){ 
            wifi_failed_selector = 0;
            lcd.menu_selector("NO", 1);
        }
        else{
            wifi_failed_selector = 1;
            lcd.menu_selector("YES", 1);
        }
        delay(menuDelay);     
      }
      
      if(digitalRead(Quick_BTN) == LOW){
        lcd.clear_screen();
        if(wifi_failed_selector == 0){
          lcd.text_center("DEVICE WILL", 0);
          lcd.text_center("RESTART", 1);  
          delay(3000);  
          lcd.clear_screen();
          ESP.restart();
        }
        else{
          wifi_failed_connection = false;
          offline_mode = true;
        }
        delay(menuDelay);     
      }
   
      return;
  }


  /*
   *  SETUP FOR QUICK SELECTION, AFTER CONFIGURED YOU CAN CHANGE IT ANYTIME ON THE MENU
   */ 
  if(!EEPROM.read(2)){
    if(!quick_dispence_exe_once){
      lcd.text_center("QUICK DISPENSE", 0);
      lcd.menu_selector(qd_list[0], 1);
      quick_dispence_exe_once = true;
    }
    
    if(digitalRead(BTN_1) == LOW){
      MENU_BTN_RIGHT(qd_list_len, quick_dispense_selector, 3, qd_list, "main");
    }
    else if(digitalRead(BTN_2) == LOW){
      MENU_BTN_LEFT(qd_list_len, quick_dispense_selector, 3, qd_list, "main");
    }

    if(digitalRead(Quick_BTN) == LOW){
      String container = "";
      
      lcd.clear_screen();
      
      if(quick_dispense_selector == 3){container = "Mixed";}
      else if(quick_dispense_selector == 2){container = "Right Container";}
      else if(quick_dispense_selector == 1){container = "Left Container";}
      
      EEPROM.write(2, quick_dispense_selector);
      EEPROM.commit();
   
      lcd.text_center("QD is set to", 0);
      lcd.text_center(container.c_str(), 1);

      quick_dispence_exe_once = false;
      
      delay(2000);
      lcd.clear_screen();
    }
    return;
  }

  if(EEPROM.read(2) && !EEPROM.read(6)){
    if(!portion_exe_once){
      lcd.text_center("FOOD PORTION", 0);
      lcd.menu_selector(portion_list[0], 1);
      portion_exe_once = true;
    }
    
    if(digitalRead(BTN_1) == LOW){
      MENU_BTN_RIGHT(portion_list_len, portion_selector, 4, portion_list, "main");
    }
    else if(digitalRead(BTN_2) == LOW){
      MENU_BTN_LEFT(portion_list_len, portion_selector, 4, portion_list, "main");
    }

    if(digitalRead(Quick_BTN) == LOW){
      int portion_delay;
      
      lcd.clear_screen();
      
      if(portion_selector == 3){portion_delay = 3;}
      else if(portion_selector == 2){portion_delay = 2;}
      else if(portion_selector == 1){portion_delay = 1;}
      
      EEPROM.write(6, portion_delay);
      EEPROM.commit();
   
      lcd.text_center("QUICK DISPENSE", 0);
      lcd.text_center("SETUP DONE!", 1);

      quick_dispence_exe_once = false;
      portion_selected = false;
      delay(2000);
      lcd.clear_screen();
    }
    return;
  }
  /*
   *  SETUP FOR SCHEDULE TO DISPENSE FOOD
   */
  if((!EEPROM.read(4) && !schedule_later) && !EEPROM.read(5) && !schedule_now){
      if(!scheduler_exe_once){
        lcd.text_center("SET SCHEDULER",0);
        lcd.set_cursor(0, 0);
        lcd.menu_selector(scheduler_list[0], 1);
        scheduler_exe_once = true;
      }

      if(digitalRead(BTN_1) == LOW){
        MENU_BTN_RIGHT(scheduler_list_len, scheduler_selector, 2, scheduler_list, "main");
      }
      else if(digitalRead(BTN_2) == LOW){
        MENU_BTN_LEFT(scheduler_list_len, scheduler_selector, 2, scheduler_list, "main");
      }
      
      if(digitalRead(Quick_BTN) == LOW){
        lcd.clear_screen(); 
          
        if(scheduler_selector == 1){schedule_now = true;}
        else if(scheduler_selector == 2){
          schedule_later = true;
        }
        else if(scheduler_selector == 3){
          lcd.text_center("SCHEDULER HAS", 0);
          lcd.text_center("SET TO DISABLE", 1);
          EEPROM.write(5, 1);
          EEPROM.commit();
          delay(2000);
        }
        delay(menuDelay);
      }
   
      return;
  }

  if(schedule_now && !set_schedule){
    if(!schedule_now_exe_once){
      lcd.text_center("SCHEDULE DAYS", 0);
      lcd.menu_selector(schedule_now_list[schedule_now_selector - 1], 1);
      schedule_now_exe_once = true;
      delay(menuDelay);
    }

    if(digitalRead(BTN_3) == LOW && digitalRead(BTN_4) == LOW){
      lcd.clear_screen();
      lcd.text_center("SCHEDULER IS", 0);
      lcd.text_center("SET", 1);
      EEPROM.write(4, 1);
      EEPROM.commit();

      schedule_now = false;
      delay(3000);
    }
    else if(digitalRead(BTN_1) == LOW){
      MENU_BTN_RIGHT(schedule_now_list_len, schedule_now_selector, 5, schedule_now_list, "main");
    }
    else if(digitalRead(BTN_2) == LOW){
      MENU_BTN_LEFT(schedule_now_list_len, schedule_now_selector, 5, schedule_now_list, "main");
    }

    if(digitalRead(Quick_BTN) == LOW){
      lcd.clear_screen();
      
      set_schedule = true;
      delay(menuDelay);
    }
    return;
  }

  if(set_schedule){
    if(!set_schedule_exe_once){
      String time_set = "";
      int first_time_index = 11 + ((schedule_now_selector - 1) * 9);
      String display_minute = "";
   
      if(EEPROM.read(first_time_index)) hours = EEPROM.read(first_time_index); 
      else hours = default_time[0];
  
      if(hours == 12) hours = 12;
      else if(hours > 12) hours = hours - 12;
      else hours = hours;
  
      minutes = EEPROM.read(first_time_index + 1); 
      if(minutes <= 9) display_minute = "0"+String(minutes);
      else display_minute = String(minutes);
      
      if(EEPROM.read(first_time_index + 2) == 1) reference_time = "AM";
      else if(EEPROM.read(first_time_index + 2) == 2) reference_time = "PM";

      time_set = String(hours)+":"+display_minute+" "+reference_time;
      
      lcd.text_center(type_of_day[0].c_str(), 0);
      lcd.menu_selector(time_set, 1);
      set_schedule_exe_once = true;
    }

    if(digitalRead(BTN_3) == LOW && digitalRead(BTN_4) == LOW){
      set_schedule_ctr ++;
      if(set_schedule_ctr >= 500){
         set_schedule = false;
         set_schedule_exe_once = false;
         hours = 0;
         minutes = 0;
         reference_time = "AM";
         return;
      }
    }
    else if(digitalRead(Quick_BTN) == LOW){
      MENU_BTN_NEXT_TIME(type_of_day_len, type_of_day_selector, 8, type_of_day, default_time, 11 + ((schedule_now_selector - 1) * 9));
    }
    else if(digitalRead(BTN_1) == LOW){
      MENU_BTN_RIGHT_TIME(2, time_selector, 7);
    }
    else if(digitalRead(BTN_2) == LOW){
      MENU_BTN_LEFT_TIME(2, time_selector, 7);
    }
    else if(digitalRead(BTN_3) == LOW){
      MENU_BTN_DOWN_TIME(hours, minutes, reference_time, time_selector);
    }
    else if(digitalRead(BTN_4) == LOW){
      MENU_BTN_UP_TIME(hours, minutes, reference_time, time_selector);
    }
    
    set_schedule_ctr = 0;
    return;
  }
  
  /*
   *  GOTO MENU STATE
   */

   if(digitalRead(BTN_3) == LOW && !menu_select){
      menu_ctr ++;
      Serial.println(menu_ctr);
      if(menu_ctr >= 1500) menu_select = true;
      
      return;
   }
   else menu_ctr = 0;

   /*
    *   MENU SELECTOR
    */
  
   if(menu_select && !(wifi_config || qd_config)){
      if(!menu_select_exe_once){
        lcd.text_center("SELECT MENU",0);
        lcd.set_cursor(0, 0);
        lcd.menu_selector(menu_list[menu_selector - 1], 1);
        menu_select_exe_once = true;
      }

      if(digitalRead(BTN_4) == LOW){
        menu_select = false;
        menu_select_exe_once = false;
      }
      
      if(digitalRead(BTN_1) == LOW){
        MENU_BTN_RIGHT(menu_list_len, menu_selector, 1, menu_list, "main");
      }
      else if(digitalRead(BTN_2) == LOW){
        MENU_BTN_LEFT(menu_list_len, menu_selector, 1, menu_list, "main");
      }

      if(digitalRead(Quick_BTN) == LOW){
        if(menu_selector == 2){
            wifi_config = true;
        }
        else if(menu_selector == 3){
            qd_config = true;
        }
        else if(menu_selector == menu_list_len){
            lcd.clear_screen(); 
            lcd.text_center("DEVICE WILL", 0);
            lcd.text_center("RESTART", 1);
            delay(5000);
            lcd.clear_screen();
            ESP.restart();
        }
        delay(menuDelay);
      }
      return;
   }
  
   /*
    *  WIFI CONFIG
    */
   if(wifi_config && !wifi_config_select){
      if(!wifi_config_exe_once){
        lcd.text_center("WIFI CONFIG",0);
        lcd.set_cursor(0, 0);
        lcd.menu_selector(wifi_config_list[wifi_config_selector - 1], 1);
        wifi_config_exe_once = true;
      }

      if(digitalRead(BTN_4) == LOW){
        wifi_config = false;
        wifi_config_exe_once = false;
        menu_select_exe_once = false;
      }
      
      if(digitalRead(BTN_1) == LOW){
        MENU_BTN_RIGHT(wifi_config_len, wifi_config_selector, 9, wifi_config_list, "main");
      }
      else if(digitalRead(BTN_2) == LOW){
        MENU_BTN_LEFT(wifi_config_len, wifi_config_selector, 9, wifi_config_list, "main");
      }
      else if(digitalRead(BTN_3) == LOW){
        wifi_config = false;
        menu_select_exe_once = false;
      }

      if(digitalRead(Quick_BTN) == LOW){
        wifi_config_select = true;
        delay(menuDelay);
      }
      return;
   }

   /*
    * WIFI CONFIG SELECTION
    */
   if(wifi_config_select){
     if(!wifi_config_select_once){
        lcd.text_center(wifi_config_list[wifi_config_selector - 1].c_str(),0);
        lcd.set_cursor(0, 0);
        
        if(wifi_config_selector == 1){
          if(EEPROM.read(3) == 1) {
            lcd.menu_selector(option_list[1], 1);
            wifi_config_selection = 2;
          }
          else {
            lcd.menu_selector(option_list[0], 1);
          }
        }
        else {
          lcd.menu_selector(option_list[0], 1);
        }
        wifi_config_select_once = true;
     }

     if(digitalRead(BTN_1) == LOW){
       MENU_BTN_RIGHT(option_len, wifi_config_selection, 10, option_list, "main");
     }
     else if(digitalRead(BTN_2) == LOW){
       MENU_BTN_LEFT(option_len, wifi_config_selection, 10, option_list, "main");
     }
     else if(digitalRead(BTN_3) == LOW){
       wifi_config_select_once = false;
       wifi_config_select = false;
       wifi_config_exe_once = false;
       delay(menuDelay);
     }

     if(digitalRead(Quick_BTN) == LOW){
       lcd.clear_screen();
       wifi_config_select = true;
       if(wifi_config_selector == 1){
          EEPROM.write(3, wifi_config_selection - 1);
       }
       else if(wifi_config_selector == 2){
          if(wifi_config_selection - 1 == 1) {
            EEPROM.write(255, 1);
            EEPROM.commit();
            WiFiManager wifiManager;
            lcd.text_center("RESETING WIFI", 0);
            lcd.text_center("PLEASE WAIT", 1);
            wifiManager.resetSettings();         
            delay(3000);
            lcd.clear_screen();
            lcd.text_center("RESETING WIFI", 0);
            lcd.text_center("COMPLETE", 1);
            delay(2000);
            lcd.text_center("DEVICE WILL", 0);
            lcd.text_center("RESTART", 1);
            WiFi.begin("0","0");       // adding this effectively seems to erase the previous stored SSID/PW
            WiFi.disconnect(true, true); 
            delay(3000);
            lcd.clear_screen();
            ESP.restart();
          }
       }
       EEPROM.commit();

       wifi_config_select_once = false;
       wifi_config_select = false;
       wifi_config_exe_once = false;
       delay(menuDelay);
     }
     return;
   }


   /*
    *  QD CONFIG
    */
   if(qd_config && !qd_config_select){
      if(!qd_config_exe_once){
        lcd.text_center("QD CONFIG",0);
        lcd.set_cursor(0, 0);
        lcd.menu_selector(qd_config_list[qd_config_selector - 1], 1);
        qd_config_exe_once = true;
      }

      if(digitalRead(BTN_4) == LOW){
        qd_config = false;
        qd_config_exe_once = false;
        menu_select_exe_once = false;
      }
      
      if(digitalRead(BTN_1) == LOW){
        MENU_BTN_RIGHT(qd_config_len, qd_config_selector, 11, qd_config_list, "main");
      }
      else if(digitalRead(BTN_2) == LOW){
        MENU_BTN_LEFT(qd_config_len, qd_config_selector, 11, qd_config_list, "main");
      }
      else if(digitalRead(BTN_3) == LOW){
        qd_config = false;
        portion_selected = false;
        menu_select_exe_once = false;
      }

      if(digitalRead(Quick_BTN) == LOW){
        qd_config_select = true;
        if(qd_config_selector == 1){
          EEPROM.write(6, 0);
          EEPROM.commit();
          portion_selected = true;
        }
        else if(qd_config_selector == 2){
          clean_bottle_selected = true;
        }
        delay(menuDelay);
      }
      return;
   }

   /*
    * QD CONFIG SELECTION
    */
   if(qd_config_select && !portion_selected){
     if(!qd_config_select_once){
        lcd.text_center(qd_config_list[qd_config_selector - 1].c_str(),0);
        lcd.set_cursor(0, 0);
        lcd.menu_selector(option_list2[0], 1);
        wifi_config_select_once = true;
     }

     if(digitalRead(BTN_1) == LOW){
       MENU_BTN_RIGHT(option_len2, qd_config_selection, 11, option_list2, "main");
     }
     else if(digitalRead(BTN_2) == LOW){
       MENU_BTN_LEFT(option_len2, qd_config_selection, 11, option_list2, "main");
     }
     else if(digitalRead(BTN_3) == LOW){
       portion_selected = false;
       qd_config_select_once = false;
       qd_config_select = false;
       qd_config_exe_once = false;
       delay(menuDelay);
     }

     if(digitalRead(Quick_BTN) == LOW){
       lcd.clear_screen();
       wifi_config_select = true;
       if(clean_bottle_selected == true){
          lcd.text_center("OPENING BOTTLE", 0);
          lcd.text_center("3", 1);
          delay(1000);
          lcd.clear_screen();
          lcd.text_center("OPENING BOTTLE", 0);
          lcd.text_center("2", 1);
          delay(1000);
          lcd.clear_screen();
          lcd.text_center("OPENING BOTTLE", 0);
          lcd.text_center("1", 1);
          delay(1000);
          lcd.clear_screen();
          lcd.text_center("PLEASE", 0);
          lcd.text_center("TILT THE BOX", 1);
          delay(1000);
          
          if(qd_config_selection == 1){
             myservo2.write(0);
             delay(10000); //10secs
             myservo2.write(80);
          }
          else if(qd_config_selection == 2){
             myservo.write(150);
             delay(10000); //10secs
             myservo.write(0);
          }
          else if(qd_config_selection == 3){
             myservo2.write(0);
             delay(10000); //10secs
             myservo2.write(80);
             delay(menuDelay);
             myservo.write(150);
             delay(10000); //10secs
             myservo.write(0);
          }
          clean_bottle_selected = false;
          
          lcd.clear_screen();
          lcd.text_center("CLEANING", 0);
          lcd.text_center("BOTTLE DONE!", 1);
          delay(3000);
          
          //myservo.write(0); //150 open // 0 close //right
          //myservo2.write(80); //0 open // 80 close //left
       }
       
       qd_config_selection = 1;
       wifi_config_select_once = false;
       wifi_config_select = false;
       wifi_config_exe_once = false;
       delay(menuDelay);
     }
     return;
   }
   
  /*
   *  QUICK DISPENSE AUTO
   */
   if(EEPROM.read(2) && !menu_select){
      if(!quick_dispence_exe_once){
        lcd.clear_screen();
        lcd.text_center("QUICK DISPENSE", 0);
        
        if(quick_dispense_triggered) lcd.menu_selector(qd_list[quick_dispense_selector - 1], 1);
        else lcd.menu_selector(qd_list[EEPROM.read(2) - 1].c_str(), 1);
        quick_dispence_exe_once = true;
      }
      
      if(digitalRead(BTN_1) == LOW){
        MENU_BTN_RIGHT(qd_list_len, quick_dispense_selector, 3, qd_list, "main");
      }
      else if(digitalRead(BTN_2) == LOW){
        MENU_BTN_LEFT(qd_list_len, quick_dispense_selector, 3, qd_list, "main");
      }
  
      if(digitalRead(Quick_BTN) == LOW){
         lcd.clear_screen();
        
         lcd.text_center("DISPENSING", 0);
         lcd.text_center("PLEASE WAIT.", 1);
         if(quick_dispense_selector == 1){
           dispense_left(myservo2, EEPROM.read(6) * 100);
         }
         else if(quick_dispense_selector == 2){
           dispense_right(myservo, EEPROM.read(6) * 100);
         }
         else if(quick_dispense_selector == 3){
           mixedDispense(myservo, myservo2, EEPROM.read(6) * 100);
         }
         delay(3000); 
         lcd.clear_screen();
         lcd.text_center("DONE!", 0);
         quick_dispence_exe_once = false;
         quick_dispense_triggered = true;
         delay(2000);
      }
   }
  delay(200);
}

void dispense_left(Servo &servo, int portion) {
  servo.write(30);
  delay(portion);
  servo.write(80);
}

void dispense_right(Servo &servo, int portion) {
  servo.write(60);
  delay(portion);
  servo.write(0);
}

void mixedDispense(Servo &servo1, Servo &servo2, int portion) {
  int partial = float(portion * 1.2 / 2);
  servo1.write(60);
  delay(partial);
  servo1.write(0);
  delay(partial);
  servo2.write(30);
  delay(partial);
  servo2.write(80);
  delay(partial);
  servo1.write(60);
  delay(partial);
  servo1.write(0);
  delay(partial);
  servo2.write(30);
  delay(partial);
  servo2.write(80);
}

void playScale(uint8_t pin, uint8_t channel){
  ledcAttachPin(pin, channel);
  //    ledcWriteTone(channel, 600);
  //    delay(500);
  ledcWriteTone(channel, 700);
  delay(100);
  ledcWriteTone(channel, 800);
  delay(100);
  ledcWriteTone(channel, 900);
  delay(100);
  ledcWriteTone(channel, 900);
  delay(100);
  ledcWriteTone(channel, 800);
  delay(100);
  ledcWriteTone(channel, 800);
  delay(100);
  ledcDetachPin(pin);
}

void EEPROM_Reset(){    
    for (int i = 0; i < 512; i++) {
     EEPROM.write(i,0);
     delay(10);
    }
    EEPROM.commit();
}

void EEPROM_Read(){
  for (int i = 0; i < 10; i++) {
     Serial.print("EEPROM ");
     Serial.print(i);
     Serial.print(": ");
     Serial.println(EEPROM.read(i));
     delay(10);
  }
}

void MENU_BTN_LEFT(int end_ctr, int selector, int index, String list[], String type_){
    if(type_ == "main"){
        if(selector == 1){
          selector = end_ctr;
        }
        else{
          selector --;
        }
        lcd.menu_selector(list[selector - 1], 1);
        SET_VARIABLE(index, selector);
    }
    delay(menuDelay);
}

void MENU_BTN_RIGHT(int end_ctr, int selector, int index, String list[], String type_){
    if(type_ == "main"){
        if(selector <= end_ctr){
            if(selector == end_ctr){
              selector = 1;
            }
            else{
              selector ++;
            }
        }
        lcd.menu_selector(list[selector - 1], 1);
        SET_VARIABLE(index, selector);
    }
    delay(menuDelay);
}

void MENU_BTN_NEXT_TIME(int end_ctr, int selector, int index, String list[], int time_list[], int EEPROM_start){
    String rt = "";
    String time_set = "";
    int current_time;
    String display_minute = ""; 
    
    int prev_eeprom_index = (EEPROM_start + ((selector - 1) * 3));  
    int curr_eeprom_index = (EEPROM_start + (selector * 3));  
    int eeprom_rt = reference_time == "AM" ? 1 : 2;
    int curr_selector = selector;
//    Serial.println("=====EEPROM=====");
//    Serial.println(selector);
//    Serial.println(eeprom_index);
//    Serial.println((EEPROM_start + ((selector - 1) * 3)));
//    Serial.println("===============");
//    Serial.println("=====START=====");
//    Serial.println(hours);
//    Serial.println(time_list[selector - 1]);
//    Serial.println(hours != time_list[selector - 1] || reference_time == "PM");
//    Serial.println("===============");
//    Serial.println("=====BOOLEAN=====");
//    Serial.println(hours == 0);
//    Serial.println(hours != time_list[selector - 1]);
//    Serial.println("===============");
    if(hours == 0) EEPROM.write(prev_eeprom_index, time_list[selector - 1]);
    else if(hours != 12 && reference_time == "PM") EEPROM.write(prev_eeprom_index, hours + 12);
    else EEPROM.write(prev_eeprom_index, hours);

    if(minutes == 0) EEPROM.write(prev_eeprom_index + 1, 0);
    else EEPROM.write(prev_eeprom_index + 1, minutes);

    EEPROM.write(prev_eeprom_index + 2, eeprom_rt);
    EEPROM.commit();
    
    if(selector <= end_ctr){
        if(selector == end_ctr){
           selector = 1;
           set_schedule = false;
           set_schedule_exe_once = false;
           schedule_now_exe_once = false;
           hours = 0;
           minutes = 0;
           reference_time = "AM";
           SET_VARIABLE(index, selector);
           return;
        }
        else{
          selector ++;
        }
    }

    if(time_list[selector - 1] == 12){
      rt = "PM";
      current_time = 12;
    }
    else if(time_list[selector - 1] > 12){
        current_time = time_list[selector - 1] - 12;
        rt = "PM";
    }
    else {
        current_time = time_list[selector - 1];
        rt = "AM";
    }

    if(selector == 1) {
      int set_eeprom_state = EEPROM_start;  
      hours = EEPROM.read(set_eeprom_state); 
    }
    else if(EEPROM.read(curr_eeprom_index)) hours = EEPROM.read(curr_eeprom_index);
    else hours = current_time;

    if(hours == 12) hours = 12;
    else if(hours > 12) hours = hours - 12;
    else hours = hours;

    if(selector == 1){
      int set_eeprom_state = EEPROM_start;  
      minutes = EEPROM.read(set_eeprom_state + 1); 
    }
    else if(EEPROM.read(curr_eeprom_index + 1)) minutes = EEPROM.read(curr_eeprom_index + 1);
    else minutes = 0;

    if(minutes <= 9) display_minute = "0"+String(minutes);
    else display_minute = String(minutes);
      
    if(selector == 1){
      int set_eeprom_state = EEPROM_start;  
      if(EEPROM.read(set_eeprom_state + 2) == 1) reference_time = "AM";
      else if(EEPROM.read(set_eeprom_state + 2) == 2) reference_time = "PM";
    }
    else if(EEPROM.read(curr_eeprom_index + 2) == 1) reference_time = "AM";
    else if(EEPROM.read(curr_eeprom_index + 2) == 2) reference_time = "PM";
    
    time_set = String(hours)+":"+display_minute+" "+rt;
    lcd.menu_selector(list[selector - 1], 0);
    lcd.menu_selector(time_set, 1);
    
    SET_VARIABLE(index, selector);

//    if(hours != time_list[selector - 1]) EEPROM.write(eeprom_index, time_list[selector - 1]);
//    else EEPROM.write(eeprom_index, hours);
//
//    if(minutes == 0) EEPROM.write(eeprom_index + 1, 0);
//    else EEPROM.write(eeprom_index + 1, minutes);
//
//    EEPROM.write(eeprom_index + 2, eeprom_rt);
    Serial.println("=====INDEX=====");
    Serial.print("PREV: ");
    Serial.println(prev_eeprom_index);
    Serial.print("CURR: ");
    Serial.println(curr_eeprom_index);
    Serial.println("===============");
    Serial.println("=====VALUE=====");
    Serial.print(prev_eeprom_index);
    Serial.print(": ");
    Serial.println(EEPROM.read(prev_eeprom_index));
    Serial.print(prev_eeprom_index+1);
    Serial.print(": ");
    Serial.println(EEPROM.read(prev_eeprom_index+1));
    Serial.print(prev_eeprom_index+2);
    Serial.print(": ");
    Serial.println(EEPROM.read(prev_eeprom_index+2));
    Serial.println("===============");
    delay(menuDelay);
}

void MENU_BTN_LEFT_TIME(int end_ctr, int selector, int index){
    if(selector == 1){
      selector = end_ctr;
    }
    else{
      selector --;
    }
    
    SET_VARIABLE(index, selector);
    
    delay(menuDelay);
}

void MENU_BTN_RIGHT_TIME(int end_ctr, int selector, int index){
    if(selector <= end_ctr){
        if(selector == end_ctr){
          selector = 1;
        }
        else{
          selector ++;
        }
    }
    
    SET_VARIABLE(index, selector);
   
    delay(menuDelay);
}

void MENU_BTN_UP_TIME(int h, int m, String rt, int selector){
    String time_set = "";
    String min_val = "";
    if(selector == 1){ //HOURS
        h ++;
        
        if(h == 12){
            if(rt == "AM") rt = "PM";
            else rt = "AM";
        }
        else if(h > 12){
            h = 1;
        }
    }
    else if(selector == 2){ //MINUTES
        m ++;
        
        if(m >= 60){
          h ++;
          m = 0;
          if(h == 12){
              if(rt == "AM") rt = "PM";
              else rt = "AM";
          }
          else if(h > 12){
              h = 1;
          }
        }
    }
    
    if(m == 0) min_val = "00";
    else if(m <= 9) min_val = "0"+String(m);
    else min_val = String(m);
    
    time_set = String(h) + ":" + min_val + " " + rt;
    reference_time = rt;
    hours = h;
    minutes = m;
    
    lcd.menu_selector(time_set, 1);
    delay(menuDelay);
}

void MENU_BTN_DOWN_TIME(int h, int m, String rt, int selector){
    String time_set = "";
    String min_val = "";
    if(selector == 1){ //HOURS
        h --;
        if(h <= 1){
            h = 1;
            if(rt == "AM") rt = "PM";
            else rt = "AM";
        }
    }
    else if(selector == 2){ //MINUTES
        m --;
        if(m <= 0){
          m = 59;
        }
    }

    if(m == 0) min_val = "00";
    else if(m <= 9) min_val = "0"+String(m);
    else min_val = String(m);
        
    time_set = String(h) + ":" + min_val + " " + rt;
    reference_time = rt;
    hours = h;
    minutes = m;
    
    lcd.menu_selector(time_set, 1);
    delay(menuDelay);
}

void SET_VARIABLE(int selector, int val){
    switch(selector){
        case 1: menu_selector = val;
          break;
        case 2: scheduler_selector = val;
          break;
        case 3: quick_dispense_selector = val;
          break;
        case 4: portion_selector = val;
          break;
        case 5: schedule_now_selector = val;
          break;
        case 6: set_schedule_selector = val;
          break;
        case 7: time_selector = val;
          break;
        case 8: type_of_day_selector = val;
          break;
        case 9: wifi_config_selector = val;
          break;
        case 10: wifi_config_selection = val;
          break;
        case 11: qd_config_selector = val;
          break;
        case 12: qd_config_selection = val;
          break;
    }
}
