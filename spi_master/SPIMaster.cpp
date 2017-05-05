#include <Arduino.h>
#include <SPI.h>
#include "SPIMaster.h"

static volatile int cntr = 0;

void SPIMasterInit(){
  pinMode(slaveSelect, OUTPUT);
  pinMode(MISO, INPUT);
  SPI.begin();
}

void sendByte(byte addr, byte val){
  digitalWrite(slaveSelect, LOW);
  SPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE0));
  SPI.transfer(WRITE_CMD);
  SPI.transfer(addr);
  SPI.transfer(val);
  digitalWrite(slaveSelect, HIGH);
  SPI.endTransaction();
}

float getFlt(void){
  union Data data;
  digitalWrite(slaveSelect, LOW);
  SPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE0));
  SPI.transfer(READ_CMD);
  SPI.transfer(0);
  for (int i = 0; i < FLOAT_SIZE; i++){
    data.data_byte[i] = sendReceive(0);
    Serial.println(data.data_byte[i]);
  }
  digitalWrite(slaveSelect, HIGH);
  SPI.endTransaction();
  return data.data_float;
}

byte getByte(byte addr){
  byte val = 0;
  digitalWrite(slaveSelect, LOW);
  SPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE0));
  SPI.transfer(READ_CMD);
  SPI.transfer(addr);
  val = sendReceive(0);
  digitalWrite(slaveSelect, HIGH);
  SPI.endTransaction();
  return val;
}

byte sendReceive(byte data){
  byte a = SPI.transfer(data);
  delayMicroseconds(30);
  return a;
}
