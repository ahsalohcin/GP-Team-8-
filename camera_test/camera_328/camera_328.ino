#include <avr/io.h>
#define LINE_WIDTH 130
#define NUM_LINES 3
#define A0 3
//char s[sizeof(char)*5*LINE_WIDTH];
//int pixels[NUM_LINES][LINE_WIDTH];
//int current_line = 0;

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.print("DDRB:\t");
    Serial.println(DDRB);
    Serial.print("DDRC:\t");
    Serial.println(DDRC);
    Serial.print("DDRD:\t");
    Serial.println(DDRD);
    DDRC |= (1<<5); //set PortC pin0 as output; use as SI
    DDRC |= (1<<4); //set PortC pin1 as output; use as CLK
    Serial.print("DDRB:\t");
    Serial.println(DDRB);
    Serial.print("DDRC:\t");
    Serial.println(DDRC);
    Serial.print("DDRD:\t");
    Serial.println(DDRD);

    Serial.print("PORTB:\t");
    Serial.println(PORTB);
    Serial.print("PORTC:\t");
    Serial.println(PORTC);
    Serial.print("PORTD:\t");
    Serial.println(PORTD);
    while(true) {
        PORTB |= (1<<5);    //set SI high;  PortC pin0
        Serial.print("PORTB:\t");
        Serial.println(PORTB);
        Serial.print("PORTC:\t");
        Serial.println(PORTC);
        Serial.print("PORTD:\t");
        Serial.println(PORTD);
        delay(1000);
        PORTB &= ~(1<<5);    //set SI low;   PortC pin0
        Serial.print("PORTB:\t");
        Serial.println(PORTB);
        Serial.print("PORTC:\t");
        Serial.println(PORTC);
        Serial.print("PORTD:\t");
        Serial.println(PORTD);
        delay(1000);
    }
}

void loop() {
    /*
    PORTB |= (1<<5);    //set SI high;  PortC pin0
    delay(500);
    PORTB |= (0<<5);    //set SI low;   PortC pin0
    delay(500);
    */
    // nada
    //while(true) {   //main loop
    /*
        int i = 0;
        PORTB |= (1<<0);    //set SI high;  PortC pin0
        PORTB |= (1<<1);    //set CLK high; PortC pin1
        //pixels[current_line][i] = analogRead(A0);   //read ith pixel (0th pixel)
        delay(1);
        PORTB |= (0<<0);    //set SI low;   PortC pin0
        PORTB |= (0<<1);    //set CLK low;  PORTB pin1
        Serial.println(PORTB);
        for(i=1; i<LINE_WIDTH; i++) {
            PORTB |= (1<<1);    //set CLK high; PortC pin1
            Serial.println(PORTB);
            //pixels[current_line][i] = analogRead(A0);   //read ith pixel
            delay(1);
            PORTB |= (0<<1);    //set CLK low;  PORTB pin1
            Serial.println(PORTB);
        }  
        current_line++;
        if(current_line >= NUM_LINES)   {
            //s = {'0'};  //set all values to 0;
            sprintf(s, 'sdflkewj%d\n', pixels[0][0]);
            Serial.print(s);
            for(int j=0; j<NUM_LINES; j++)  {
            }
            current_line = 0;
        }
        Serial.println("end loop");
    */
    //}
}
