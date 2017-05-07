// CAMERA AND PROCESSING
//************************************************************************
  int pinSI = 29;
  int pinCL = 30;
  int pinAO = 31;
  
  double out[128]; //straight from camera;
  double averaged[128]; //after moving average filter
  double differences[127];//after taking differences
  
  int threshhold = 100; // for threshholding technique
  unsigned int integrationPeriod = 4;
  unsigned int prevCameraTime = 0;
  int lineMinWidth = 8;
  int lineMaxWidth = 20;
  
  //gets one line of camera data
  void getline(double lineBuffer[]);
  
  //Calculates average of numsToAverage points around all elements in input
  // and stores each averaged point in result
  // input and result should be the same size.
  // input[]: Data to manipulate
  // arraySize: Size of input array
  // numsToAverage: Number of elements around point p to use to calculate the average
  // result[]: Array to return the results
  void averageElements(double input[], int arraySize, int numsToAverage, double result[]);
  
  //Calcualtes difference between adjencents elements of input and stores in result
  //result's size will be arraySize-1
  void diff(double input[], int arraySize, double result[]);
  
  //Finds max and min of input data, calculates center point bewtween max and min,
  //and returns the index of the calculated center point
  double center(double input[], int arraySize);
