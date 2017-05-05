#ifndef speedsensing_h
#define speedsensing_h


// SPEED SENSING
//************************************************************************
  unsigned int hallTimeout = 500; // in millisec
  int speedLimit = 15; //higher than this will pause it. 
  double wheelDiameter = 1.8; //inches
  
  int hallPin_L = 1;//left
  int hallValue_L;
  double wheelSpeed_L;
  double wheelSpeed_L_Copy = 0; //copy to prevent memory issues
  volatile unsigned long int prevHallTime_L; // volatile to prevent memory issues
  void magnet_detect_L();
  
  int hallPin_R = 2;//right
  int hallValue_R;
  double wheelSpeed_R;
  double wheelSpeed_R_Copy=0; //copy to prevent memory issues
  volatile unsigned long int prevHallTime_R; // volatile to prevent memory issues
  void magnet_detect_R();

#endif
