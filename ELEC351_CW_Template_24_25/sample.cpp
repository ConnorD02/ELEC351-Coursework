#include "sample.hpp"
#include "Output.hpp"


Ticker timer;

EventQueue queue;

Semaphore flush_semaphore(0, 1);    //for SD card writing
Semaphore bufferSemaphore(0, 1);    //for adding the data back to the buffer after the SD card write

bool sampleOn = 1;

bool cleartxt = 0;

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
    if(sampleOn){
        data.getsample();
        queue.call(sampleP);
    }
}

void timerISR(){
    queue.call(sampleThread);
}

void sampleP(){
    //dataLock.acquire();
    //add to buffer
    adddataBuffer(data.samplenum, data.temp, data.pressure, data.light_level, data.timestamp);
    
    //print sample results and time and date to terminal
    //queue.call(printsample);
    printsample(data.samplenum, data.temp, data.pressure, data.light_level, data.timestamp);

    //process the data
    //queue.call(sampleThread);
    thresholdsample(data.temp, data.pressure, data.light_level);

    //Led strip
    //queue.call(sampleThread);
    stripLED(data.temp, data.pressure, data.light_level, data.mode);

    //dataLock.release();
    
}

void adddataBuffer(uint32_t sample_num, float temp, float pressure, float light_level, time_t timestamp){
    sampleData newsample = {sample_num, temp, pressure, light_level};
    sampleData *mail = mail_data.alloc();   //allocate memory for the mailbox
    if(!mail_data.full()){

        struct tm* tt = localtime(&timestamp);  // Convert time_t to tm struct using localtime
        char timeStr[26];  // Array to store the formatted time string
        asctime_r(tt, timeStr);  // Convert tm struct to string (reentrant version of asctime)

        mail->samplenum = sample_num;
        mail->temp = temp;
        mail->pressure = pressure;
        mail->light_level = light_level;
        mail->timestamp = timestamp;
        //add sampledata to the mailbox
        mail_data.put(mail);

        // if(sample_num % 10 == 0){
        //     flush_semaphore.release();
        // }

    } 
    if(mail_data.full()){
        //put semaphore release here
        flush_semaphore.release();
    }
}


void writeBufferToSD() {

    while(true) {

        flush_semaphore.acquire();
        // Check if the SD card is inserted
        if (sd.card_inserted()) {

            if(cleartxt == 0){
                int clr = sd.write_file("sample.txt", "", false);  // Clear data in the file
                if (clr == 0) {
                    printf("Successfully written to SD card\n");
                    sd.print_file("sample.txt", false);  // Print file contents for debug
                } else {
                    printf("Error writing to SD card\n");
                }
                cleartxt = 1;
            }
            
            char SDsend[2048];
            SDsend[0] = '\0';   //empty on init

            while (!mail_data.empty()) {
                osEvent evt = mail_data.get();  // Get data from the mailbox
                if (evt.status == osEventMail) {  // Check if a mail item is received
                    sampleData* dataSD = (sampleData*)evt.value.p;

                    // Prepare the timestamp
                    struct tm* tt = localtime(&dataSD->timestamp);  // Convert time_t to tm struct using localtime
                    char timeStr[26];  // Array to store the formatted time string
                    asctime_r(tt, timeStr);  // Convert tm struct to string (reentrant version of asctime)
                    timeStr[strlen(timeStr) - 1] = '\0';  // Remove the newline at the end

                    // Format the sample data into a string
                    char text_to_write[256];  // Temporary buffer for each sample's formatted text
                    snprintf(text_to_write, sizeof(text_to_write),
                        "\n----- Sample %d -----\nTemperature:\t%3.1fC\nPressure:\t%4.1fmbar\nLight Level:\t%1.2f\n%s\n\n",
                        dataSD->samplenum, dataSD->temp, dataSD->pressure, dataSD->light_level, timeStr);

                    // Append the formatted data to the full_text buffer
                    strncat(SDsend, text_to_write, sizeof(SDsend) - strlen(SDsend) - 1);

                    // Free the mailbox item after processing
                    mail_data.free(dataSD);
                }   
            }

            int err = sd.write_file("sample.txt", SDsend, true);  // Append data to the file
            if (err == 0) {
                printf("Successfully written to SD card\n");
                //sd.print_file("sample.txt", false);  // Print file contents for debug
            } else {
                printf("Error writing to SD card\n");
            }

            // Free the mailbox space after writing
            // bufferSemaphore.release();
        } else {
            printf("SD card not inserted\n");
        } 
    }
}