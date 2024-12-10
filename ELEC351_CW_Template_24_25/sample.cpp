/*
    Connor Dykes
    10724083
*/

//Holds all the functions required for taking samples, buffering samples, sending samples and processing the sample data

#include "sample.hpp"
#include "Output.hpp"


Ticker timer;

EventQueue queue;

Semaphore flush_semaphore;    //for SD card writing

bool sampleOn = 1;            //To check if sampling has been enabled

bool gotFlushSem;

sampleData data;

std::vector<sampleData> dataBuffer;

Mail<sampleData, 10> mail_data;

void init(){
    data.samplenum = 0;
}

void printsample(int sample_num, float temp, float pressure, float light_level, time_t timestamp){
    // Print the samples to the terminal
    printf("\n----- Sample %d -----\nTemperature:\t%3.1fC\nPressure:\t%4.1fmbar\nLight Level:\t%1.2f\n", sample_num,temp,pressure,light_level);

    struct tm* tt;                  // Create empty tm struct
    tt = localtime(&data.timestamp);      // Convert time_t to tm struct using localtime
    
    printf("%s\n",asctime(tt));     // Print in human readable format

    // Write the current light level as a float to the 7-segment display
    latchedLEDs.write_seven_seg(light_level);
}

void thresholdsample(float temp, float pressure, float light_level){
    // If the current light level is above a threshold take action
    bool lightT = 0; bool tempT=0; bool presT=0;

    //Set the thresholds with hysteresis
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

void sampleThread(){    //Grabs a sample if sampling is enabled
    if(sampleOn){
        data.getsample();
        queue.call(sampleP);    //passes the data processing functions of to another thread
    }
}

void timerISR(){        //adds the sample thread to the queue
    queue.call(sampleThread);
    //Can't call functions in an ISR if semaphores are being used in it
}

void sampleP(){

    adddataBuffer(data.samplenum, data.temp, data.pressure, data.light_level, data.timestamp);
    
    printsample(data.samplenum, data.temp, data.pressure, data.light_level, data.timestamp);

    thresholdsample(data.temp, data.pressure, data.light_level);

    stripLED(data.temp, data.pressure, data.light_level, data.mode);

}

void adddataBuffer(uint32_t sample_num, float temp, float pressure, float light_level, time_t timestamp){
    sampleData newsample = {sample_num, temp, pressure, light_level};
    sampleData *mail = mail_data.alloc();   //allocate memory for the mailbox
    if(!mail_data.full()){

        mail->samplenum = sample_num;
        mail->temp = temp;
        mail->pressure = pressure;
        mail->light_level = light_level;
        mail->timestamp = timestamp;
        //add sampledata to the mailbox
        mail_data.put(mail);


    } 
    if(mail_data.full()){
        flush_semaphore.release();  //release the semaphore to allow the write to buffer thread to run.
    }
}


void writeBufferToSD() {

    while(true) {

        flush_semaphore.acquire();
        sampleOn = 0;   //turn off sampling to prevent race conditions
        // Check if the SD card is inserted
        if (sd.card_inserted()) {
            //clear the data in the document on startup
            
            
            char SDsend[2048];  //initialise the array to hold the text data being sent
            SDsend[0] = '\0';   //empty on init

            while (!mail_data.empty()) {
                osEvent evt = mail_data.get();  // Get data from the mailbox
                if (evt.status == osEventMail) {  // Check if a mail item is received
                    sampleData* dataSD = (sampleData*)evt.value.p;

                    //Prepare the timestamp
                    struct tm* tt = localtime(&dataSD->timestamp);  //Convert time_t to tm struct using localtime
                    char timeStr[26];  //Array to store the formatted time string
                    asctime_r(tt, timeStr);  //Convert tm struct to string (reentrant version of asctime)
                    timeStr[strlen(timeStr) - 1] = '\0';  //Remove the newline at the end

                    //Format the sample data into a string
                    char text_to_write[256];  //array to store each samples data to go into the main array buffer
                    snprintf(text_to_write, sizeof(text_to_write),
                        "\n----- Sample %d -----\nTemperature:\t%3.1fC\nPressure:\t%4.1fmbar\nLight Level:\t%1.2f\n%s\n\n",
                        dataSD->samplenum, dataSD->temp, dataSD->pressure, dataSD->light_level, timeStr);

                    //Put the buffered sample into the main buffer
                    strncat(SDsend, text_to_write, sizeof(SDsend) - strlen(SDsend) - 1);

                    //Free the mailbox item after writing the data
                    mail_data.free(dataSD);
                }   
            }

            int err = sd.write_file("sample.txt", SDsend, true);  //send data buffer to SD card
            if (err == 0) {
                printf("Successfully written to SD card\n");
                //sd.print_file("sample.txt", false);  //Print file contents for debug
            } else {
                printf("Error writing to SD card\n");
            }
        } else {
            printf("SD card not inserted\n");
        }
        sampleOn = 1;   //turn sampling back on
    }
}