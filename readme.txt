1.-Compulsory Done 10
A Ticker class timer is used to set a time of 10 seconds which enters an ISR after triggering. The ISR collects a sample and then adds the data processing functions to an event queue. The sample collection is done by a dedicated thread of the highest priority while the event queue is handled by another thread. The data is held within the structure sampleData, taking each measurement reading as a float. This class also holds the integer sample number, integer mode (used for the LED strip reading), and time_t timestamp.

2.Done 5
Time and date data is written to the SD card along with each sample. A new timestamp is taken with each sample. The time and date can be changed via the terminal; a command "datetime" must be typed into the terminal with the new date followed by the new time. The timestamp is taken from the real time clock in the epoch format. The timestamp is stored in epoch format and then converted into a human readable format while the data in the mailbox buffer is being converted into a format to be sent over to the SD card.

3.-Compulsory Done 15
A mailbox is setup to buffer 10 samples; once finished, the samples are written to the SD card all at once. The mailbox is used as a FIFO buffer before all of the samples are setup in a certain format and written to a text file on the SD card. Each time a sample is added to the buffer, the mailbox is checked to see if it's full and if it is, a semaphore is released. Another dedicated thread is constantly trying to acquire the semaphore and blocks the rest of the function until it can get the semaphore. The write to SD function checks to make sure that the SD card is still inserted, sets up the buffer for the formatted data and then goes through the cycle of formatting the mailbox samples before sending them off. The loop that formats each sample starts off with ah while condition that checks if the mailbox is empty each loop. The first sample is then taken from the mailbox and a check is done to ensure data has been received. The timestamp is converted to a readable format, then the sample data is also formatted in a temporary buffer that gets written to the larger formatted buffer. The first sample of the mailbox is then freed. Once the loop exits, the large formatted buffer is then sent off to the SD card and a check is done to ensure that it has been sent properly.

4.Done 10
Within the data processing functions is one that compares the readings to a set of thresholds. There is a threshold value for each reading with hysteresis included in each one. Once a threshold is set off, the buzzer is set off along with flashing lights on the LED strip. The thresholds are employed using if statements and the buzzer is done by setting it off, then sending the thread to sleep for 200ms before turning them off again. This happens each time a sample is collected until the values falls back within the hysteresis values. 

5.-Compulsory Done 15
There are a total of 5 threads running at once - including the main. There are 3 dedicated threads: the highest priority thread for collecting the samples and sending the data to the mailbox, a thread for taking user inputs through the terminal, and a thread for writing the data buffer to the SD card.

6.Done 10
Where any code needs to be blocked, the thread is blocked so that it doesn't take up and processing time during the blocking. The only other times are when a dedicated thread is used and semaphores are used to manage when the function in the thread runs or when it relies on the user input - a non-predictable asynchronous event that needs to be polled constantly to get a real time reaction from the system. 

7.Done 10
A function constantly runs in a dedicated thread to monitor the users input. Standard classes are used as they are a dynamically allocated C++ function, so less memory will be used up with each user input. This also means that the variable is empty until a user inputs a function. Once the user inputs something into the terminal, a semaphore is released and the thread processes the user input. String stream is used to split the user input into a command and arguments. Each argument is then split and pushed into the arguments std::vector made up of strings. After outputting the users input back to the terminal, a set of if statements are set up to do different things depending on the command entered. "Sampling" enables or disables sampling depending on the arguments "1" or "0". A Boolean sampleOn is set which is checked in an if statement each time the sample function is called. "Select" determines what reading gets output on the LED strip with the arguments "T", "P", and "L" for temperature, pressure and light levels respectively. A global variable in the sampleData structure gets changed to a different number depending on what mode is enabled. "Flush" sends all the data in the mailbox to the SD card by releasing the flush semaphore. "Datetime" is used to change the time on the real time clock. The arguments are split into date and time and then further split into year, month, day and hour, minute, second using the string stream C++ function; split by delimiters. This is then converted back into epoch format and stored into the real time clock which updates the LCD in other functions.

8.Half done need to add more locks 4
Mitigate against locks

9.Done 10
Each reading is normalised to 24 - being the max reading. Each LED on the strip is then turned on according to what integer number the calculations come out with

10.Done/easy to finish 1
Structred and indented(NEED TO FINISH)

Total 89