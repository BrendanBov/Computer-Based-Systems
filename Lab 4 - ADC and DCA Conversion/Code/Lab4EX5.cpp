// Luke McIntyre, Brendan Bovenschen
// Lab 3, Exercise 5
// g++ -std=c++14 Lab4EX5.cpp -o Lab4EX5 -lwiringPi
// Reference: http://wiringpi.com/reference/i2c-library/


#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <string.h>
#include <iostream>

using namespace std;

/* Complete the code: I2c address of LCD, some LCD i2c address might be 0x27 */
int LCDAddr = 0x27;  

int BLEN = 1; // 1--open backlight.0--close backlight

int fd; // Linux file descriptor

int btnPresses = 0; //global variable defining button presses


/* Complete the code:Set button pin wPi26 */
int BUTTON = 26;


int button_state = LOW;
bool changed = false;


// Send an 16 bits data to LCD buffer
void write_word(int data){
    int temp = data;
    if ( BLEN == 1 )
        temp |= 0x08;
    else
        temp &= 0xF7;

    /* Complete the code: write into the device using wiringPiI2CWrite */
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

    /* Complete the code: 4bit mode 2 Lines & 5*8 dots */
    send_command(0x28);
    delay(5);

    send_command(0x0C); // Enable display without cursor
    delay(5);

    /* Complete the code: Clear screen */
    send_command(0x01); 
    
}

// Clear screen
void clear(){
    /* Complete the code:clear Screen */
    send_command(0x01);

}

// Print the message on the lcd
void write(int x, int y, const char data[]){
    int addr, i;
    int tmp;
    if (x < 0) x = 0; if (x > 15) x = 15;
    if (y < 0) y = 0; if (y > 1)  y = 1;

    /* Complete the code: Target address, Move cursor */
    addr =  0x80 + 0x40 * y + x;

    // Set the address
    send_command(addr);
    
    tmp = strlen(data);
    for (i = 0; i < tmp; i++){
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

void push_button() 
{
    clear();
    delay(500); //delay for debouncing
    if(digitalRead(BUTTON)) 
    {
        btnPresses++; //iterate button presses

        write(0,0,"PRESSED");

        //create string with button presses
        char str[127];
        sprintf(str,"times: %i", btnPresses);
        write(0,1,str);
        
        return;
    }
    write(0,0,"NOT PRESSED");
      

}

int main(){

    // Set up WiringPi
    if (wiringPiSetup () == -1) exit (1);    
    
    // Init I2C, assign a buffer handler to variable fd
    fd = wiringPiI2CSetup(LCDAddr);

    /* Complete the code: in the init function*/
    init();


    print_info();

    /* Complete here to print "PRESSED" or "NOT PRESSED"
      to the LCD screen using a hardware interrupt */
      pinMode(BUTTON, INPUT);


    write(0,0, "NOT PRESSED");

    /*
    Tips:
    1) Set digital pins to input, Use pinMode(PIN, MODE);
    2) Read the state of pin digitalRead(PIN);
    3) Implement interrupt: wiringPiISR() and define callback function;
    4) Fill in the code in function init() , write(), clear(), write_word()
    5) Use write() to display the messages
    6) Function sprintf() coulb be used for generate the string of press times
    7) You can declare a variable like flag, and update the flag value(Using 1 or 0) in callback function, 
       and check the flag in the main function then display the relevant messages
    */

    wiringPiISR(BUTTON, INT_EDGE_BOTH, &push_button);


    while(true) {
        /* Check the flag status based on the Tip 7*/
        /* Display  "PRESSED and the press times " or Display  "NOT PRESSED" */
    }


    return 0;
}