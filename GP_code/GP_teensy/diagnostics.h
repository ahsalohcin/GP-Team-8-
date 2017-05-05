// DIAGNOSTICS
//************************************************************************
  double BATT_VOLTAGE_SENSE = 17; //pin number;
  double battVoltSenseVal;
  double battVoltVal;
  double battConst = 2.728*.992; // (R2+R1)/R1; R1 = 2.8k, R2 = 1.62k, also a fudge factor 
  double battPeriod = 1000; 
  double battPrevTime = 0;
  double lowBattWarning = 6.7;
  double getBattVoltVal();

  double EMF_SENSE_HIGH = 19;
  double emfSenseHigh = 0;
  double EMF_SENSE_LOW = 18;
  double emfSenseLow = 0;
  double emfConst = 3.0174; // (R3+R4)/R4; R3 = 2.32k, R4 = 1.15k
  double emfHigh; // emfHighV = (emfSenseHighVal - emfSenseLowVal)*emfConst;
  double emf; //emf = emfHigh-emfSenseLow

  double I_SENSE = 5;
  double iSenseV;
  double iCalc;
  double iConst = 2.5; // 5k /(RS*RL) RS = .02 RL = 100k. unreliable with RL = 100k? RL = 10k better for ADC

    /*
    //reading emf 
    emfSenseHigh = mapdouble(analogRead(EMF_SENSE_HIGH),0,1023,0,3.3);
    emfSenseLow = mapdouble(analogRead(EMF_SENSE_LOW),0,1023,0,3.3);
    emfHigh = (emfSenseHigh - emfSenseLow)*emfConst;
    emf = emfHigh-emfSenseLow;
    Serial.print(" emfSenseLow(V): ");
    Serial.print(emfSenseLow);
    Serial.print(" emfSenseHigh(V): ");
    Serial.print(emfSenseHigh);
    Serial.print(" emf(V): ");
    Serial.print(emf);
*/

    /*
    iSenseV = mapdouble(analogRead(I_SENSE),0,1023,0,3.3);
    iCalc = iConst * iSenseV;
    Serial.print(" iSenseV: ");
    Serial.print(iSenseV);
    Serial.print(" iCalc: ");
    Serial.print(iCalc); 
    */
