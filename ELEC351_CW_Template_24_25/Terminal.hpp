/*
    Connor Dykes
    10724083
*/

//Hold all the functions required for user input into the terminal and processing of the user input

#ifndef TERMINAL_HPP
#define TERMINAL_HPP

#include "sample.hpp"

void terminalInput();
void processUserInput();
void processDateTime(const std::string& date, const std::string& time);
void epochConvert(int year, int month, int day, int hour, int minute, int second);

extern Semaphore inputReadySemaphore;   //Let thread know that a user has input

extern std::string userInput;                 //store user input from terminal
extern std::vector<std::string> arguments;  //store arguments from the user input

extern bool gotSem;

#endif