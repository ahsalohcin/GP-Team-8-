// SERVO
//************************************************************************
  int REC_SERVO = 20;
  int SERVO = 9;
  
  int servoValueMicro; //1000-2000 for full range. is actually less because of the physical stops 
  int servoMidMicro = 1455; // 2/18 test: min 1140, max 1770 before running into mechanical stops
  int servoRangeMicro = 315;
  
  Servo myServo;
