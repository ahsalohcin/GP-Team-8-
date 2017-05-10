// SERVO
//************************************************************************
  int REC_SERVO = 20;
  int SERVO = 9;
  
  int servoValue; //1000-2000 for full range. is actually less because of the physical stops 
  int servoMid = 1460; // 2/18 test: min 1140, max 1770 before running into mechanical stops
  int servoRange = 280;
  
  Servo myServo;
