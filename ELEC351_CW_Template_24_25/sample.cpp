#include "sample.h"

int sample_num = 0;

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

void thresholdsample(float light_level){
    // If the current light level is above a threshold take action
    if(light_level>0.5f){
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
    while(1){
        sampleData data = getsample();
        printsample(data.temp, data.pressure, data.light_level);

        thresholdsample(data.light_level);

        ThisThread::sleep_for(10s);
    }
}
