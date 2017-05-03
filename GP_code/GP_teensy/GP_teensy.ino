#include "useful.h"
#include "drivemodes.h"
#include "speedsensing.h"
#include "motors.h"
#include "camera.h"
#include "myservoinit.h"
#include "steering.h"
#include "diagnostics.h"
#include "bluetooth.h"
#include "stateflow.h"

void printAll();

void setup() {
  Serial.begin(9600); //setting speed of communication in bits/second

  //bluetooth serial
    BTSERIAL.begin(9600);

  //state flow
    pinMode(REC_STATE,INPUT);
    pinMode(SW1,INPUT_PULLUP);
    pinMode(SW2,INPUT_PULLUP);
    pinMode(SW3,INPUT_PULLUP);
    pinMode(SW4,INPUT_PULLUP);

  //Motor things  
    pinMode(HI,OUTPUT);
    pinMode(LI,OUTPUT);

  //for transmitter throttle
    pinMode(REC_MOTOR,INPUT);
  
  //camera stuff
    pinMode(pinSI,OUTPUT);
    pinMode(pinCL,OUTPUT);
    pinMode(pinAO, INPUT);
  
  //from transmitter to control the servo
    pinMode(REC_SERVO,INPUT);
    myServo.attach(SERVO);

  //speed sensing
    pinMode(hallPin_L,INPUT);
    attachInterrupt(digitalPinToInterrupt(hallPin_L), magnet_detect_L, RISING); //set up interrupt function. should work on any teensy pin.

    pinMode(hallPin_R,INPUT);
    attachInterrupt(digitalPinToInterrupt(hallPin_R), magnet_detect_R, RISING); //set up interrupt function. should work on any teensy pin.

  // diagnostics
    pinMode(BATT_VOLTAGE_SENSE,INPUT);
    
    pinMode(EMF_SENSE_HIGH, INPUT);
    pinMode(EMF_SENSE_LOW,INPUT);

    pinMode(I_SENSE, INPUT);
    
  //indicator LED that program is running
    pinMode(14, OUTPUT);
    digitalWrite(14,HIGH);

    pinMode(15, OUTPUT);
    digitalWrite(15,HIGH);

    pinMode(16, OUTPUT);
    digitalWrite(16,HIGH);

    Serial.println("Setup Successful");
}

void loop() {

  //if (stateCheck()) // 1 if everything is fine
  if(stateCheck() == PLAY)
  {
    if (mySpeedMode = SPEED_CL)
    getSpeedCL();
    else 
    getSpeedOL();
  
  //Write to motor
     analogWrite(HI, motorValue); //HI denotes the pin on which the motor is in; motorValue represents the duty cycle of the PWM
     analogWrite(LI, brakeValue);
  
  //Get a line of camera data
    while(millis()- prevCameraTime < integrationPeriod) {}
    getline(out);
     
  //Determine the center
    averageElements(out,128,5,averaged);   
    diff(averaged,128,differences);
    xMeasured = center(differences,127);
    
  if (mySteerMode == AUTO_ST)  
    {//Get Tx steering input and writes to servo
    steerCamera(xRef,xMeasured);
    }
  else 
    {//Camera feedback to write to servo instead
    steerTx;
    }
    
  //Speed Sensing    
    if ( (millis()-prevHallTime_R) > hallTimeout )
    wheelSpeed_R = 0;
    
    noInterrupts(); // to prevent memory issues
    wheelSpeed_R_Copy = wheelSpeed_R;
    interrupts();

  //Diagnostics
    //batt voltage runs once in a while (moved to print loop)
    
    //reading motor current
    /*
    iSenseV = mapdouble(analogRead(I_SENSE),0,1023,0,3.3);
    iCalc = iConst * iSenseV;
    Serial.print(" iSenseV: ");
    Serial.print(iSenseV);
    Serial.print(" iCalc: ");
    Serial.print(iCalc); 
    */

   if (millis()-prevBtTime >btPeriod)
   {
   telemetry();
   prevBtTime = millis();
   }

   printAll();
  }

  else // PAUSE if pause signal is high
  {
     Serial.print("PAUSE");
     analogWrite(HI, 0);
     analogWrite(LI, 0);
     while (stateCheck() == PAUSE)
     {
      Serial.println("PAUSE");
      delay(100);
     }       
  }  
}

void printAll()
{
  Serial.print(" motor = ");
  Serial.print(motorValue); //corresponding value of the motor that runs at the value of PotValue
  Serial.print(" brake = ");
  Serial.print(brakeValue);

  Serial.print(" wheelSpeed_R: ");
  Serial.print(wheelSpeed_R_Copy);
  
  Serial.print(" xMeasured: ");
  Serial.print(xMeasured);

  if ( millis() - battPrevTime > battPeriod)
  {
  getBattVoltVal();
  }
  
  Serial.print(" Batt: ");
  Serial.print(battVoltVal);
  
  //Print loop time
  Serial.print("L: ");    
  Serial.println(micros()-prevLoop);
  prevLoop = micros();
}

