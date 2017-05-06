#include <Arduino.h>
#include "SPISlave.h"

volatile int cntr;
byte cmd = 0;
uint8_t addr;
byte spi_state = 0;
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
  SPCR |=(1<<SPR0);
  //SPCR |= (1<<SPR1);       
  //SPSR |= (1<<SPI2X);
  SPCR |= (1<<SPE);                  // Enable SPI
  SPCR |= (1<<SPIE);           // Enable Interrupt
  spi_state = 0;
  cmd = 0;
  SPDR = 0;
}

void pointToData(float * data_point, byte * settings){
  datap = data_point;
  settingsp = settings;
}

void resetSPI(){
  if (digitalRead(SS) == HIGH){
    cmd = 0;
    spi_state = 0;
    flt_cnt = 0;
    byte stat = SPSR;
  }
}

ISR (SPI_STC_vect)
{
  if (spi_state == 163){
    spi_state = 0;
  }
  byte c = SPDR;
  //Serial.println(spi_state);
 switch (spi_state){
    case 0:
      cmd = c;
      spi_state = 1;
      break;
    case 1:
      addr = c;
      if (cmd == READ_CMD){
        if (addr == 0){
          data.data_float = *datap;
          while(!(SPSR & (1<<SPIF)));
          SPDR = data.data_byte[0];
          //Serial.println(data.data_byte[0], HEX);
          flt_cnt = 1;
        }else{
          while(!(SPSR & (1<<SPIF)));
          SPDR = *(settingsp + addr);
          cmd = 0;
          spi_state = 0;
          flt_cnt = 0;
          break;
        }
      }
      spi_state = 2;
      break;
    case 2:
      if (cmd == READ_CMD){
        if (addr != 0){
          spi_state = 0; 
          break;
        }
        if (flt_cnt < FLOAT_SIZE){
          //while(!(SPSR & (1<<SPIF)));
          SPDR = data.data_byte[flt_cnt++];
          //Serial.println(data.data_byte[flt_cnt-1], HEX);
        }
      }else if (cmd == WRITE_CMD){
        *(settingsp+addr) = c;
        cmd = 0;
        spi_state = 0;
        flt_cnt = 0;
        break;
      }
      break;
    default:
      spi_state = 0;
      break;
  }
}
