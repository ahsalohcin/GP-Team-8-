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
}

// needs a lot of work

void getSpeedCL()
{
  throttleValue = pulseIn(REC_MOTOR, HIGH, 25000); // throttleValue is a Speed setpoint used for closed loop
  if ( throttleValue < 1000 || throttleValue > 2000)
  {
    throttleValue = throttleMid;
  }
  Serial.print(" throttleValue: ");
  Serial.print(throttleValue);

  if (throttleValue >=  throttleMid)
  {
   vRef = constrain(mapdouble(throttleValue, throttleMid+buffTxIn, 1850, 0,topSpeed),0,topSpeed); 
  }
  else
  {
   vRef = constrain(mapdouble(throttleValue,throttleMid-buffTxIn, 1150, 0,-topSpeed),-topSpeed,0);
  }
    
  vMeas = wheelSpeed_R_Copy;
  vError = vRef-vMeas; // in ft/sec
  vErrorTotal += vError;
  Serial.print(" vRef: ");
  Serial.print(vRef);
  //Serial.print(" vError: ");
  //Serial.print(vError);

  //throttleValue = throttleValue + kpSpeed*vError + kiSpeed*vErrorTotal; //kSpeed must be in us /(ft/sec). before they were both .5 
  //throttleValue = constrain(mapdouble(vError,-topSpeed,topSpeed,1000,2000),1000,2000); //convert back to microseconds
  
  //motorValue = constrain(map(throttleValue, throttleMid+buffTxOut,1850, 0, topDutyCycleCL),0,topDutyCycleCL);
  //brakeValue = constrain(map(throttleValue, throttleMid-buffTxOut,1150, 0, topDutyCycleCL),0,topDutyCycleCL);
  
  //Serial.print(" throttleValue (1150 to 1850) = " );     
  //Serial.print(throttleValue); //integer between 1150 and 1850,

  vValue = kpSpeed*vError + kiSpeed*vErrorTotal; 
  Serial.print(" vValue: ");
  Serial.print(vValue);
  motorValue = constrain(mapdouble(vValue,0+buffOut,10,0,topDutyCycleCL),0,topDutyCycleCL); //should topSpeed be in here? or should it be a constant?
  brakeValue = constrain(mapdouble(vValue,0-buffOut,-10,0,topDutyCycleCL),0,topDutyCycleCL);
  
}

