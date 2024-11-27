#ifndef SAMPLE_HPP
#define SAMPLE_HPP
#include <iostream>
#include "uop_msb.h"
#include "mbed.h"
#include <chrono>
#include <vector>

struct sampleData{
    uint32_t samplenum;
    float temp;
    float pressure;
    float light_level;
    void getsample(){
        temp = env.getTemperature();
        pressure = env.getPressure();
        light_level = ldr.read();
    }
};

extern Ticker timer;

extern EventQueue queue;

extern int sample_num;

// Buffer to hold multiple samples
extern std::vector<sampleData> dataBuffer;

extern Mail<sampleData, 128> mail_data;

void getsample();
void printsample(float temp, float pressure, float light_level);
void thresholdsample(float light_level);
void sampleThread();
void sampleP();
void timerISR();
void adddataBuffer(float temp, float pressure, float light_level);
void writeBufferToSD(sampleData datatosend);

//Ticker
extern Ticker timer;

#endif