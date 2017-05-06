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
float edge_pixel = 0;
uint8_t edge_loc = 128;
uint8_t cur_line = 0;
uint8_t max = 0;
uint8_t min = 0;
int last_time = millis();

void setup()    {
    //setup ports
    DDRD |= (1<<6); //set PortD pin 7 as output, use as aux
    DDRD |= (1<<7); //set PortD pin 7 as output, use as SI
    DDRB |= (1<<0); //set PortB pin 0 as output, use as CLK
    //setup ADC prescaler to 16
    ADCSRA |= 1<<ADPS2;
    ADCSRA &= ~(1<<ADPS1);
    ADCSRA &= ~(1<<ADPS0);

    //setup timer
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
    PORTD |= (1<<6);    //set aux high
    cur_line += 1;
    if (cur_line == NUM_LINES)   {
        cur_line = 0;
    }
    //compute sum
    PORTD &= ~(1<<6);   //set aux low
    for(int i=0; i<LINE_WIDTH; i++) {
        send_pixels[i] = send_pixels[i]*0.4 + 0.6*pixels[cur_line][i];
    }
    //find edge
    PORTD |= (1<<6);    //set aux high
    for(int i=0; i<LINE_WIDTH; i++) {
        edge_pixel = -0.5*send_pixels[i-1 >= 0 ? i-1 : 0] + 0.5*send_pixels[i] + 0*send_pixels[i+1 <= LINE_WIDTH ? i+1 : LINE_WIDTH];
        if(edge_pixel < min)    {
            min = i;
        }
        else if(edge_pixel > max)   {
            max = i;
        }
    }
    edge_pixel= (min+max)/2.0;
    PORTD &= ~(1<<6);   //set aux low
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
