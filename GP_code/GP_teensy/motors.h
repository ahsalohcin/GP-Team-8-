// MOTORS
//************************************************************************

  int HI = 3; //pin numbers
  int LI = 4;
  
  int motorValue = 0;
  int brakeValue = 0;

  // allow Tx input fomr channel 3 (left stick vertical)
  int REC_MOTOR = 21; // pin to read signals from reciever
  int throttleValue;
  int topDutyCycleOL = 256*.5; //max speed
  int buffTxIn = 10; // buffer for tx input . full range of tx input is 1160 to 1830
  int throttleMid = 1500;
  void getSpeedOL(); //gets input from tx, assigns it to a desired speed, determines what to give HI and LI. 

  // closed loop speed control
  double vRef;
  double vMeas;
  double vError;
  double vValue; 
  double topSpeed = 5; // in ft/sec
  int buffTxOut = 5; // for safety. units of us. (tested)
  int buffOut = 1; // for safety. larger will mess up linearity . may be in units of speed (ft/sec).(untested)
  int topDutyCycleCL = 256*.5; // max speed 
  double kpSpeed = 3.0;
  double kiSpeed = 0.005;
  double vErrorTotal = 0.0;
  void getSpeedCL();
