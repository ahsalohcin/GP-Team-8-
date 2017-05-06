#include "SPIMaster.h"

void setup(){
  Serial.begin(9600);
  SPIMasterInit();
}
unsigned long starttime = 0;
unsigned long delta = 0;
void loop(){
  sendByte(1, 0xAB);
  starttime = micros();
  byte b = getByte(1);
  delta = micros()-starttime;
  Serial.println(delta);
  
  Serial.println(getByte(2), HEX);
  
  starttime = micros();
  float f = getFlt();
  delta = micros()-starttime;
  Serial.println(delta);
  
  delay(1000);
}
