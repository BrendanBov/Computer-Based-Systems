// Lab 5, Exercise 3
// g++ -std=c++14 Lab5EX3.cpp -o Lab5EX3 -lwiringPi

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

void writeString(string data) {
    int addr, i;
    // Move cursor
    addr = 0x80 ;

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

void printRacers() 
{
    //log counter
    counter++;
    int i = counter;
    string timestamp = formatTime(begin_time, high_resolution_clock::now());

    //format output through switch statement
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
void press_button() {
    //enter running state
    if (state == STOPPED) {
        state = RUNNING;
        begin_time = high_resolution_clock::now();
        } 
    //log racer if running
    else if (state == RUNNING) {
        printRacers();
    } 
    //enter stop during reset state
    else {
        state = STOPPED;
    }
    delay(500);
}


int main(){

    // Set up WiringPi
    if (wiringPiSetup () == -1) exit (1);

    // Init I2C, assign a buffer handler to variable fd
    fd = wiringPiI2CSetup(LCDAddr);

    init();
    print_info();

    //initialize variables
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

    //initialize screen
    clear();
    write(0,0,"00:00:000");

    while(true) 
    {
        //write during running state
        if(state == RUNNING)
        writeString(formatTime(begin_time, high_resolution_clock::now()));

        //enter reset loop
        while(digitalRead(BUTTON) && RUNNING) 
        {
            //continue to print during reset wait
            writeString(formatTime(begin_time, high_resolution_clock::now()));

            usleep(sleep_duration * 1000 * 1000);
            reset_counter += 1;
            if(reset_counter >= counter_max) 
            {
                //handle reset state
                state = RESET;
                reset_counter = 0;
                counter = 0;
                cout << "\nRESET\n\n";
                write(0,0, "Reset!    ");
                delay(500);
                break;
            }
        }
        reset_counter = 0;
    }

    
    /* Complete here to implement the stopwatch  */

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
