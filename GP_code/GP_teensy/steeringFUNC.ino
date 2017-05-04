void steerTx()
{
 //get data from Tx
 /* 
 servoValue = pulseIn(REC_SERVO,HIGH, 25000);

  if ( servoValue < 1000 || servoValue > 2000)
  {
    servoValue = servoMid;
  }
  
 //servoValue = map(servoValue,1350,1730,1000,2000); was used for calibration
 //map to servo steering range
   // 0-180
      //servoValue = constrain( map( servoValue, 1830, 1160, servoMid - servoRange, servoMid + servoRange ), servoMid - servoRange, servoMid + servoRange );
      //myServo.write(servoValue); 
   //1000-2000
      //servoValue = constrain(map( servoValue,1830,1160,servoMid - servoRange, servoMid + servoRange), servoMid - servoRange, servoMid + servoRange );
*/
 servoValue = servoMid-.4*servoRange;
 
 servoValue = constrain(servoValue,servoMid-servoRange,servoMid+servoRange);
 servoValue = map(servoValue, servoMid-servoRange,servoMid+servoRange,servoMid+servoRange,servoMid-servoRange);

 myServo.writeMicroseconds(servoValue);

 Serial.print(" servoValue: ");
 Serial.print(servoValue);
}

double getSteeringPID()
{
  double steerValue_=
  steerValue_ = servoMid + kSteering*xError;
  return steerValue_;
}

double getSteeringPP(int xError)
{
  double delta_;
  delta_ = atan2( xError*2.0 /l_d/l_d*wheelBase*fovWidth, 128.0); // rads
  delta_ = delta_ * 57.2958; // degrees

  return delta_; 
  
}

void steerCamera(double xRef, double xMeasured)
{
  double xError;
  xError = xRef - xMeasured; 

  delta = getSteeringPP(xError);

  steerValue = constrain(steerValue,servoMid-servoRange, servoMid+servoRange);
  myServo.writeMicroseconds(steerValue);
}

