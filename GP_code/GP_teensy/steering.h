// STEERING
//************************************************************************
  // for camera steering
  double xError; // xRef - xMeasured (from camera);
  double xRef = 64.0; // center is 64;
  double xMeasured;  
  double steerValue; 
  //PID
  double kSteering =  9.84;
  double getSteeringPID();
  
  void steerCamera(double xRef, double xMeasured); // steers using camera input 


  // pure pursuit 
  double wheelBase = .205; //m
  double l_d = .81; //m near top, 5th tick
  double fovWidth = .64; //m near top, 5th tick
  double delta = 0.0;
  double getSteeringPP(int xError);
    
  
  // for Tx steering
  void steerTx(); // steers using Tx input
