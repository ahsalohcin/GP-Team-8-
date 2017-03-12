#include <avr/io.h>
#define LINE_WIDTH 130
#define NUM_LINES 3
#define A0 3

void setup()    {
    DDRC |= (1<<0); //set PortC pin0 as output; use as SI
    DDRC |= (1<<1); //set PortC pin1 as output; use as CLK
    while(true) {
        PORTC |= (1<<1);    //set CLK high; PortC pin1
        PORTC &= ~(1<<1);   //set CLK low;  PortC pin1
    }
}

void loop() {
}
