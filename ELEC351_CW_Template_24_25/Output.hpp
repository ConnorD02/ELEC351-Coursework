#ifndef OUTPUT_HPP
#define OUTPUT_HPP
#include <iostream>
#include <sstream>
#include "uop_msb.h"
#include "mbed.h"
#include <chrono>
#include <vector>
#include <atomic>    // For std::atomic
#include "sample.hpp"


void stripLED(float temp, float pressure, float light_level, int mode);
void LCD();


#endif