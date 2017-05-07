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



int median( int in[])
// returns median of array in. in needs to be an odd number of elements
{
  int n =sizeof(in)/sizeof(int);
  int i,j,temp;
  for (i=0;i<n-1;i++)
      {
        for(j=1;j<n;j++)
        {
          if (in[j]<in[i])
          {
            temp=in[i];
            in[i]=in[j];
            in[j]=temp;
          }
        }
      }
  return in[n/2];
}
