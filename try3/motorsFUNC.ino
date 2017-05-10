// what to do if THROTTLE_BT?


/* old version worked for a while 
void getSpeedOL()
{
     throttleValue = pulseIn(REC_MOTOR, HIGH, 25000); //throttleValue is speed setpoint open loop
     if ( throttleValue < 1000 || throttleValue > 2000)
     {
      throttleValue = throttleMid;
     }

     motorValue = constrain(map(throttleValue, throttleMid+buffTxIn,1850, 0, topDutyCycleOL),0,topDutyCycleOL);
     brakeValue = constrain(map(throttleValue, throttleMid-buffTxIn,1150, 0, topDutyCycleOL),0,topDutyCycleOL);
     Serial.print("transmitter (1150 to 1850) = " );     
     Serial.print(throttleValue); //integer between 1150 and 1850
}*/

double getVRef()
{
  double vRef_ = 0; // underscore for local variable vRef
  if (myThrottleMode == THROTTLE_RC)
  {
    throttleValue = pulseIn(REC_MOTOR, HIGH, 25000); // throttleValue is a Speed setpoint used for closed loop

    //in case Tx is out of range, set it to middle
    if ( throttleValue < 1000 || throttleValue > 2000)
    {
      throttleValue = throttleMid;
    } 
    
    //Serial.print(" throttleValue: ");
    //Serial.print(throttleValue);

    //translate this to desired speed range
    if (throttleValue >=  throttleMid)
    {
      vRef_ = constrain(mapdouble(throttleValue, throttleMid+buffTxIn, 1850, 0,topSpeed),0,topSpeed); 
    }
    else
    {
      vRef_ = constrain(mapdouble(throttleValue,throttleMid-buffTxIn, 1150, 0,-topSpeed),-topSpeed,0);
    }
  }
  
  else if (myThrottleMode == THROTTLE_SWITCHES || myThrottleMode == THROTTLE_SWITCHES_CURVATURE)
  {
    /*byte sw1byte = digitalRead(SW1) ? 0 : 1;
    byte sw2byte = digitalRead(SW2) ? 0 : 1;
    byte sw3byte = digitalRead(SW3) ? 0 : 1;
    byte baseSpeed = 0;
    if (digitalRead(SW1) == LOW || digitalRead(SW2) == LOW || digitalRead(SW3) == LOW){
      baseSpeed = SPEED1;
    }else{
      baseSpeed = 0;
    }
     vRef_ = 1*(sw1byte) + 2*(sw2byte) + 4*(sw3byte) + baseSpeed;*/
    if (digitalRead(SW1) == LOW)
    {
      vRef_ = SPEED1;
      if (digitalRead(SW2) == LOW)
      {
        vRef_ = SPEED2;
        if (digitalRead(SW3) == LOW)
        {
          vRef_ = SPEED3;
        }
      }
    }
    if (myThrottleMode == THROTTLE_SWITCHES_CURVATURE)
    {
      //option 1:
     //vRef_ = vRef_ - constrain(mCurvature*curvature,0,2);
      
      //option 2: 
      /*if (curvature > threshCurvature || curvature < -threshCurvature)
      {
        vRef_ = vRef_ - bCurvature;
      }*/
    }
  }

  else if (myThrottleMode == THROTTLE_BT)
  {}

  else if (myThrottleMode == THROTTLE_AUTO)
  {
    //vRef_ = getVRefMemory();
  }
    return vRef_;
}

void getSpeedLinear(unsigned long curTime, unsigned long maxTime){
  double vRef_linear = (((double)curTime)/maxTime)*getVRef();

  vMeas = wheelSpeed_1_Copy;
  vError = vRef_linear - vMeas; // in ft/sec
  vErrorTotal += vError;
  //Serial.print(" vRef: ");
  //Serial.print(vRef)
  
  //Serial.print(" vError: ");
  //Serial.print(vError);

  //throttleValue = throttleValue + kpSpeed*vError + kiSpeed*vErrorTotal; //kSpeed must be in us /(ft/sec). before they were both .5 
  //throttleValue = constrain(mapdouble(vError,-topSpeed,topSpeed,1000,2000),1000,2000); //convert back to microseconds
  
  //motorValue = constrain(map(throttleValue, throttleMid+buffTxOut,1850, 0, topDutyCycleCL),0,topDutyCycleCL);
  //brakeValue = constrain(map(throttleValue, throttleMid-buffTxOut,1150, 0, topDutyCycleCL),0,topDutyCycleCL);
  
  //Serial.print(" throttleValue (1150 to 1850) = " );     
  //Serial.print(throttleValue); //integer between 1150 and 1850,

  vValue = kpSpeed*vError + kiSpeed*vErrorTotal; 
  
  //Serial.print(" vValue: ");
  //Serial.print(vValue);

  // note: 5/6 added softstart (untested)
  motorValue = constrain(mapdouble(vValue,0+buffOut,50,0,255),0,255); 
  if(motorValue > topDutyCycleCL)    {
    motorValue = topDutyCycleCL;
  }
  brakeValue = constrain(mapdouble(vValue,0-buffOut,-1.0*50,0,255),0,255);
  if(brakeValue > topDutyCycleCL)    {
    brakeValue = topDutyCycleCL;
  }
}

// new version more like CL version

void getSpeedOL()
{
  vRef = getVRef();
  vMeas = wheelSpeed_1_Copy;
  vError = vRef-vMeas;
  //Serial.print(" vValue: ");
  //Serial.print(vValue);
  motorValue = constrain(mapdouble(vRef,0+buffOut,topSpeed,0,topDutyCycleOL),0,topDutyCycleOL); //should topSpeed be in here? or should it be a constant?. used 10 for a while 
  brakeValue = constrain(mapdouble(vRef,0-buffOut,-topSpeed,0,topDutyCycleOL),0,topDutyCycleOL);
}

void getSpeedCL()
{
  
  vRef = getVRef();
  
  vMeas = wheelSpeed_1_Copy;
  vError = vRef-vMeas; // in ft/sec
  vErrorTotal += vError;
  //Serial.print(" vRef: ");
  //Serial.print(vRef)
  
  //Serial.print(" vError: ");
  //Serial.print(vError);

  //throttleValue = throttleValue + kpSpeed*vError + kiSpeed*vErrorTotal; //kSpeed must be in us /(ft/sec). before they were both .5 
  //throttleValue = constrain(mapdouble(vError,-topSpeed,topSpeed,1000,2000),1000,2000); //convert back to microseconds
  
  //motorValue = constrain(map(throttleValue, throttleMid+buffTxOut,1850, 0, topDutyCycleCL),0,topDutyCycleCL);
  //brakeValue = constrain(map(throttleValue, throttleMid-buffTxOut,1150, 0, topDutyCycleCL),0,topDutyCycleCL);
  
  //Serial.print(" throttleValue (1150 to 1850) = " );     
  //Serial.print(throttleValue); //integer between 1150 and 1850,

  vValue = kpSpeed*vError + kiSpeed*vErrorTotal; 
  
  //Serial.print(" vValue: ");
  //Serial.print(vValue);

  // note: 5/6 added softstart (untested)
  motorValue = constrain(mapdouble(vValue,0+buffOut,50,0,255),0,255); 
  if(motorValue > topDutyCycleCL)    {
    motorValue = topDutyCycleCL;
  }
  brakeValue = constrain(mapdouble(vValue,0-buffOut,-1.0*50,0,255),0,255);
  if(brakeValue > topDutyCycleCL)    {
    brakeValue = topDutyCycleCL;
  }
  
}

