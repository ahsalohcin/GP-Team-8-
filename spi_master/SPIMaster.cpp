#include <Arduino.h>
#include <SPI.h>
#include "SPIMaster.h"


boolean read_error_n = true;

void SPIMasterInit(){
  pinMode(slaveSelect, OUTPUT);
  pinMode(MISO, INPUT);
  SPI.begin();
}

boolean SPIReadError(){
  return ~read_error_n;
}

boolean sendData(byte addr, float value){
  union Data data;
  byte CRC = 0x00;
  bool CRC_res = true;
  data.data_float = value;
  digitalWrite(slaveSelect, LOW);
  SPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE0));
  SPI.transfer(WRITE_CMD);
  if (CRC_EN){
    CRC = generateCRC8Byte(CRC, WRITE_CMD);
  }
  SPI.transfer(addr);
  if (CRC_EN){
    CRC = generateCRC8Byte(CRC, addr);
  }
  for (int i = 0; i < FLOAT_SIZE; i++){
    SPI.transfer(data.data_byte[i]);
    if (CRC_EN){
      CRC = generateCRC8Byte(CRC, data.data_byte[i]);
    }
  }
  if (CRC_EN){
    byte CRC_rec = sendReceive(0);
    CRC_res = (CRC == CRC_rec);
  }
  digitalWrite(slaveSelect, HIGH);
  SPI.endTransaction();
  return CRC_res;
}

float getData(byte addr){
  union Data data;
  byte CRC = 0;
  read_error_n = true;
  digitalWrite(slaveSelect, LOW);
  SPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE0));
  SPI.transfer(READ_CMD);
  SPI.transfer(addr);
  if (CRC_EN){
    CRC = generateCRC8Byte(CRC, READ_CMD);
  }
  SPI.transfer(0);
  for (int i = 0; i < FLOAT_SIZE; i++){
    data.data_byte[i] = sendReceive(0);
    if (CRC_EN){
      CRC = generateCRC8Byte(CRC, data.data_byte[i]);
    }
  }
  if (CRC_EN){
    byte crc_rec = sendReceive(0);
    read_error_n = (CRC == crc_rec);
  }
  digitalWrite(slaveSelect, HIGH);
  SPI.endTransaction();
  return data.data_float;
}

byte sendReceive(byte data){
  byte a = SPI.transfer(data);
  delayMicroseconds(30);
  return a;
}

byte generateCRC8Byte(byte CRC_in, byte val){
  return (byte)(CRC_LUT[(byte)(val^CRC_in)]);
}

