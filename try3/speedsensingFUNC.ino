void magnet_detect_L()
{
  if (myHallMode == TIMES_3)
  {
    wheelSpeed_L = (wheelDiameter*PI/12/3)/((millis()-prevHallTime_L)/1000.0); // converts to ft/sec
    totalDist = totalDist + wheelDiameter*PI/12/3;
    prevHallTime_L = millis();
  }
  else 
  {
    wheelSpeed_L = (wheelDiameter*PI/12/6)/((millis()-prevHallTime_L)/1000.0); // converts to ft/sec
    totalDist = totalDist + wheelDiameter*PI/12/6;
    prevHallTime_L = millis();
  }
  //Serial.println("magnet_detect_L");
}

void magnet_detect_R()
{
  wheelSpeed_R = (wheelDiameter*PI/12/3)/((millis()-prevHallTime_R)/1000.0); // converts to ft/sec
  prevHallTime_R = millis();
}

