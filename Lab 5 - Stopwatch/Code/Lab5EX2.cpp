// Lab 5, Exercise 2
// g++ -std=c++14 Lab5EX2.cpp -o Lab5EX2 -lwiringPi

#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <string.h>
#include <chrono>
#include <ratio>
#include <ctime>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <unistd.h>
/* Complete the code: include the file lab_util.cpp  */
#include "lab_util.cpp"

using namespace std::chrono;
using namespace std;


/* Complete the code: Set button pin */
int BUTTON = 26;

// State enum definition
enum State {
	STOPPED,
	RUNNING,
	RESET
};

// Global variables
State state;
high_resolution_clock::time_point begin_time;
unsigned int counter;


void printRacers() 
{
    //increase counter each button press
    counter++;
    int i = counter;

    string timestamp = formatTime(begin_time, high_resolution_clock::now());

    //Format suffix through switch statement
    cout << i << " : " << i;
        switch(i % 10) 
        {
            case 1:
            if(i % 100 == 11) goto th;
            cout<< "st";
            break;
            case 2:
            if(i % 100 == 12) goto th;
            cout <<"nd";
            break;
            case 3:
            if(i % 100 == 13) goto th;
            cout << "rd";
            break;
            default:
            th:
            cout << "th";
            break;
        }

        cout << " place: " << timestamp << "\n";
}

/* Complete the code, implement the timer */
void press_button() 
{
    delay(1000);
    //enter running state
    if (state == STOPPED) {
        state = RUNNING;
        begin_time = high_resolution_clock::now();
        } 
    //log racer
    else if (state == RUNNING) {
        printRacers();
    } 
}


int main(){

    // Set up WiringPi
    if (wiringPiSetup() == -1) exit (1);

    //initialize states and variables
    state = STOPPED;
    counter = 0;
    unsigned int button_time = 0;

    cout << "Holding for long time to reset" << endl;


    /* Complete the code to implement the timer  */

    pinMode(BUTTON, INPUT);

    wiringPiISR(BUTTON, INT_EDGE_RISING, &press_button);

    //initialize reset variables
    double sleep_duration = 0.1;
    int reset_counter = 0;
    int counter_max = 4 / sleep_duration;

    while(true) 
    {
        //enter loop while awaiting reset
        while(digitalRead(BUTTON)) 
        {
            usleep(sleep_duration * 1000 * 1000);
            reset_counter += 1;
            if(reset_counter >= counter_max) 
            {
                //reset functions
                state = STOPPED;
                reset_counter = 0;
                counter = 0;
                cout << "\nRESET\n\n";
            }
        }
        reset_counter = 0;
    }
    /*
    Tips:
    1) Set digital pins to input, Use pinMode(PIN, MODE)
    2) Use wiringPiISR() to get the interupt
    3) Read the state of pin: digitalRead(PIN)
    4) Use delay() to set a delay
    5) Define the state for the timer
    6) Use the formatTime() in lab_util.cpp
    */


    return 0;
}