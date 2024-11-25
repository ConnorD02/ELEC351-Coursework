#ifndef SAMPLE_HPP
#define SAMPLE_HPP
#include <iostream>
#include "uop_msb.h"
#include "mbed.h"
#include <chrono>

struct sampleData{
    float temp;
    float pressure;
    float light_level;
    void getsample(){
        temp = env.getTemperature();
        pressure = env.getPressure();
        light_level = ldr.read();
    }
};

extern EventQueue queue;

extern int sample_num;

void getsample();
void printsample(float temp, float pressure, float light_level);
void thresholdsample(float light_level);
void sampleThread();
void sampleP();
void timerISR();

//Ticker
extern Ticker timer;

#endif