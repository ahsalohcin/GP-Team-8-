#ifndef useful_h
#define useful_h

//USEFUL
//***********************************************************************
#include <Arduino.h>
#include <math.h>
#include <Servo.h>
#include <SoftwareSerial.h>

unsigned long int prevLoop; // to track loop time

// useful function
double mapdouble(double x, double in_min, double in_max, double out_min, double out_max)
{
 return (double)(x - in_min) * (out_max - out_min) / (double)(in_max - in_min) + out_min;
}

// buttons
int SW1 = 24;
int SW2 = 25;
int SW3 = 26;
int SW4 = 27;

#endif
