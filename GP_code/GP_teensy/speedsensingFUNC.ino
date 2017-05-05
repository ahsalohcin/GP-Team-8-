void magnet_detect_L()
{
  wheelSpeed_L = (wheelDiameter*PI/12/3)/((millis()-prevHallTime_L)/1000.0); // converts to ft/sec
  prevHallTime_L = millis();
  Serial.println("magnet_detect_L");
}

void magnet_detect_R()
{
  wheelSpeed_R = (wheelDiameter*PI/12/3)/((millis()-prevHallTime_R)/1000.0); // converts to ft/sec
  prevHallTime_R = millis();
}

