#ifndef INIT_SETUP_H
#define INIT_SETUP_H

#include "Arduino.h"

extern int delay_;

class SETUP_SELECTOR{
    public:
        SETUP_SELECTOR();
        int init_offline_selection(int BTN_1, int BTN_2, int Quick_BTN, int selector);
        int init_quick_dispense_setup(int BTN_1, int BTN_2, int Quick_BTN, int selector);
};

//extern setup_class init_setup;

#endif
