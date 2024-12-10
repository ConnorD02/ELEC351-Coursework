/*
    Connor Dykes
    10724083
*/

//Holds all the functions required for taking samples, buffering samples, sending samples and processing the sample data

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
    int mode;   //for LED strip
    void getsample(){   //sampling function
        samplenum++;
        temp = env.getTemperature();
        pressure = env.getPressure();
        light_level = ldr.read();
        timestamp = time(NULL);   // Get a time_t timestamp from the RTC
    }
};
//Structure that holds all the necessary variables for sampling and controlling the outputs of the system that depend on the samples taken like the LED strip

extern struct tm t;
extern time_t t_of_day;


extern Ticker timer;

extern EventQueue queue;    //queues sample processing functions

extern Semaphore flush_semaphore;   //flushes data to SD card

// Buffer to hold multiple samples
extern std::vector<sampleData> dataBuffer;

extern Mail<sampleData, 10> mail_data;      //FIFO buffer

extern bool sampleOn;   //determines if sampling is enabled or not

void init(); //initialises variables
void getsample();
void printsample(int sample_num, float temp, float pressure, float light_level, time_t timestamp);
void thresholdsample(float light_level);
void sampleThread();
void sampleP();
void timerISR();
void adddataBuffer(uint32_t sample_num, float temp, float pressure, float light_level, time_t timestamp);
void writeBufferToSD();



//Ticker for sample ISR
extern Ticker timer;

#endif