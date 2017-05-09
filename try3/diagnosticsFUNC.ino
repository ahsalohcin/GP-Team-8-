double getBattVoltVal(){
      battVoltSenseVal = analogRead(BATT_VOLTAGE_SENSE);
      battVoltVal = battVoltSenseVal*battConst/1024.0*3.3;
      battPrevTime = millis();
      return battVoltVal;
} 
