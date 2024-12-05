#ifndef TERMINAL_HPP
#define TERMINAL_HPP
#include <iostream>
#include <sstream>
#include "uop_msb.h"
#include "mbed.h"
#include <chrono>
#include <vector>
#include <atomic>    // For std::atomic
#include "sample.hpp"

void terminalInput();
void processUserInput();
void processDateTime(const std::string& date, const std::string& time);
void epochConvert(int year, int month, int day, int hour, int minute, int second);

extern Semaphore inputReadySemaphore;

extern std::string userInput;                 //store user input from terminal
extern std::vector<std::string> arguments;  // To store arguments from the user input

extern bool gotSem;

#endif