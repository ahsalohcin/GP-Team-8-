#include "SPISlave.h"


void setup(){
  SPISlaveInit();
  float data_arr[255];
  SPIdataP(&data_arr[0]);
}

void loop(){
  SPIreset();
}
