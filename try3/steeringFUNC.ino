void steerTx()
{
 //get data from Tx
  
 steerValue = pulseIn(REC_SERVO,HIGH, 25000);

  if ( steerValue < 1000 || steerValue > 2000)
  {
    steerValue = servoMid;
  }
  
 //steerValue = map(steerValue,1350,1730,1000,2000); was used for calibration
 //map to servo steering range
   // 0-180
      //steerValue = constrain( map( steerValue, 1830, 1160, servoMid - servoRange, servoMid + servoRange ), servoMid - servoRange, servoMid + servoRange );
      //myServo.write(steerValue); 
   //1000-2000
      //steerValue = constrain(map( steerValue,1830,1160,servoMid - servoRange, servoMid + servoRange), servoMid - servoRange, servoMid + servoRange );
 
 steerValue = constrain(steerValue,servoMid-servoRange,servoMid+servoRange);
 steerValue = map(steerValue, servoMid-servoRange,servoMid+servoRange,servoMid+servoRange,servoMid-servoRange);

 myServo.writeMicroseconds(steerValue);

 //Serial.print(" steerValue: ");
 //Serial.print(steerValue);
}

double getSteeringPID()
{
 
  double steerValue_;
  
  if (mySteerMode == STEER_PID)
  {
    steerValue_ = servoMid + kSteering*xError;
    //return steerValue_;
  }

  if (mySteerMode == STEER_PID_KP_SPEED_DEP)
  {
    steerValue_ = servoMid + kSteering*xError*(1+kSteeringM*vMeas);
    //return steerValue_;
  }
  steerValue_ = steerValue_ +kDSteering*xErrorDiff;
  return steerValue_;
  
}

double getSteeringPP(int xError)
{
  l_d = kFudge*(l_d_actual*(bFudge + mFudge*vMeas));
  curvature = xError*2.0/l_d/l_d*fovWidth/128;
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
  if (mySteerMode == STEER_PP)
  {
    steerValue = getSteeringPP(xError);
  }
  else if (mySteerMode == STEER_PID || mySteerMode == STEER_PID_KP_SPEED_DEP)
  {
    steerValue = getSteeringPID();
  }
  steerValue = constrain(steerValue,servoMid-servoRange, servoMid+servoRange);
  myServo.writeMicroseconds(steerValue);
}

