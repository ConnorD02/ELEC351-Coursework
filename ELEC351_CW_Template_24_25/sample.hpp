#ifndef SAMPLE_HPP
#define SAMPLE_HPP
#include <iostream>
#include "uop_msb.h"
#include "mbed.h"
#include <chrono>
#include <vector>
#include <atomic>    // For std::atomic

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

extern Semaphore inputReadySemaphore;

extern int sample_num;

// Buffer to hold multiple samples
extern std::vector<sampleData> dataBuffer;

extern Mail<sampleData, 10> mail_data;

extern std::string userInput;

void getsample();
void printsample(float temp, float pressure, float light_level);
void thresholdsample(float light_level);
void sampleThread();
void sampleP();
void timerISR();
void adddataBuffer(uint32_t sample_num, float temp, float pressure, float light_level);
void writeBufferToSD(sampleData datatosend);
void terminalInput();
void processUserInput();


//Ticker
extern Ticker timer;

#endif