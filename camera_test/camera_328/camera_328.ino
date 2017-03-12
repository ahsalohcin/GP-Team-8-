#include <avr/io.h>
#define LINE_WIDTH 130
#define NUM_LINES 1
#define A0 3
char s[sizeof(char)*5*LINE_WIDTH];
int pixels[NUM_LINES][LINE_WIDTH];
int current_line = 0;

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.print("DDRB:\t");
    Serial.println(DDRB);
    Serial.print("DDRC:\t");
    Serial.println(DDRC);
    Serial.print("DDRD:\t");
    Serial.println(DDRD);
    Serial.print("Set DDRC...\t");
    DDRC |= (1<<0); //set PortC pin0 as output; use as SI
    DDRC |= (1<<1); //set PortC pin1 as output; use as CLK
    Serial.print("done\n");
    Serial.print("DDRB:\t");
    Serial.println(DDRB);
    Serial.print("DDRC:\t");
    Serial.println(DDRC);
    Serial.print("DDRD:\t");
    Serial.println(DDRD);
    int line_num = 0;
    float tmp = 2;
    while(true) {
        PORTC |= (1<<0);    //set SI high;  PortC pin0
        PORTC |= (1<<1);    //set CLK high; PortC pin1
        PORTC &= ~(1<<0);   //set SI low;   PortC pin0
        PORTC &= ~(1<<1);   //set CLK low;  PortC pin1
        for(int i=1; i<LINE_WIDTH; i++) {
            PORTC |= (1<<1);    //set CLK high; PortC pin1
            PORTC &= ~(1<<1);   //set CLK low;  PortC pin1
        }
        delay(50);
        PORTC |= (1<<0);    //set SI high;  PortC pin0
        PORTC |= (1<<1);    //set CLK high; PortC pin1
        pixels[current_line][0] = analogRead(2);
        PORTC &= ~(1<<0);   //set SI low;   PortC pin0
        PORTC &= ~(1<<1);   //set CLK low;  PortC pin1
        for(int i=1; i<LINE_WIDTH; i++) {
            PORTC |= (1<<1);    //set CLK high; PortC pin1
            pixels[current_line][i] = analogRead(2);
            PORTC &= ~(1<<1);   //set CLK low;  PortC pin1
        }
        Serial.print("line_num:\t");
        Serial.print(line_num);
        Serial.print(";\t");
        for(int k=0; k<LINE_WIDTH; k++) {
            int j = 0;
            Serial.print(pixels[j][k]);
            Serial.print(",\t");
        }
        Serial.println("\n");
        line_num++;
        /*
        if(current_line = NUM_LINES)    {
            current_line = 0;
            for(int j=0; j<LINE_WIDTH; j++) {
                Serial.print("line_num:\t");
                Serial.print(line_num);
                Serial.print(";\t");
                for(int k=0; k<LINE_WIDTH; k++) {
                    Serial.print(pixels[j][k]);
                    Serial.print(",\t");
                }
                Serial.println("\n");
                line_num++;
            }
            Serial.println("\n");
        }
        */
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
