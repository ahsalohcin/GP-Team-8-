#include "SPISlave.h"

float * val;
byte valByte[10];

void setup(){
  Serial.begin(115200);
  *val = 12456.798;
  valByte[2] = 0xEF;
  init_SPI();
  pointToData(val, &valByte[0]);
}

void loop(){
  resetSPI();
}

