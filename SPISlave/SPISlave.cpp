#include <Arduino.h>
#include "SPISlave.h"

volatile int cntr;
byte cmd;
uint8_t addr;
byte state;
float * datap;
byte * settingsp;
volatile byte flt_cnt;

union{
  float data_float;
  byte data_byte[sizeof(float)];
} data;

void init_SPI(void)
{
  //DDRB &= ~(1<<PB1) & ~(1<<PB2) & ~(1<<PB6) & ~(1<<PB7);
  DDRB &= ~((1<<PB5)|(1<<PB3)|(1<<PB2));   // SCK, MOSI and SS as inputs
  DDRB |= (1<<PB4);                    // MISO as output
  SPCR &= ~(1<<MSTR);                // Set as slave
  SPCR |=(1<<SPR0)|(1<<SPR1);       // divide clock by 16
  SPCR |= (1<<SPE);                  // Enable SPI
  SPCR |= (1<<SPIE);           // Enable Interrupt
}

void pointToData(float * data_point, byte * settings){
  datap = data_point;
  settingsp = settings;
}

void resetSPI(){
  if (digitalRead(SS) == HIGH){
    cmd = 0;
    state = 0;
    flt_cnt = 0;
    byte stat = SPSR;
  }
}

ISR (SPI_STC_vect)
{
  byte c = SPDR;
  Serial.println(c, HEX);
  switch (state){
    case 0:
      cmd = c;
      state = 1;
      break;
    case 1:
      addr = c;
      if (cmd == READ_CMD){
        if (addr == 0){
          data.data_float = *datap;
          while(!(SPSR & (1<<SPIF)));
          SPDR = data.data_byte[0];
          flt_cnt = 1;
        }else{
          SPDR = *(settingsp + addr);
        }
      }
      state = 2;
      break;
    case 2:
      if (cmd == READ_CMD){
        if (flt_cnt < FLOAT_SIZE){
         while(!(SPSR & (1<<SPIF)));
          SPDR = data.data_byte[flt_cnt++];
        }
      }else if (cmd == WRITE_CMD){
        *(settingsp+addr) = c;
      }
      break;
    default:
      break;
  }
}
