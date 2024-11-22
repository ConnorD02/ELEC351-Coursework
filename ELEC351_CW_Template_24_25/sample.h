#pragma once
#include <iostream>
#include "uop_msb.h"
#include "mbed.h"
#include <chrono>

struct sampleData{
    float temp;
    float pressure;
    float light_level;
};

extern int sample_num;

sampleData getsample();
void printsample(float temp, float pressure, float light_level);
void thresholdsample(float light_level);
void sampleThread();
