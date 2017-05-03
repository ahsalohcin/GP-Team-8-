void steerTx()
{
 //get data from Tx 
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
 servoValue = constrain(servoValue,servoMid-servoRange,servoMid+servoRange);
 servoValue = map(servoValue, servoMid-servoRange,servoMid+servoRange,servoMid+servoRange,servoMid-servoRange);
 myServo.writeMicroseconds(servoValue);
 Serial.print(" servoValue: ");
 Serial.print(servoValue);
}

void steerCamera(double xRef, double xMeasured)
{
  double xError;
  double steerValue;
  xError = xRef - xMeasured;
  //0-180
    //steerValue = servoMid + kSteering*xError; // for full range: 64 pixels --> 35 ms has kSteering = 35/64 = .546875
    //myServo.write(steerValue);
  //1000-2000
    steerValue = servoMid + kSteering*xError;
    steerValue = constrain(steerValue,servoMid-servoRange, servoMid+servoRange);
    myServo.writeMicroseconds(steerValue);
  //does fullrange camera to fullrange servo
  //steerValue = constrain( map( xMeasured, 128, 0, servoMid - servoRange, servoMid + servoRange ), servoMid - servoRange, servoMid + servoRange );
}

