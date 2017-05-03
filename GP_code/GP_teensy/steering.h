// STEERING
//************************************************************************
  // for camera steering
  double xError; // xRef - xMeasured (from camera);
  double xRef = 64.0; // center is 64;
  double xMeasured;  
  double kSteering = .546875; // 35/64 = .546875 for full range. will be adjusted later.
  double kSteeringMicro =  (double)(servoRangeMicro)/64.00;
     
  void steerCamera(double xRef, double xMeasured); // steers using camera input 
  
  // for Tx steering
  void steerTx(); // steers using Tx input
