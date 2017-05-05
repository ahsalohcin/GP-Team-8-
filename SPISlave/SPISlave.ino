#include "SPISlave.h"

float * val;
byte valByte[10];

void setup(){
  Serial.begin(115200);
  Serial.println("HEY1");
  *val = 12456.798;
  init_SPI();
  pointToData(val, &valByte[0]);
}

void loop(){
  resetSPI();
}

