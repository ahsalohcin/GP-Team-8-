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

    Serial.print(" Modes: ");
    Serial.print(myThrottleMode);
    Serial.print(" ");
    Serial.print(mySpeedMode);
    Serial.print(" ");
    Serial.print(myMotorMode);
    Serial.print(" ");
    Serial.println(mySteerMode);  
}

void loop() {

  //if (stateCheck()) // 1 if everything is fine
  if(stateCheck() == PLAY)
  {
    if (mySpeedMode == SPEED_CL)
    getSpeedCL();
    else 
    getSpeedOL();
  //Serial.print(" 1: ");  
  //Serial.print(micros());
  //Write to motor
     analogWrite(HI, motorValue); //HI denotes the pin on which the motor is in; motorValue represents the duty cycle of the PWM
     analogWrite(LI, brakeValue);
  
  //Get a line of camera data
    while (millis()- prevCameraTime < integrationPeriod) 
    {}
      getline(out);
    
    
  //Serial.print(" 2: ");  
  //Serial.print(micros());
     
  //Determine the center
    averageElements(out,128,5,averaged);   
    diff(averaged,128,differences);
    xMeasured = center(differences,127);

     //Serial.print(" 3: ");  
     //Serial.print(micros());
    
  if (mySteerMode == STEER_PP || mySteerMode == STEER_PID)  
    {//Get Tx steering input and writes to servo
    steerCamera(xRef,xMeasured);
    }
  else 
    {//Camera feedback to write to servo instead
    steerTx();
    }
  //Serial.print(" 4: ");  
  //Serial.print(micros());
    
  //Speed Sensing    
    if ( (millis()-prevHallTime_L) > hallTimeout )
    wheelSpeed_L = 0;
    
    noInterrupts(); // to prevent memory issues
    wheelSpeed_L_Copy = wheelSpeed_L;
    interrupts();
  //Serial.print(" 5: ");  
  //Serial.print(micros());
  //Diagnostics
    //batt voltage runs once in a while (moved to print loop)

   if (millis()-prevBtTime >btPeriod)
   {
   telemetry();
   prevBtTime = millis();
   }
  //Serial.print(" 6: ");  
  //Serial.print(micros());

    // can also read backemf and motor current
    Serial.println("running");

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
      delay(500);
     }       
  }  
}

void printAll()
{
  Serial.print(" vRef: ");
  Serial.print(vRef);
  Serial.print(" HI ");
  Serial.print(motorValue); //corresponding value of the motor that runs at the value of PotValue
  Serial.print(" LI ");
  Serial.print(brakeValue);

  Serial.print(" Speed_L: ");
  Serial.print(wheelSpeed_L_Copy);
 
  Serial.print(" xMeas: ");
  Serial.print(xMeasured);


  Serial.print(" steer: ");
  Serial.print(steerValue);

  if ( millis() - battPrevTime > battPeriod)
  {
  getBattVoltVal();
  }
  
  Serial.print(" Batt: ");
  Serial.print(battVoltVal);

  Serial.print(" vError: ");
  Serial.print(vError);
 

  Serial.print(" delta: ");
  Serial.print(delta);
  //Print loop time
 
  Serial.print("L: ");    
  Serial.println(micros()-prevLoop);
  prevLoop = micros();
}

