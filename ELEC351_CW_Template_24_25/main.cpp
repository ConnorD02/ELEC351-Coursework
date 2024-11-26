/* 
 * Filename: main.cpp
 * Author: Andrew Norris
 * Institution: Plymouth University
 * Date: 21/10/24
 * Description: Starter Code for ELEC351 Coursework 24/25
 * 
 * Notes:
 *
 * For documentation regarding the module support board library, please see the Readme.md on github
 * https://github.com/PlymouthUniversityEEER/PlymouthUniversityModuleSupportBoard/blob/main/README.md
 *
 * You will need to set the module support board version that you have in MSB_Config.h
 * The default if V4. If you are using a V2 comment out "#define MSB_VER 4" and uncomment "#define MSB_VER 2"
 * 
 * 
 */

#include "sample.hpp"


//threads
Thread tsample;
Thread tq;
Thread t3;

int main()
{

    Thread tsample(osPriorityHigh);
    Thread t2(osPriorityNormal);
    Thread t3(osPriorityNormal);
    //maybe set another thread to handle an event queue only consisting of printing 

    // Set output enable on the latched LEDs.
    latchedLEDs.enable(true);
    
    // Set the time on the RTC (You can use https://www.epochconverter.com/ for testing)
    uint64_t now = 1729519318;
    set_time(now);

    // Write some text to the SD card
    if(sd.card_inserted()){ // Check to see if the card is present (polls PF_4)
        int err = sd.write_file("test.txt", "Plymouth University - ELEC351 Coursework 24-25\n");    // Attempt to write text to file
        if(err == 0){   // If is successful, read the content of the file back
            printf("Successfully written to SD card\n");
            printf("---------------------------\nFile contents:\n");
            sd.print_file("test.txt",false);
            printf("---------------------------\n");
        }
        else{
            printf("Error Writing to SD card\n");
        }
    }
    else{
        printf("No SD Card Detected\n");
    }


    // Print the time and date
    time_t time_now = time(NULL);   // Get a time_t timestamp from the RTC
    struct tm* tt;                  // Create empty tm struct
    tt = localtime(&time_now);      // Convert time_t to tm struct using localtime
    printf("%s\n",asctime(tt));     // Print in human readable format

    tsample.start(sampleThread);    //Sample on the highest priority thread
    timer.attach(&timerISR, 10s);
    tq.start(callback(&queue, &EventQueue::dispatch_forever));
    //Start Thread tq onto the event queue

    //I don't think this code above is working in the way I think it is
    //A Ticker will count for 10s and then trigger the timerISR

    while (true) {
        
        
/*
        // Print the time and date
        time_t time_now = time(NULL);   // Get a time_t timestamp from the RTC
        struct tm* tt;                  // Create empty tm struct
        tt = localtime(&time_now);      // Convert time_t to tm struct using localtime
        //printf("%s\n",asctime(tt));     // Print in human readable format

        // Write the time and date on the LCD
        disp.cls();                     // Clear the LCD                 
        char lcd_line_buffer[17];           
        
        strftime(lcd_line_buffer, sizeof(lcd_line_buffer), "%a %d-%b-%Y", tt);  // Create a string DDD dd-MM-YYYY
        disp.locate(0,0);                                                       // Set LCD cursor to (0,0)
        disp.printf("%s",lcd_line_buffer);                                      // Write text to LCD
        
        strftime(lcd_line_buffer, sizeof(lcd_line_buffer), "     %H:%M", tt);   // Create a string HH:mm
        disp.locate(1,0);                                                       // Set LCD cursor to (0,0)
        disp.printf("%s",lcd_line_buffer);                                      // Write text to LCD
*/
        ThisThread::sleep_for(Kernel::wait_for_u32_forever);
    }
}


