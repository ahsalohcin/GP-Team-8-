#include "SPIMaster.h"

void setup(){
  Serial.begin(9600);
  SPIMasterInit();
}

void loop(){
  float val = ((float)random(-32768, 32767))+((float)random(0,99))/100;
  bool sendRes = sendData(11, val);
  float readRes = getData(11);
  Serial.println(val);
  Serial.println(sendRes);
  Serial.println(SPIReadError());
  Serial.println(readRes);
  Serial.println("-----------------------------------------------------");
  delay(1000);
}
