void magnet_detect_L()
{
  wheelSpeed_L = (wheelDiameter*PI/12/3)/((millis()-prevHallTime_L)/1000.0); // converts to ft/sec
  Serial.println("magnet_detect_L");
}

void magnet_detect_R()
{
  wheelSpeed_R = (wheelDiameter*PI/12/3)/((millis()-prevHallTime_R)/1000.0); // converts to ft/sec
  prevHallTime_R = millis();
}


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

bool packetAvailable()
{
  while(BTSERIAL.available() > 0)
    {
      char incomingByte = BTSERIAL.read();

      if(incomingByte == MESSAGE_BEGIN)
      {
        //clear buffer for new incoming packet
        bufferPos = 0;
      }
      else if(incomingByte == MESSAGE_END)
      {
        //full packet received
        messageBuffer[bufferPos] = '\0';
        return true;
      }
      else
      {
        messageBuffer[bufferPos] = incomingByte;
        bufferPos++;
      }
         
    }

    return false;
}

void packetParse()
{  
  char packet_name[40];
  char packet_value[40] = {0};
  long int value = 0;
  
  //get divider position
  char *separator = strchr(messageBuffer, ':');
  *separator = '\0';

  //get command name
  strcpy(packet_name, messageBuffer);

  //skip white space
  *separator = ':';
  separator += 2;

  //get value
  for(int i = 0; *separator != '\0'; i++, separator++)
    packet_value[i] = *separator;

  exec_packet_cmd(packet_name, packet_value);

  BTSERIAL.write("\b");
  BTSERIAL.write(messageBuffer);
  BTSERIAL.write("\n");
}

void exec_packet_cmd(char *cmd_name, char* cmd_value)
{

  char chg_state[] = "change_state";
  char change_kpSpeed[] = "kpSpeed";
  char change_kiSpeed[] = "kiSpeed";

  if(strcmp(chg_state, cmd_name) == 0)
  {
      long int value = strtol(cmd_value, NULL, 10);

      if(value == 1)
        myState = PLAY;
      else
        myState = PAUSE;
  }
  else if(strcmp(change_kpSpeed, cmd_name) == 0)
  {
    double value = strtod(cmd_value, NULL);

    kpSpeed = value;
  }
  else if(strcmp(change_kiSpeed, cmd_name) == 0)
  {
    double value = strtod(cmd_value, NULL);

    kiSpeed = value;
  }
  
/*  
  Serial.write("name: ");
  Serial.println(cmd_name);

  Serial.write("value: ");
  Serial.println(cmd_value);
*/
}

void telemetry()
{
  /*
  Serial.print(" motor = ");
  Serial.print(motorValue); //corresponding value of the motor that runs at the value of PotValue
  Serial.print(" brake = ");
  Serial.print(brakeValue); 

  Serial.print("transmitter (1160 to 1830) = " );     
  Serial.print(throttleValue); //integer between 1160 and 1830

  Serial.print(" servoValueMicro: ");
  Serial.print(servoValueMicro);

  Serial.print(" steerValue: ");
  Serial.print(steerValue);

  */

  
  //String strdata = String("mtr:" + String(motorValue) + ";brk:" + String(brakeValue) + ";trst:" + String(throttleValue) + ";srv:" + String(servoValueMicro) + ";spd:" + String(wheelSpeed_R_Copy));
  String strdata = String( "vRef:" +String(vRef) + ";vError:" + String(vError));
  //Serial.println("Telemetry data: ");
  //Serial.println(strdata.c_str());

  //char data[strdata.length() + 1];
  BTSERIAL.write("\b");
  BTSERIAL.write(strdata.c_str());
  BTSERIAL.write("\n");
  //BTSERIAL.write("\b");
  //BTSERIAL.write("A");
  //BTSERIAL.write("\n");  
}

state stateCheck()
{
  //Serial.println("checking state");
  // If pause switch is on
  if (digitalRead(SW1) == LOW) 
    {
      myState = PAUSE;
      Serial.print(digitalRead(SW1));
      Serial.println("pause switch");
      return myState;
    }
  
  //If Tx says to pause
  stateValue = pulseIn(REC_STATE, HIGH, 25000);
  if (stateValue > 1550 && stateValue < 2000 ) // channel is all the way high
    {myState = PAUSE;
    Serial.print(" stateValueTx: ");
    Serial.println(stateValue);
    return myState;}
    
  //Check for bluetooth message that might change state

  /*
  if(packetAvailable())
     packetParse();
  if (myState == PAUSE)
     {return myState;}   
  */
  // low Batt Warning
  
  if ( getBattVoltVal() < lowBattWarning )
  {
    if (millis() > 2000)
    {
      Serial.println("batt volts error");
      myState = PAUSE;
      return myState;
    }
  }
  //exceeding top speed 

  
  noInterrupts();
  wheelSpeed_R_Copy = wheelSpeed_R;
  interrupts();
    
  if (wheelSpeed_R_Copy > speedLimit )
    {myState = PAUSE;
    return myState;}
 
  myState = PLAY;
  return myState;
}

void getline(int lineBuffer[])
{ 
  //digitalWrite(triggerPin2, HIGH);
  digitalWrite(pinSI, HIGH);
  digitalWrite(pinCL, HIGH);
  lineBuffer[0] = analogRead(pinAO);
  digitalWrite(pinSI, LOW);
  digitalWrite(pinCL, LOW);
 int i;
  for (i=1;i<128;i++)
  {
    digitalWrite(pinCL, HIGH);
    lineBuffer[i] = analogRead(pinAO);
    digitalWrite(pinCL, LOW);
    //Serial.print(out[i]);
    //Serial.print(" ");
  }
  //digitalWrite(triggerPin2, LOW);
  prevCameraTime = millis();
}

void averageElements(int input[], int arraySize, int numsToAverage, double result[])
{
  double sum;
  int elementsUsed; //keep track of how many elements were used for sum
  double average;

  for(int i = 0; i < arraySize; i++)
  {
    sum = 0;
    elementsUsed = 0;
  
    for(int j = 1; j <= numsToAverage; j++)
    {
      //check for valid index going to left of array
      if((i-j) >= 0)
      {
        sum += input[i-j];
        elementsUsed++;  
      }

      //check for valid index going to right of array
      if((i+j) < arraySize)
      {
        sum += input[i+j];
        elementsUsed++;
      }
    }// end of inner loop

    //calculate average
    average = sum / elementsUsed;
    result[i] = average;
    
  } //end of outer loop
  
}

void diff(double input[], int arraySize, double result[])
{
  for(int i = 1; i < arraySize; i++)
    result[i-1] = input[i] - input[i-1];
}

double center(double input[], int arraySize)
{
  //Index of max and min
  int maxIndex = 0;
  int minIndex = 0;
  int maxValue = 0;
  int minValue = 1000.0;  

  //Find max and min
  for(int i = 0; i < arraySize; i++)
  {
    if(input[i] < minValue)
    {
      minValue = input[i];
      minIndex = i;
    }
    else if(input[i] > maxValue)
    {
      maxValue = input[i];
      maxIndex = i;
    }
  }

  return (minIndex + maxIndex) / 2.0;
}
/*
void getPot()
{
     potentioValue = analogRead(potentioPin); //value between 0 and 1023 
     motorValue = constrain(map(potentioValue, 512+buff, 1023, 0, 256),0,256);
     brakeValue = constrain(map(potentioValue, 511-buff, 0, 0, 256),0,256);
     Serial.print("potentiometer = " );     
     Serial.print(potentioValue); //integer between 0 and 1023 
}
*/


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
double getBattVoltVal(){
      battVoltSenseVal = analogRead(BATT_VOLTAGE_SENSE);
      battVoltVal = battVoltSenseVal*battConst/1024.0*3.3;
      battPrevTime = millis();
      return battVoltVal;
} 

