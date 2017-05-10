void magnet_detect_1()
{ 
  hallValue_1 = digitalRead(hallPin_1);
  hallValue_2= digitalRead(hallPin_2);
  
  if (myHallMode == TIMES_3)
  {
    wheelSpeed_1 = (wheelDiameter*PI/12/3)/((millis()-prevHallTime_1)/1000.0); // converts to ft/sec
    totalDist = totalDist + wheelDiameter*PI/12/3;
    prevHallTime_1 = millis();
  }
  
  else // then its TIMES_6 
  {
    wheelSpeed_1 = (wheelDiameter*PI/12/6)/((millis()-prevHallTime_1)/1000.0); // converts to ft/sec
    totalDist = totalDist + wheelDiameter*PI/12/6;
    prevHallTime_1 = millis();
  }
  
  if ( hallValue_1 == 1 && hallValue_2 == 1 )
  {
    wheelSpeed_1 = -wheelSpeed_1;
  }
  
  if (hallValue_1 == 0 && hallValue_2 == 0)
  {
    wheelSpeed_1 = -wheelSpeed_1;
  }
  
  //Serial.println("magnet_detect_1");
}

void magnet_detect_2()
{  
  wheelSpeed_2 = (wheelDiameter*PI/12/3)/((millis()-prevHallTime_2)/1000.0); // converts to ft/sec
  prevHallTime_2 = millis();
  //Serial.print(" magnet_detect_2: ");  
}

