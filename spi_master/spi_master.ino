#include "SPIMaster.h"

void setup(){
  Serial.begin(9600);
  SPIMasterInit();
}

void loop(){
  sendByte(1, 0xAB);
  Serial.println(getFlt());
  delay(1000);
}
