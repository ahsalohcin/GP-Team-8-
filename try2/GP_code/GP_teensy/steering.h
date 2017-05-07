// STEERING
//************************************************************************
  // for camera steering
  double xError; // xRef - xMeasured (from camera);
  double xRef = 64.0; // center is 64;
  double xMeasured;  
  double steerValue; 
  //PID
  double kSteering =  9.84;
  double kSteeringM = .1; // dependence of k on vMeas. It is assumed that kSteering should be higher with high vMeas.   
  double getSteeringPID();
  
  void steerCamera(double xRef, double xMeasured); // steers using camera input 


  // pure pursuit 
  double wheelBase = .205; //m
  double l_d_actual = .81; //m near top, 5th tick
  double fovWidth = .64; //m near top, 5th tick
  
  double delta = 0.0;
  double deviation_us = 0.0;
  double curvature = 0.0;
  double kFudge = 1.0;
  double mFudge = 0; 
  double bFudge = 1.0;
  double l_d; // l_d = kFudge(l_d_actual*(b + mfudge*vMeas)). we expect a tuning factor and some dependence on velocity
  
  double getSteeringPP(int xError);

    
  
  // for Tx steering
  void steerTx(); // steers using Tx input
