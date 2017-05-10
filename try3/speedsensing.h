#ifndef speedsensing_h
#define speedsensing_h


// SPEED SENSING
//************************************************************************
  unsigned int hallTimeout = 500; // in millisec
  int speedLimit = 15; //higher than this will pause it. 
  double wheelDiameter = 1.8; //inches
  
  int hallPin_1 = 1;
  int hallValue_1;
  double wheelSpeed_1;
  double wheelSpeed_1_Copy = 0; //copy to prevent memory issues
  volatile unsigned long int prevHallTime_1; // volatile to prevent memory issues
  double totalDist = 0; // resolution is wheeldiameter*PI/3 inches;
  double totalDist_Copy = 0;
  void magnet_detect_1();
  
  int hallPin_2 = 2;// second one for polarity
  int hallValue_2;
  double wheelSpeed_2;
  double wheelSpeed_2_Copy=0; //copy to prevent memory issues
  volatile unsigned long int prevHallTime_2; // volatile to prevent memory issues
  void magnet_detect_2();

#endif
