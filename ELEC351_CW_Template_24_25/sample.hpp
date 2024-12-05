#ifndef SAMPLE_HPP
#define SAMPLE_HPP
#include <iostream>
#include <sstream>
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
    time_t timestamp;
    int mode;
    void getsample(){
        samplenum++;
        temp = env.getTemperature();
        pressure = env.getPressure();
        light_level = ldr.read();
        timestamp = time(NULL);   // Get a time_t timestamp from the RTC
    }
};

extern struct tm t;
extern time_t t_of_day;


extern Ticker timer;

extern EventQueue queue;

extern Semaphore flush_semaphore;
extern Semaphore bufferSemaphore;

extern int sample_num;

// Buffer to hold multiple samples
extern std::vector<sampleData> dataBuffer;

extern Mail<sampleData, 10> mail_data;

extern std::string userInput;

extern std::vector<std::string> arguments;

extern bool sampleOn;

void init();
void getsample();
void printsample(int sample_num, float temp, float pressure, float light_level, time_t timestamp);
void thresholdsample(float light_level);
void sampleThread();
void sampleP();
void timerISR();
void adddataBuffer(uint32_t sample_num, float temp, float pressure, float light_level, time_t timestamp);
void writeBufferToSD();



//Ticker
extern Ticker timer;

#endif