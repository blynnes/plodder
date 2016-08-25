/**===========================================================================*\
 * Name        : GPIOManip.h
 * Author      : Brett Lynnes
 * Copyright   : 2016 Palo Alto Innovation, all rights reserved
 * Description : Manipulates various GPIO pins
\**===========================================================================*/

#ifndef GPIO_MANIP_H
#define GPIO_MANIP_H

#include <string>
using namespace std;

class GPIOManip {
public:
    /**
     * Create an instance to manipulate the GPIO with "num"
     */
    GPIOManip(string num);
    int export_gpio();             // exports GPIO
    int unexport_gpio();           // unexport GPIO
    int setdir_gpio(string dir);   // Set GPIO Direction
    int setedge_gpio(string edge); // Set GPIO Direction
    int setval_gpio(string val);   // Set GPIO Value (putput pins)
    int getval_gpio(string& val);  // Get GPIO Value (input/ output pins)
    string get_gpionum();          // return the GPIO number associated with the instance of an object
private:
    string gpionum; // GPIO number associated with the instance of an object
};

#endif
