#include "SPIMaster.h"

void setup(){
  Serial.begin(9600);
  SPIMasterInit();
}
float wrsuccess = 0;
float rdsuccess = 0;
int trials = 0;
float wr = 0;
float rd = 0;
void loop(){
  float val = ((float)random(-32768, 32767))+((float)random(0,99))/100;
  wr += sendData(11, val);
  float readRes = getData(11);
  if (!SPIReadError()){
    rd++;
  }
  trials++;
  
  wrsuccess = ((float)wr)/trials;
  rdsuccess = ((float)rd)/trials;
  Serial.print(wrsuccess);
  Serial.print(" / ");
  Serial.println(rdsuccess);
  Serial.print(val);
  Serial.print(" / ");
  Serial.println(readRes);
  //Serial.println(sendRes);
  //Serial.println(SPIReadError());
  Serial.println("-----------------------------------------------------");
  delay(100);
}
