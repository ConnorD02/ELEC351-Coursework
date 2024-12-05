#include "Output.hpp"

void stripLED(float temp, float pressure, float light_level, int mode){
    //temp
    //lower threshold for the LEDs around 15
    //upper around 30
    //Divide by 3 then by 8 for each colour then each LED respectively
    //normalise the value to max temp being max and set LED

    latchedLEDs.write_strip(0x00,RED);     //Turn off all LEDs on R bar
    latchedLEDs.write_strip(0x00,GREEN);     //Turn off all LEDs on G bar
    latchedLEDs.write_strip(0x00,BLUE);     //Turn off all LEDs on B bar

    int tempLED = 24 * (temp - 15)/15;
    if(mode == 0){
        if(tempLED < 8){
            latchedLEDs.write_strip((1 << tempLED)-1,RED);
            latchedLEDs.write_strip(0x00,GREEN);     //Turn off all LEDs on G bar
            latchedLEDs.write_strip(0x00,BLUE);     //Turn off all LEDs on B bar
        } else if(tempLED >= 8 && tempLED < 16){
            latchedLEDs.write_strip(0xFF,RED);      // Turn on all LEDs on R bar
            latchedLEDs.write_strip((1 << (tempLED - 5))-1,GREEN);
            latchedLEDs.write_strip(0x00,BLUE);     //Turn off all LEDs on B bar
        } else if(tempLED >= 16 && tempLED < 24){
            latchedLEDs.write_strip(0xFF,GREEN);    //Turn on all LEDs on G bar
            latchedLEDs.write_strip((1 << (tempLED - 15))-1,BLUE);
        }else{
            latchedLEDs.write_strip(0xFF,RED);      // Turn on all LEDs on R bar
            latchedLEDs.write_strip(0xFF,GREEN);    //Turn on all LEDs on G bar
            latchedLEDs.write_strip(0xFF,BLUE);     //Turn on all LEDs on B bar
        }
    }

    //pressure
    //lower = 800
    //upper = 1200
    int presLED = 24 * (pressure - 800)/400;
    if(mode == 1){
        if(presLED < 8){
            latchedLEDs.write_strip((1 << (presLED))-1,RED);
            latchedLEDs.write_strip(0x00,GREEN);     //Turn off all LEDs on G bar
            latchedLEDs.write_strip(0x00,BLUE);     //Turn off all LEDs on B bar
        } else if(presLED >= 8 && presLED < 16){
            latchedLEDs.write_strip(0xFF,RED);      // Turn on all LEDs on R bar
            latchedLEDs.write_strip((1 << (presLED - 7))-1,GREEN);
            latchedLEDs.write_strip(0x00,BLUE);     //Turn off all LEDs on B bar
        } else if(presLED >= 16 && presLED < 24){
            latchedLEDs.write_strip(0xFF,GREEN);    //Turn on all LEDs on G bar
            latchedLEDs.write_strip((1 << (presLED - 15))-1,BLUE);
        }else{
            latchedLEDs.write_strip(0xFF,RED);      // Turn on all LEDs on R bar
            latchedLEDs.write_strip(0xFF,GREEN);    //Turn on all LEDs on G bar
            latchedLEDs.write_strip(0xFF,BLUE);     //Turn on all LEDs on B bar
        }
    }

    //light
    //lower = 0
    //upper = 1
    int lightLED = light_level*24;
    if(mode == 2){
        if(lightLED < 8){
            latchedLEDs.write_strip((1 << (lightLED+1)) - 1,RED);
            latchedLEDs.write_strip(0x00,GREEN);     //Turn off all LEDs on G bar
            latchedLEDs.write_strip(0x00,BLUE);     //Turn off all LEDs on B bar
        } else if(lightLED >= 8 && lightLED < 16){
            latchedLEDs.write_strip(0xFF,RED);      // Turn on all LEDs on R bar
            latchedLEDs.write_strip((1 << (lightLED-7))-1,GREEN);
            latchedLEDs.write_strip(0x00,BLUE);     //Turn off all LEDs on B bar
        } else if(lightLED >= 16 && lightLED < 24){
            latchedLEDs.write_strip(0xFF,RED);      // Turn on all LEDs on R bar
            latchedLEDs.write_strip(0xFF,GREEN);    //Turn on all LEDs on G bar
            latchedLEDs.write_strip((1 << (lightLED-15))-1,BLUE);
        }else{
            latchedLEDs.write_strip(0xFF,RED);      // Turn on all LEDs on R bar
            latchedLEDs.write_strip(0xFF,GREEN);    //Turn on all LEDs on G bar
            latchedLEDs.write_strip(0xFF,BLUE);     //Turn on all LEDs on B bar
        }
    }

}

void LCD(){
    // Get the time and date
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
        
        strftime(lcd_line_buffer, sizeof(lcd_line_buffer), "     %H:%M:%S", tt);   // Create a string HH:mm
        disp.locate(1,0);                                                       // Set LCD cursor to (0,0)
        disp.printf("%s",lcd_line_buffer);                                      // Write text to LCD

        ThisThread::sleep_for(200ms);
}