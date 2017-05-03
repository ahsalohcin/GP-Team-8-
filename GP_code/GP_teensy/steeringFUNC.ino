void steerTx() //needs checking
{
 //get data from Tx 
 servoValueMicro = pulseIn(REC_SERVO,HIGH, 25000);

  if ( servoValueMicro < 1000 || servoValueMicro > 2000)
  {
    servoValueMicro = servoMidMicro;
  }
  
 //servoValue = map(servoValue,1350,1730,1000,2000); was used for calibration
 //map to servo steering range
   // 0-180
      //servoValue = constrain( map( servoValue, 1830, 1160, servoMid - servoRange, servoMid + servoRange ), servoMid - servoRange, servoMid + servoRange );
      //myServo.write(servoValue); 
   //1000-2000
      //servoValue = constrain(map( servoValue,1830,1160,servoMidMicro - servoRangeMicro, servoMidMicro + servoRangeMicro), servoMidMicro - servoRangeMicro, servoMidMicro + servoRangeMicro );
 servoValueMicro = constrain(servoValueMicro,servoMidMicro-servoRangeMicro,servoMidMicro+servoRangeMicro);
 servoValueMicro = map(servoValueMicro, servoMidMicro-servoRangeMicro,servoMidMicro+servoRangeMicro,servoMidMicro+servoRangeMicro,servoMidMicro-servoRangeMicro);
 myServo.writeMicroseconds(servoValueMicro);
 Serial.print(" servoValueMicro: ");
 Serial.print(servoValueMicro);
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
    steerValue = servoMidMicro + kSteeringMicro*xError;
    steerValue = constrain(steerValue,servoMidMicro-servoRangeMicro, servoMidMicro+servoRangeMicro);
    myServo.writeMicroseconds(steerValue);
  //does fullrange camera to fullrange servo
  //steerValue = constrain( map( xMeasured, 128, 0, servoMid - servoRange, servoMid + servoRange ), servoMid - servoRange, servoMid + servoRange );
   
  Serial.print(" steerValue: ");
  Serial.print(steerValue);
}

