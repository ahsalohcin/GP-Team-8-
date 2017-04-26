#include <Arduino.h>
#include <SPI.h>
#include "SPISlave.h"

byte cmd;
uint8_t addr;
byte state;
float * datap; //255 DATA POINTS
volatile byte cntr;

byte CRC = 0;

union{
  float data_float;
  byte data_byte[sizeof(float)];
} data;

void SPIreset(){
  if (digitalRead(SS) == HIGH){
    cmd = 0;
    state = 0;
    cntr = 0;
    CRC = 0;
    byte stat = SPSR;
  }
}

void SPIdataP(float * data_point){
  datap = data_point;
}

void SPISlaveInit(){
  SPCR |= _BV(SPE);
  SPCR |= _BV(SPIE);
  pinMode(MISO, OUTPUT);
  addr = 0;
  cntr = 0;
  state = 0;
  CRC = 0;
}

ISR (SPI_STC_vect){
  SPSR = 0;
  byte c = SPDR;
  switch (state){
    case 0: //assign cmd
      cmd = c;
      SPDR = c;
      state = 1;
      if (CRC_EN){
          CRC = generateCRC8Byte(CRC, c);
      }
      break;
    case 1:
      addr = c;
      if (cmd == READ_CMD){
        data.data_float = *(datap+addr);
        while(!(SPSR & (1<<SPIF)));
        SPDR = data.data_byte[0]; 
        if (CRC_EN){
          CRC = generateCRC8Byte(CRC, data.data_byte[0]);
        }
        cntr++;
      }else{
        if (CRC_EN){
            CRC = generateCRC8Byte(CRC, c);
        }
        SPDR = c;
      }
      state = 2;
      break;
    case 2:
      if (cntr < FLOAT_SIZE){
        if (cmd == READ_CMD){
          if (CRC_EN){
            CRC = generateCRC8Byte(CRC, data.data_byte[cntr]);
          }
          SPDR = data.data_byte[cntr++];
        }else if (cmd == WRITE_CMD){
          if (CRC_EN){
              CRC = generateCRC8Byte(CRC, c);
          }
          if (CRC_EN){
            SPDR = CRC;
          }else{
            SPDR = 0;
          }
          data.data_byte[cntr++] = c;
        }
      }
      if (cntr == FLOAT_SIZE){
        if (cmd == WRITE_CMD){
          if (addr > READ_ONLY){
            *(datap+addr) = data.data_float;
          }
          if (CRC_EN){
            SPDR = CRC;
            state = 4;
          }
        }else if(cmd == READ_CMD){
          if (CRC_EN){
            state = 3;
          }
        }
      }
      break;
    case 3:
      SPDR = CRC;
      state = 4;
      break;
    default:
      break;
  }
}

byte generateCRC8Byte(byte CRC_in, byte val){
  byte CRC_out = CRC_in;
  byte addr = (byte)(val^CRC_out);
  CRC_out = (byte)(CRC_LUT[addr]);
  return CRC_out;
}

