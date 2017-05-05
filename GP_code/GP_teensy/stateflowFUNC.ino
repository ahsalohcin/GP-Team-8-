
state stateCheck()
{
  //Serial.println("checking state");
  // If pause switch is on

  /*
  if (digitalRead(SW1) == LOW) 
    {
      myState = PAUSE;
      Serial.print(digitalRead(SW1));
      Serial.println("pause switch");
      return myState;
    }
  */
  //If Tx says to pause
  /*
  stateValue = pulseIn(REC_STATE, HIGH, 25000);
  if (stateValue > 1550 && stateValue < 2000 ) // channel is all the way high
    {myState = PAUSE;
    Serial.print(" stateValueTx: ");
    Serial.println(stateValue);
    return myState;}
  */
    
  //Check for bluetooth message that might change state

  
  if(packetAvailable())
     packetParse();
  if (myState == PAUSE)
     {return myState;}   
 
  // low Batt Warning
  
  if ( getBattVoltVal() < lowBattWarning )
  {
    if (millis() > 1000)
    {
      Serial.println("batt volts error");
      myState = PAUSE;
      return myState;
    }
    else 
    myState = PLAY;
  }
  //exceeding top speed 

  /*
  noInterrupts();
  wheelSpeed_L_Copy = wheelSpeed_L;
  interrupts();
    
  if (wheelSpeed_L_Copy > speedLimit )
    {myState = PAUSE;
    return myState;}

  */
  Serial.println("reached bottom");
  //myState = PLAY;
  return myState;
}
