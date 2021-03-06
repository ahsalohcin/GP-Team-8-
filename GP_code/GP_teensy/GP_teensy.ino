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

unsigned long int prevMicrooo = 0;
unsigned long int loopTime = 0;

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
    //digitalWrite(15,HIGH);

    pinMode(16, OUTPUT);
    //digitalWrite(16,HIGH);

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

  Serial.print(" 0: ");  
  Serial.print(micros()-prevMicrooo);
  prevMicrooo = micros();

  //if (stateCheck()) // 1 if everything is fine
  if(stateCheck() == PLAY)
  {
    digitalWrite(15, HIGH);
    if (mySpeedMode == SPEED_CL)
    getSpeedCL();
    else 
    getSpeedOL();
  Serial.print(" 1: ");  
  Serial.print(micros()-prevMicrooo);
  prevMicrooo = micros();
  //Write to motor
     analogWrite(HI, motorValue); //HI denotes the pin on which the motor is in; motorValue represents the duty cycle of the PWM
     analogWrite(LI, brakeValue);
  Serial.print(" 2: ");  
  Serial.print(micros()-prevMicrooo);
  prevMicrooo = micros();

  
  //Get a line of camera data
    while (millis()- prevCameraTime < integrationPeriod) 
    {}
      getline(out);
      prevCameraTime = millis();
  
  Serial.print(" 3: ");  
  Serial.print(micros()-prevMicrooo);
  prevMicrooo = micros();
  
  //Determine the center
    averageElements(out,128,2,averaged);   
    diff(averaged,128,differences);
    xMeasured = center(differences,127,xMeasured);

  Serial.print(" 4: ");  
  Serial.print(micros()-prevMicrooo);
  prevMicrooo = micros();

  xError = xRef-xMeasured;
  curvature = xError*2.0/l_d_actual/l_d_actual*fovWidth/128;
    
  if (mySteerMode == STEER_PP || mySteerMode == STEER_PID)  
    {//Get Tx steering input and writes to servo
    steerCamera(xRef,xMeasured);
    }
  else 
    {//Camera feedback to write to servo instead
    steerTx();
    }
  Serial.print(" 5: ");  
  Serial.print(micros()-prevMicrooo);
  prevMicrooo = micros();
    
  //Speed Sensing    
    if ( (millis()-prevHallTime_L) > hallTimeout )
    wheelSpeed_L = 0;
    
    noInterrupts(); // to prevent memory issues
    wheelSpeed_L_Copy = wheelSpeed_L;
    totalDist_Copy = totalDist;
    interrupts();
        
  Serial.print(" 6: ");  
  Serial.print(micros()-prevMicrooo);
  prevMicrooo = micros();
  //Diagnostics
    //batt voltage runs once in a while (moved to print loop)

   if (millis()-prevBtTime >btPeriod)
   {
   telemetry();
   prevBtTime = millis();
   }
  Serial.print(" 7: ");  
  Serial.print(micros()-prevMicrooo);
  prevMicrooo = micros();
    // can also read backemf and motor current
    //Serial.println("running")

  //Print loop time

 loopTime = micros()-prevLoop;
  Serial.print(" L: ");    
  Serial.println(loopTime);
  prevLoop = micros();

   printAll();
  }

  else // PAUSE if pause signal is high
  {
     digitalWrite(15, LOW);
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

  Serial.print(" Speed_L: ");
  Serial.print(wheelSpeed_L_Copy);
 
  Serial.print(" vError: ");
  Serial.print(vError);
   
  Serial.print(" HI ");
  Serial.print(motorValue); //corresponding value of the motor that runs at the value of PotValue
  Serial.print(" LI ");
  Serial.print(brakeValue);
 
  Serial.print(" xError: ");
  Serial.print(xError);

  Serial.print(" curvature: ");
  Serial.print(curvature);

  //Serial.print(" delta(deg): ");
  //Serial.print(delta);

  //Serial.print(" deviation_us : ");
  //Serial.print(deviation_us);  

  Serial.print(" steer: ");
  Serial.print(steerValue);

  if ( millis() - battPrevTime > battPeriod)
  {
  getBattVoltVal();
  }
  Serial.print(" Batt: ");
  Serial.print(battVoltVal);
}

