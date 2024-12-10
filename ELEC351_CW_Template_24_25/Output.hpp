/*
    Connor Dykes
    10724083
*/

#ifndef OUTPUT_HPP
#define OUTPUT_HPP

#include "sample.hpp"

//Output controls the LEDs and LCD
void stripLED(float temp, float pressure, float light_level, int mode);
void LCD();


#endif