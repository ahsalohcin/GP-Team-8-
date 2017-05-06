state motorSaturationTimeout(double vError)
{
  // if we reset it
  if  ( vError >= saturationThresh )
  {
    if (counting == 0)
    {
      saturationStart = millis();
      counting = 1;
    }
    else 
    {
      if (millis()-saturationStart > saturationDuration)
      {
        myState = PAUSE;
        Serial.println(" Motor Sat Timeout ");
        return myState;
      }
    } 
  }
  else
  {
    counting = 0;
  }
  
  myState = PLAY;
  return myState;
}


state stateCheck()
{
  //Serial.println("checking state");
  // If pause switch is on  
  if (digitalRead(SW4) == LOW) 
    {
      myState = PAUSE;
      Serial.println("pause switch");
      return myState;
    }
  else
  {
    myState = PLAY;
  }
  //If Tx says to pause
  
  /*
  stateValue = pulseIn(REC_STATE, HIGH, 25000);
  Serial.print(" State Value: ");
  Serial.print(stateValue);
  if (stateValue > 1550 && stateValue < 2000 ) // channel is all the way high
    {myState = PAUSE;
    Serial.print(" stateValueTx: ");
    Serial.println(stateValue);
    return myState;}
  */
        
  // low Batt Warning
  
  if ( getBattVoltVal() < lowBattWarning )
  {
    if (millis() > 1000)
    {
      Serial.println("batt volts error");
      myState = PAUSE;
      return myState;
    }
  }
  else 
    {myState = PLAY;}
    
  //exceeding top speed 

  /*
  noInterrupts();
  wheelSpeed_L_Copy = wheelSpeed_L;
  interrupts();
    
  if (wheelSpeed_L_Copy > speedLimit )
    {myState = PAUSE;
    return myState;}

  */
/*
  myState = motorSaturationTimeout(vError);
  if (myState == PAUSE)
  {
    return myState;
  }
*/
  
  //Serial.println("reached bottom");
  //myState = PLAY;

    //Check for bluetooth message that might change state
    
  if(packetAvailable())
  {
    packetParse(); 
      if(BTStateValue == 1)
        myState = PLAY;
      else
      {
        myState = PAUSE;
        Serial.print(" BT Start Pause ");
      }
  }
  else
  {
      if(BTStateValue == 1)
        myState = PLAY;
      else
      {
        myState = PAUSE;
        Serial.print(" BT Continue Pause ");
      }
  }
  
  return myState;
}
