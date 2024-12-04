#include "sample.hpp"


Ticker timer;

EventQueue queue;

Semaphore inputReadySemaphore(0, 1);  // Count 0 means no input is ready initially

int sample_num = 0;

bool sampleOn = 1;

sampleData data;

std::vector<sampleData> dataBuffer;

Mail<sampleData, 10> mail_data;

void printsample(float temp, float pressure, float light_level, time_t timestamp){
    // Print the samples to the terminal
    printf("\n----- Sample %d -----\nTemperature:\t%3.1fC\nPressure:\t%4.1fmbar\nLight Level:\t%1.2f\n", sample_num,temp,pressure,light_level);

    struct tm* tt;                  // Create empty tm struct
    tt = localtime(&data.timestamp);      // Convert time_t to tm struct using localtime
    
    printf("%s\n",asctime(tt));     // Print in human readable format

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
    if(sampleOn){
        data.getsample();
        queue.call(sampleP);
        adddataBuffer(data.samplenum, data.temp, data.pressure, data.light_level, data.timestamp);
    }
}

void timerISR(){
    queue.call(sampleThread);
}

void sampleP(){
    //dataLock.acquire();
    //add to buffer
    //queue.call(adddataBuffer(data.samplenum, data.temp, data.pressure, data.light_level, data.timestamp));
    
    //print sample results and time and date to terminal
    //queue.call(printsample);
    printsample(data.temp, data.pressure, data.light_level, data.timestamp);

    //process the data
    //queue.call(sampleThread);
    thresholdsample(data.temp, data.pressure, data.light_level);

    //Led strip
    //queue.call(sampleThread);
    stripLED(data.temp, data.pressure, data.light_level, 2);

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
        mail->timestamp = 
        //add sampledata to the mailbox
        mail_data.put(mail);
    } else{
        //add write to SD function here
        //Write current samples back to the mailbox after flushing it
    }
}


void writeBufferToSD(sampleData datatosend) {
    //set mailbox into a buffer each time something new comes in
    while (true) {
        osEvent evt = mail_data.get();  // Get an event from the mailbox
        if (evt.status == osEventMail) {  // Check if a mail item is received
            sampleData *sample = (sampleData*)evt.value.p;
            
            // Check if the SD card is inserted
            if (sd.card_inserted()) {

                int err1 = sd.write_file("sample.txt", "sample %d \n", sample->samplenum);
                int err2 = sd.write_file("sample.txt", "temp = %f\n", sample->temp);
                int err3 = sd.write_file("sample.txt", "pressure = %f\n", sample->pressure);
                int err4 = sd.write_file("sample.txt", "light = %f\n", sample->light_level);
                int err5 = sd.write_file("sample.txt", "%f\n\n", sample->timestamp);

                //NOTE: Also need to send the time and date with the samples.
                
                if(err1 && err2 && err3 && err4 == 0){   // If is successful, read the content of the file back
            printf("Successfully written to SD card\n");
            printf("---------------------------\nFile contents:\n");
            sd.print_file("sample.txt",false);
            printf("---------------------------\n");
                // Free the mailbox space after writing
            mail_data.free(sample);
        } else{
            printf("Error Writing to SD card\n");
        }
                //but for the samples in the buffer
                //fprintf(file, "Sample number: %d\n", samples[i]->samplenum);
                //fprintf(file, "Temperature: %.1f C, Pressure: %.1f mbar, Light Level: %.2f\n\n",
                //samples[i]->temp, samples[i]->pressure, samples[i]->light_level);

            } else {
                printf("SD card not inserted\n");
            }

            

            //clear the buffer
        }
    }
    
}

/*
    if (sd.card_inserted()) {
        // Open file for appending
        sampleData* samples = new sampleData;
        int error = sd.write_file("sample.txt", "sample %d \ntemp = %f, pressure = %f, light = %f\n\n", sample_num, );
        if (file != nullptr) {
            // Write each sample in the buffer to the file
            for (const auto &sample : sampleBuffer) {
                fprintf(file, "Temperature: %.1f C, Pressure: %.1f mbar, Light Level: %.2f\n",
                        sample.temp, sample.pressure, sample.light_level);
            }
            fclose(file);
            printf("Buffer written to SD card\n");
        } else {
            printf("Failed to open file for writing\n");
        }
    } else {
        printf("SD card not inserted\n");
    }
}

 SD card writing thread - flushes data to the SD card periodically
void sdCardWriteThread() {
    while (true) {
        // Periodically flush the buffer if it's not full yet
        ThisThread::sleep_for(5s);
        bufferLock.acquire();
        if (!sampleBuffer.empty()) {
            writeBufferToSD();
            sampleBuffer.clear(); // Clear the buffer after writing
        }
        bufferLock.release();
    }
}
*/

//Terminal commands

std::string userInput;                 //store user input from terminal
std::vector<std::string> arguments;  // To store arguments from the user input

void terminalInput() {
    while (true) {
        std::getline(std::cin, userInput);

        if (!userInput.empty()){
            inputReadySemaphore.release();  // Signal that input is ready
            processUserInput();
        }
    }
}

void processUserInput(){
    inputReadySemaphore.acquire();
    //printf("User input: %s\n", userInput.c_str());

    std::istringstream ss(userInput);
    //allows the command to be separated into a string and numbers
    std::string command;
    std::string argument;
    arguments.clear();  //clears all previous arguments

    

    ss >> command;

    while(ss >> argument){
        arguments.push_back(argument);
    }

    // Display the parsed input (for debugging)
    printf("Command: %s\n", command.c_str());
    printf("Arguments: ");
    for (const auto& arg : arguments) {
        printf("%s ", arg.c_str());
    }
    printf("\n");

    if(command == "datetime"){
        if (arguments.size() == 2) {
            std::string date = arguments[0]; // Get the date string
            std::string time = arguments[1]; // Get the time string
            // Call the function to process the datetime string
            processDateTime(date, time);
        }

    }else if(command == "flush"){
        //Write current samples in the buffer to the SD card
        printf("Flushing buffer\n");

    }else if(command == "select"){
        if(argument == "T" || "t"){
            //set temp on the LED strip
            printf("Set LED strip to temperature\n");
        }else if(argument == "P" || "P"){
            //set pressure on LED strip
            printf("Set LED strip to pressure\n");
        }else if(argument == "L" || "l"){
            //set light on the LED strip
            printf("Set LED strip to light\n");
        }

    }else if(command == "sampling"){
        if(argument == "0"){
            //turn off sampling
            sampleOn = 0;
            printf("Sampling OFF\n");
        }else if(argument =="1"){
            //turn on sampling
            sampleOn = 1;
            printf("Sampling ON\n");
        }
    }

}

void processDateTime(const std::string& date, const std::string& time){
    int day, month, year, hour, minute, second;

    std::stringstream ss(date);
    char delimiter;

    //extract all parts of the date and time
    ss >> day >> delimiter >> month >> delimiter >> year;

    //std::getline (ss, time);
    std::stringstream timepart(time);

    timepart >> hour >> delimiter >> minute >> delimiter >> second;

    printf("Setting to:\n");
    printf("Day: %d, Month: %d, Year: %d\n", day, month, year);
    printf("Hour: %d, Minute: %d, Second: %d\n", hour, minute, second);

   epochConvert(year, month, day, hour, minute, second); 

}

struct tm t;
time_t t_of_day;

void epochConvert(int year, int month, int day, int hour, int minute, int second){
    

    t.tm_year = year + 100;  // Year - 1900
    t.tm_mon = month - 1;     // Month, where 0 = jan, so we subtract 1
    t.tm_mday = day;          // Day of the month
    t.tm_hour = hour;
    t.tm_min = minute;
    t.tm_sec = second;

    //tmm->tm_isdst = -1;          // Is DST on? 1 = yes, 0 = no, -1 = unknown

    // Convert to time_t (seconds since the Epoch)
    t_of_day = mktime(&t);
 // t_of_day = localtime(tmm);      // Convert time_t to tm struct using localtime

    set_time(t_of_day);
}

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
            latchedLEDs.write_strip((1 << tempLED),RED);
            latchedLEDs.write_strip(0x00,GREEN);     //Turn off all LEDs on G bar
            latchedLEDs.write_strip(0x00,BLUE);     //Turn off all LEDs on B bar
        } else if(tempLED >= 8 && tempLED < 16){
            latchedLEDs.write_strip(0xFF,RED);      // Turn on all LEDs on R bar
            latchedLEDs.write_strip((1 << (tempLED - 8)),GREEN);
            latchedLEDs.write_strip(0x00,BLUE);     //Turn off all LEDs on B bar
        } else if(tempLED >= 16 && tempLED < 24){
            latchedLEDs.write_strip(0xFF,GREEN);    //Turn on all LEDs on G bar
            latchedLEDs.write_strip((1 << (tempLED - 16)),BLUE);
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
            for(int i = 0; i < presLED; i++){
            latchedLEDs.write_strip((1 << i),RED);
            }
            latchedLEDs.write_strip(0x00,GREEN);     //Turn off all LEDs on G bar
            latchedLEDs.write_strip(0x00,BLUE);     //Turn off all LEDs on B bar
        } else if(presLED >= 8 && presLED < 16){
            latchedLEDs.write_strip(0xFF,RED);      // Turn on all LEDs on R bar
            latchedLEDs.write_strip((1 << (presLED - 8)),GREEN);
            latchedLEDs.write_strip(0x00,BLUE);     //Turn off all LEDs on B bar
        } else if(presLED >= 16 && presLED < 24){
            latchedLEDs.write_strip(0xFF,GREEN);    //Turn on all LEDs on G bar
            latchedLEDs.write_strip((1 << (presLED - 16)),BLUE);
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
            for(int i = 0; i < lightLED; i++){
            latchedLEDs.write_strip((1 << i),RED);
            }
            latchedLEDs.write_strip(0x00,GREEN);     //Turn off all LEDs on G bar
            latchedLEDs.write_strip(0x00,BLUE);     //Turn off all LEDs on B bar
        } else if(lightLED >= 8 && lightLED < 16){
            latchedLEDs.write_strip(0xFF,RED);      // Turn on all LEDs on R bar
            for(int i = 0; i < (lightLED-8); i++){
            latchedLEDs.write_strip((1 << i),GREEN);
            }
            latchedLEDs.write_strip(0x00,BLUE);     //Turn off all LEDs on B bar
        } else if(lightLED >= 16 && lightLED < 24){
            latchedLEDs.write_strip(0xFF,RED);      // Turn on all LEDs on R bar
            latchedLEDs.write_strip(0xFF,GREEN);    //Turn on all LEDs on G bar
            for(int i = 0; i < (lightLED-16); i++){
            latchedLEDs.write_strip((1 << i),BLUE);
            }
        }else{
            latchedLEDs.write_strip(0xFF,RED);      // Turn on all LEDs on R bar
            latchedLEDs.write_strip(0xFF,GREEN);    //Turn on all LEDs on G bar
            latchedLEDs.write_strip(0xFF,BLUE);     //Turn on all LEDs on B bar
        }
    }

}