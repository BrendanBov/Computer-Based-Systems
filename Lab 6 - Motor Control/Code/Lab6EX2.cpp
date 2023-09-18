// Lab 6 Exercise 2
//Brendan Bovenschen, Luke McIntyre

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;


// g++ -std=c++14 Lab6EX2.cpp -o Lab6EX2 -lwiringPi

//Pin integers
const int PIN1 = 2;
const int PIN2 = 3;
const int PWM = 26;
const int BUTTON1 = 4;
const int BUTTON2 = 1;

//speed counter to control speed
int counter = 0;


enum RunState {
	STOPPED,
	RUNNING
};

//begin in stopped state
RunState runState = STOPPED;

//Handles turning on and off the motor
//resets speed counter
void press_button1()
{
    delay(200); //Debouncing
    if(!digitalRead(BUTTON1)) return;

    if(runState == STOPPED)
    {
        counter = 0;
        runState = RUNNING;
        digitalWrite(PIN1, LOW);
        digitalWrite(PIN2, HIGH);
    }
    else
    {
        runState = STOPPED;
        digitalWrite(PIN1, LOW);
        digitalWrite(PIN2, LOW);
    }
}

//Handle changing motor speed
void press_button2()
{
    delay(200); //Debouncing
    if(!digitalRead(BUTTON2)) return;

    //Increment counter, set to 1024 if over max
    counter += 64;
    if(counter > 1024)
        counter = 1024;

    //write to motor
    pwmWrite(PWM, counter);
}

int main(void){
    
    wiringPiSetup();

    /* 1. set mode for the three pins, the pin mode of the PWM pin should be PWM_OUTPUT */

    //Pin instructions
    pinMode(PIN1, OUTPUT);
    pinMode(PIN2, OUTPUT);
    pinMode(PWM, PWM_OUTPUT);
    pinMode(BUTTON1, INPUT);
    pinMode(BUTTON2, INPUT);

    //Set up interrupts
    wiringPiISR(BUTTON1, INT_EDGE_RISING, &press_button1);
    wiringPiISR(BUTTON2, INT_EDGE_RISING, &press_button2);


	/* 2. write HIGH or LOW to the two I/O pins to control the rotation direction */

    //Set initial pin and motor conditions
    digitalWrite(PIN1, LOW);
    digitalWrite(PIN2, HIGH);
    pwmWrite(PWM, counter);

    //Non-terminating loop
    while(1){}

    return 0;
}

