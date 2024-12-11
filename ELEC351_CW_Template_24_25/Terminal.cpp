/*
    Connor Dykes
    10724083
*/

//Hold all the functions required for user input into the terminal and processing of the user input

#include "Terminal.hpp"
#include "sample.hpp"
#include "Output.hpp"

//Terminal commands

Semaphore inputReadySemaphore(0, 1);    //semaphore for flagging terminal input

std::string userInput;                 //store user input from terminal
std::vector<std::string> arguments;  // To store arguments from the user input
//Automatic memory management with std and allows for parsing

extern sampleData data;     //sampledata struct

bool gotSem;                //flag to see if semaphore has been acquired
extern std::atomic<bool> sampleOn;       //bool to control whether sampling is on or off
//Boolean is atomic so theres no race conditions

void terminalInput() {
    while (true) {
        std::getline(std::cin, userInput);
        //alows the user to input in real time regardless of the other threads

        if (!userInput.empty()){
            inputReadySemaphore.release();  //Signal that input is ready
            processUserInput();
        }
    }
}

void processUserInput(){
    gotSem = inputReadySemaphore.try_acquire_for(1000ms);  
    if(gotSem){
    std::istringstream ss(userInput);
    //allows the command to be separated into a string and numbers
    std::string command;
    std::string argument;
    arguments.clear();  //clears all previous arguments



    ss >> command;

    while(ss >> argument){
        arguments.push_back(argument);
    }
    //Takes each argument and pushes it into the arguments vector as seperate variables.

    //Display the user input
    std::cout << "Command: " << command << std::endl;
    std::cout << "Arguments: " << std::endl;
    for (const auto& arg : arguments) {
        std::cout << arg << std::endl;
    }

    if(command == "datetime"){
        if (arguments.size() == 2) {
            std::string date = arguments[0]; //Get the date string
            std::string time = arguments[1]; //Get the time string

            processDateTime(date, time);
        }

    }else if(command == "flush"){
        //Write current samples in the buffer to the SD card
        printf("Flushing buffer\n");
        flush_semaphore.release();

    }else if(command == "select"){
        if(argument == "T" || argument == "t"){
            //set temp on the LED strip
            data.mode = 0;
            stripLED(data.temp, data.pressure, data.light_level, data.mode);
            printf("Set LED strip to temperature\n");
        }else if(argument == "P" || argument == "p"){
            //set pressure on LED strip
            data.mode = 1;
            stripLED(data.temp, data.pressure, data.light_level, data.mode);
            printf("Set LED strip to pressure\n");
        }else if(argument == "L" || argument == "l"){
            //set light on the LED strip
            data.mode = 2;
            stripLED(data.temp, data.pressure, data.light_level, data.mode);
            printf("Set LED strip to light\n");
        }

    }else if(command == "sampling"){
        if(argument == "0"){
            //turn off sampling
            sampleOn.store(false);
            printf("Sampling OFF\n");
        }else if(argument =="1"){
            //turn on sampling
            sampleOn.store(true);
            printf("Sampling ON\n");
        }
    }else{
        printf("Unknown Command\n");
    }
    }else{
        printf("Error: Couldn't get semaphore for user input!\n");
    }

}

void processDateTime(const std::string& date, const std::string& time){
    int day, month, year, hour, minute, second;

    std::stringstream ss(date);
    char delimiter;

    //extract all parts of the date and time
    ss >> day >> delimiter >> month >> delimiter >> year;

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