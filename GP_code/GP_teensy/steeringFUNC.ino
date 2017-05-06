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

 //Serial.print(" servoValue: ");
 //Serial.print(servoValue);
}

double getSteeringPID()
{
  double steerValue_;
  steerValue_ = servoMid + kSteering*xError;
  return steerValue_;
}

double getSteeringPP(int xError)
{
  l_d = kFudge*(l_d_actual*(bFudge + mFudge*vMeas));
  curvature = xError*2.0/l_d/l_d_fovWidth/128;
  delta = atan2( curvature*wheelBase,1); // rads
  delta = delta * 57.2958; // degrees
  
  if (delta <= 9.25 && delta >= -9.25)
    {
      deviation_us = 15.676*delta;
    }
    else if ( delta > 9.25) 
    {
      deviation_us = -.6109*delta*delta + 29.1145*delta - 60.3992;
    }
    else if (delta < -9.25)
    {
      deviation_us = -(-.6109*delta*delta + 29.1145*(-delta) - 60.3992);
    }


  steerValue = servoMid + deviation_us;
  
  return steerValue; 
}
void steerCamera(double xRef, double xMeasured)
{
  xError = xRef - xMeasured;
  if (mySteerMode == STEER_PP)
  {
    steerValue = getSteeringPP(xError);
  }
  else if (mySteerMode == STEER_PID)
  {
    steerValue = getSteeringPID();
  }
  steerValue = constrain(steerValue,servoMid-servoRange, servoMid+servoRange);
  myServo.writeMicroseconds(steerValue);
}

