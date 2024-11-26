#include "mbed.h"

DigitalOut redLED(PC_2);
DigitalOut greenLED(PC_6);
DigitalIn din (PC_13);

Ticker redTicker;
Ticker greenTicker;

Semaphore redSem;

Thread redThread(osPriorityHigh7);
Thread greenThread;


void redLEDThread(){
    while(1){
        redSem.acquire();
        redLED = !redLED;
    }
}

void greenLEDThread(){
    while(1){
        ThisThread::flags_wait_any(1);
       // ThisThread::flags_clear(1);
        greenLED = !greenLED;
    }
}

void red_ticker_isr(){
    if(!din){
        redSem.release();
    }
}


void green_ticker_isr(){
    greenThread.flags_set(1);
}


int main()
{
    redThread.start(redLEDThread);
    greenThread.start(greenLEDThread);


    redTicker.attach_us(&red_ticker_isr, 1000000);
    greenTicker.attach_us(&green_ticker_isr, 500000);

    while (true) {

    }
}

