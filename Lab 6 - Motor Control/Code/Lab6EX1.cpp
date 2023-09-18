// Lab 6 Exercise 1 
//Brendan Bovenschen, Luke McIntyre
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

// g++ -std=c++14 Lab6EX1.cpp -o Lab6EX1 -lwiringPi

int main(void){
    
    wiringPiSetup();

    /* 1. set mode for the three pins, the pin mode of the PWM pin should be PWM_OUTPUT */
    const int PIN1 = 2;
    const int PIN2 = 3;
    const int PWM = 26;

    pinMode(PIN1, OUTPUT);
    pinMode(PIN2, OUTPUT);
    pinMode(PWM, PWM_OUTPUT);


	/* 2. write HIGH or LOW to the two I/O pins to control the rotation direction */

    //Counter Clockwize
    digitalWrite(PIN1, LOW);
    digitalWrite(PIN2, HIGH);
    
    //Clockwize
    //digitalWrite(PIN1, LOW);
    //digitalWrite(PIN2, HIGH);

    pwmWrite(PWM, 512);

    
    /* 3. use function pwmWrite to control the motor speed. Refer to document pwm.pdf for more information. */
   

    //tips:	run the code using sudo ./Lab6EX1

    return 0;
}

