// Lab 5, Exercise 4
// g++ -std=c++14 Lab5EX4.cpp -o Lab5EX4 -lwiringPi

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

/* Complete the code: I2c address of LCD, some LCD i2c address might be 0x27 */
int LCDAddr = 0x27;  

int BLEN = 1; //1--open backlight.0--close backlight

int fd;//linux file descriptor

/* Complete the code: Set button pin */
int BUTTON1 = 26;
int BUTTON2 = 4;


// State enum definition
enum State {
	STOPPED,
	RUNNING,
	RESET
};

// Global variables
State state = RESET;
high_resolution_clock::time_point begin_time;
unsigned int counter;

string nameArr[1024]; //string to hold all racers
bool flagToWrite = false; //flag used to determine when to write to screen
unsigned int lastPlace = 0; // last place written to


// Send an 16 bits data to LCD buffer
void write_word(int data){
    int temp = data;
    if ( BLEN == 1 )
        temp |= 0x08;
    else
        temp &= 0xF7;

    wiringPiI2CWrite(fd, temp);
}

// Send control command to lcd
void send_command(int comm){
    int buf;
    // Send bit7-4 firstly
    buf = comm & 0xF0;
    buf |= 0x04;            // RS = 0, RW = 0, EN = 1
    write_word(buf);
    delay(2);
    buf &= 0xFB;            // Make EN = 0
    write_word(buf);
    // Send bit3-0 secondly
    buf = (comm & 0x0F) << 4;
    buf |= 0x04;            // RS = 0, RW = 0, EN = 1
    write_word(buf);
    delay(2);
    buf &= 0xFB;            // Make EN = 0
    write_word(buf);
}

// Send character to lcd
void send_data(int data){
    int buf;
    // Send bit7-4 firstly
    buf = data & 0xF0;
    buf |= 0x05;            // RS = 1, RW = 0, EN = 1
    write_word(buf);
    delay(2);
    buf &= 0xFB;            // Make EN = 0
    write_word(buf);
    // Send bit3-0 secondly
    buf = (data & 0x0F) << 4;
    buf |= 0x05;            // RS = 1, RW = 0, EN = 1
    write_word(buf);
    delay(2);
    buf &= 0xFB;            // Make EN = 0
    write_word(buf);
}

// Initialize the lcd
void init(){
    send_command(0x33); // Initialize 
    delay(5);
    send_command(0x32); // Initialize 
    delay(5);

    send_command(0x28);
    delay(5);

    send_command(0x0C); // Enable display without cursor
    delay(5);

    send_command(0x01);    
}

// Clear screen
void clear(){
    send_command(0x01);
}

// Print the message on the LCD
void write(int x, int y, const char data[]){
    int addr, i;
    int tmp;
    if (x < 0) x = 0; if (x > 15) x = 15;
    if (y < 0) y = 0; if (y > 1)  y = 1;

    addr = 0x80 + 0x40 * y + x;

    // Set the address
    send_command(addr);
    
    tmp = strlen(data);
    for (i = 0; i < tmp; i++){
        send_data(data[i]);
    }
}

void writeString(int x, int y, string data) {
    int addr, i;
    if (x < 0) x = 0; if (x > 15) x = 15;
    if (y < 0) y = 0; if (y > 1)  y = 1;

    // Move cursor
    addr = 0x80 + 0x40 * y + x;

    // set the address
    send_command(addr);
    
    for (i = 0; i < data.length(); i++){
        send_data(data[i]);
    }
}

void print_info()
{
    printf("\n");
    printf("|***************************|\n");
    printf("|    IIC 1602 LCD test      |\n");
    printf("| --------------------------|\n");
    printf("| | LCD |            | Pi   |\n");
    printf("| --------------------------|\n");
    printf("| | GND | connect to | GND  |\n");
    printf("| | VCC | connect to | 5V   |\n");
    printf("| | SDA | connect to | SDA.1|\n");
    printf("| | SCL | connect to | SCL.1|\n");
    printf("| --------------------------|\n");
    printf("|                   ECEN3213|\n");
    printf("|***************************|\n");
    printf("Program is running...\n");
    printf("Press Ctrl+C to end the program\n");
}

void saveRacers() 
{
    //log racer
    int i = counter + 1;
    //Generate racer string from switch (for place suffix)
    string timestamp = to_string(i);
        switch(i % 10) 
        {
            case 1:
            if(i % 100 == 11) goto th;
            timestamp += "st";
            break;
            case 2:
            if(i % 100 == 12) goto th;
            timestamp += "nd";
            break;
            case 3:
            if(i % 100 == 13) goto th;
            timestamp += "rd";
            break;
            default:
            th:
            timestamp += "th";
            break;
        }

        timestamp += ": " + formatTime(begin_time, high_resolution_clock::now());
        
        flagToWrite = true; // queue to write to lcd
        //save to string array
        nameArr[counter] = timestamp;
        counter++; //iterate counter
}

/* Complete the code, implement the timer */
void press_button1() {
    delay(200);
    //log racer when running
    if(state == RUNNING)
    {
        saveRacers();
    }
    //start during reset state, begin time
    else if(state == RESET)
    {
        state = RUNNING;
        begin_time = high_resolution_clock::now();
    }
    //scroll through racers
    else
    {
        //go back racers
        if(counter > 1) //prevent from going out of array
        {
            counter--;
            writeString(0,1, nameArr[counter - 1] + "  ");
        }
    }

}

void press_button2() 
{
    delay(200);
    //enter stop state
    if(state == RUNNING)
    {
        state = STOPPED;
        lastPlace = counter - 1; //used to prevent going out of array in traversal

    }
    //scroll through racers in stopped state
    else if (state == STOPPED)
    {
        //go forward racer
        if(counter <= lastPlace) //prevent from going out of array
        {
            counter++;
            writeString(0,1, nameArr[counter - 1] + "  ");
        }

    }

}



int main(){

    // Set up WiringPi
    if (wiringPiSetup () == -1) exit (1);

    // Init I2C, assign a buffer handler to variable fd
    fd = wiringPiI2CSetup(LCDAddr);

    init();
    print_info();

    counter = 0;
    unsigned int button_time = 0;

    cout << "Holding for long time to reset" << endl;


    /* Complete the code to implement the timer  */

    pinMode(BUTTON1, INPUT);
    pinMode(BUTTON2, INPUT);

    wiringPiISR(BUTTON1, INT_EDGE_RISING, &press_button1);
    wiringPiISR(BUTTON2, INT_EDGE_RISING, &press_button2);

    double sleep_duration = 0.1;
    int reset_counter = 0;
    int counter_max = 4 / sleep_duration;

    writeString(0,0,"00:00:000       ");
    writeString(0,1,"Reset           ");

    while(true) 
    {
        //write time to screen during running state
        if(state == RUNNING)
        {
            delay(2);
            writeString(0,0,formatTime(begin_time, high_resolution_clock::now()));
            if(flagToWrite) { //flag to write to prevent both from writing at the same time
                delay(2);
                writeString(0,1,nameArr[counter-1]); //counter in position after
                flagToWrite = false; //reset flag
            }
        }
        else if(state == STOPPED)
        {
            //Enter reset timer
            while(digitalRead(BUTTON1)) 
            {
                usleep(sleep_duration * 1000 * 1000);
                reset_counter += 1;
                if(reset_counter >= counter_max) 
                {
                    //handle reset
                    state = RESET;
                    reset_counter = 0;
                    counter = 0;
                    lastPlace = 0;
                    fill_n(nameArr, 1024, 0); //clear array
                    writeString(0,0,"00:00:000       ");
                    writeString(0,1,"Reset           ");
                    delay(500);
                    break;
                }
            }
        reset_counter = 0;
        }
    }

    return 0;
}
