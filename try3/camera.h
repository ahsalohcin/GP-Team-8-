// CAMERA AND PROCESSING
//************************************************************************
  int pinSI = 29;
  int pinCL = 30;
  int pinAO = 31;
  
  double out[128]; //straight from camera;
  double averaged[128]; //after moving average filter
  double differences[127];//after taking differences
  
  int threshhold = 300; // for threshholding technique
  unsigned int integrationPeriod = 5;
  unsigned int prevCameraTime = 0;
  int lineMinWidth = 1;
  int lineMaxWidth = 15 ;
  
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

// replaces i'th element of array "in" with median of that element with "num" elements to the left and right of i. doesn't change first or last few
  void medianArray(int num, double in[], double filtered[]);
  
  //Calcualtes difference between adjencents elements of input and stores in result
  //result's size will be arraySize-1
  void diff(double input[], int arraySize, double result[]);
  
  //Finds max and min of input data, calculates center point bewtween max and min,
  //and returns the index of the calculated center point
  double center(double input[], int arraySize);
