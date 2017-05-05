//#include "TimerOne/TimerOne.h"
#include "TimerOne.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#define LINE_WIDTH 128
#define NUM_LINES 4
#define PERIOD 50000 //in microseconds
#define INTEGRATE_PERCENT 100   //percent of the period with which to integrate the camera

//A0 is connected to ADC0
uint16_t pixels[NUM_LINES][LINE_WIDTH];
float send_pixels[LINE_WIDTH];
uint8_t cur_line = 0;
uint8_t poll_line = 0; //line it was at since last polled
uint8_t max = 0;
int last_time = millis();

void setup()    {
    DDRD |= (1<<7); //set PortD pin 7 as output, use as SI
    DDRB |= (1<<0); //set PortB pin 0 as output, use as CLK
    Timer1.initialize(PERIOD);
    Timer1.attachInterrupt(capture, PERIOD);
    Serial.begin(57600);
    Serial.print('test\n');
    delay(10);
    Serial.print('test2\n');
}
void loop() {
    while(millis() > last_time+100)   {
        last_time = millis();
        send_data();
    }
}
void capture()  {
    //read whole line from camera
    PORTD |= (1<<7);    //set SI high
    PORTB |= (1<<0);    //set CLK high
    pixels[cur_line][0] = analogRead(0);    //read 0th pixel from A0
    PORTD &= ~(1<<7);   //set SI low
    PORTB &= ~(1<<0);   //set CLK low
    for(int i=1; i<LINE_WIDTH; i++) {
    PORTB |= (1<<0);    //set CLK high
    pixels[cur_line][i] = analogRead(0);    //read i-th pixel
    PORTB &= ~(1<<0);   //set CLK low
    }
    //129th pulse
    PORTB |= (1<<0);    //set CLK high
    PORTB &= ~(1<<0);   //set CLK low
    //increment line count
    cur_line += 1;
    if (cur_line == NUM_LINES)   {
        cur_line = 0;
    }
    //compute sum
    for(int i=0; i<LINE_WIDTH; i++) {
        send_pixels[i] = send_pixels[i]*(NUM_LINES-1.0)/NUM_LINES + pixels[cur_line][i];
    }
}

void send_data()    {
    //send send_pixels
    //poll_line = cur_line;
    for(int i=0; i<LINE_WIDTH; i++) {
        Serial.print(send_pixels[i], 0);
        Serial.print(",\t");
    }
    Serial.print(";\n");

}
