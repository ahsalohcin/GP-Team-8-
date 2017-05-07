#include <avr/io.h>
#include <avr/interrupt.h>
#define LINE_WIDTH 128
#define NUM_LINES 3

//A0 is connected to ADC0
int pixels[NUM_LINES][LINE_WIDTH];
int cur_line = 0;

void setup()    {
    DDRD |= (1<<7); //set PortD pin 7 as output, use as SI
    DDRD |= (1<<6); //set PortD pin 6 as output, use as SI
    DDRB |= (1<<0); //set PortB pin 0 as output, use as CLK
    DDRC |= (1<<0);

    cli();
    TCCR1B |= (1 << WGM12)|(1 << CS11)|(1 << CS10);
    // initialize counter
    TCNT1 = 0;
    // initialize compare value
    OCR1A = 24999;
    // enable compare interrupt
    TIMSK1 |= (1 << OCIE1A);
    // enable global interrupts
    sei();

    /*
    cli();  //global disable interrupts so that we can set them safely
    //set CS1[2:0] = b101   //set prescaler to clk/1024
    TCCR1B |= (1<<CS12);
    TCCR1B &= ~(1<<CS11);
    TCCR1B |= (1<<CS10);
    //set OCR1A to match value  //set to period in number of 64us (781 for 50ms)
    OCR1A = 1000;
    //set WGM1[3:0] = b0100  //set to CTC (Clear Timer on Compare)
    TCCR1B &= ~(1<<WGM13);
    TCCR1B |= (1<<WGM12);
    TCCR1A &= ~(1<<WGM11);
    TCCR1A &= ~(1<<WGM10);
    //set OCIEA in TIMSK1 to 1 to enable interrupt on match
    TIMSK1 |= (1<<OCIE1A);
    //set OCFA on TIFR1 to 1 to enable isr
    //TIFR1 |= (1<<OCF1A);
    sei();  //global enable interrupts
    */
}
void loop() {}

ISR(TIMER1_COMPA_vect)  {
    //read whole line from camera
    PORTC ^= (1<<0);
    PORTD |= (1<<7);    //set SI high
    PORTD |= (1<<6);    //set SI high
    //PORTB |= (1<<0);    //set CLK high
    pixels[cur_line][0] = analogRead(0);    //read 0th pixel from A0
    PORTD &= ~(1<<7);   //set SI low
    PORTD &= ~(1<<6);   //set SI low
    //PORTB &= ~(1<<0);   //set CLK low
    /*
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
    */
}
