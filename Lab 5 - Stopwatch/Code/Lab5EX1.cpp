//group 7 Brendan Bovenschen, Luke McIntyre
// Lab 5, Exercise 1
// g++ -std=c++14 Lab5EX1.cpp -o Lab5EX1 

#include <stdio.h>
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


// Global variables
high_resolution_clock::time_point begin_time;
unsigned int counter;

int main() {

    begin_time = high_resolution_clock::now();

    // Get hit time, increment counter
    high_resolution_clock::time_point hit_time; 

    for (int i = 0; i < 30; i++) {

        /* Complete the code: delay 1 second */
        sleep(1);


        /* Complete the code: increase counter  */


        hit_time = high_resolution_clock::now();


        // Calculate elapsed time, formatted as MM:SS:XXX
        string timestamp = formatTime(begin_time, hit_time);


        /* Complete the code: use if statement or switch statement to print the placement of counter, 
           Notice: 1 : 1st place: MM:SS:XXX
                   2 : 2nd place: MM:SS:XXX
                   3 : 3rd place: MM:SS:XXX
                   4 : 4th place: MM:SS:XXX
           */
        
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



        /* Complete the code: display the time on monitor */

        /*
        Tips:
        1) Complete the code in lab_util.cpp, Use sprintf() or  std::setfill(‘0’), std::setw(2) to get the time format
        2) Include the file lab_util.cpp in the Lab5Ex1.cpp
        3) Use formatTime() in lab_util.cpp to get the time difference, using hit_time and begin_time
        4) Use swith statement to format and print the placement of the time
         */

    }

    return 0;
}

