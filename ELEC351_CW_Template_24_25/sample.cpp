#include "sample.hpp"

sampleData getsample(){
    sampleData data;
    data.temp = env.getTemperature();
    data.pressure = env.getPressure();
    data.light_level = ldr.read();
    return data;
}

void printsample(float temp, float pressure, float light_level){
    // Print the samples to the terminal
    printf("\n----- Sample %d -----\nTemperature:\t%3.1fC\nPressure:\t%4.1fmbar\nLight Level:\t%1.2f\n", sample_num,temp,pressure,light_level);

    sample_num++;

    // Write the current light level as a float to the 7-segment display
    latchedLEDs.write_seven_seg(light_level);
}

void thresholdsample(float temp, float pressure, float light_level){
    // If the current light level is above a threshold take action
    bool lightT = 0; bool tempT=0; bool presT=0;

    if(light_level>0.55f){
        lightT = 1;
    }else if(light_level<0.45f){
        lightT = 0;
    }
    if(temp>26.1f){
        tempT = 1;
    }else if(temp<25.8f){
        tempT = 0;
    }
    if(pressure>1011){
        presT = 1;
    }else if(pressure<1009){
        presT = 0;
    }

    if(lightT || tempT || presT){
        for(int i=0;i<4;i++){
            buzz.playTone("C");                     // Play tone on buzzer
            latchedLEDs.write_strip(0xFF,RED);      // Turn on all LEDs on RGB bar
            latchedLEDs.write_strip(0xFF,GREEN);
            latchedLEDs.write_strip(0xFF,BLUE);
            ThisThread::sleep_for(200ms);

            buzz.rest();                            // Stop buzzer
            latchedLEDs.write_strip(0x0,RED);       // Turn off all LEDs on RGB bar
            latchedLEDs.write_strip(0x0,GREEN);
            latchedLEDs.write_strip(0x0,BLUE);
            ThisThread::sleep_for(200ms);
        }
    }
    else{
        latchedLEDs.write_strip(0x0,RED);           // Turn off all LEDs on RGB bar
        latchedLEDs.write_strip(0x0,GREEN);
        latchedLEDs.write_strip(0x0,BLUE);
    }
}

void sampleThread(){
        sampleData newdata = getsample();
        
        data = newdata;
        //Set flag to allow another thread to print and do threshold stuff
        //sampleflag = 1;
        dataLock.release();
}

void timerISR(){
    dataLock.release();
}

void sampleP(){
    while(1){
    //This should only run after flag has been set bu the sampling thread
    //if(sampleflag){
        dataLock.acquire();
        //print to terminal
        printsample(data.temp, data.pressure, data.light_level);
        //process the data
        thresholdsample(data.temp, data.pressure, data.light_level);
        //sampleflag = 0;
        // Print the time and date
        time_t time_now = time(NULL);   // Get a time_t timestamp from the RTC
        struct tm* tt;                  // Create empty tm struct
        tt = localtime(&time_now);      // Convert time_t to tm struct using localtime
        printf("%s\n",asctime(tt));     // Print in human readable format
    //}

    ThisThread::sleep_for(100ms);
    }
}