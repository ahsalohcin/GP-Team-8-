//control speed of DC motor with a potentiometer
//0-512 is braking, and 513-1023 is forward. There is an adjustable buffer;
//77 = .3 of 256. 180 = .7 of 256

#include <Servo.h>

// motor driver
int potentioPin = 14; //integer variable denoting the pin with the potentiometer, gets assigned a value from 0-1023 for 0-5
int motorPin = 3; //likewise with motor, arbitrary pin number 1
int brakePin = 4;
int triggerPin = 9; // help when looking at O-scope
int potentioValue = 0; //will be an integer between 0 and 1023
int motorValue = 0;
int brakeValue = 0;
int buff = 100; // deadzone

// transmitter and motor driver
int pinTx = 15; // pin to read signals from reciever
int topSpeed = 256;
int buffTx = 100; // buffer for tx input . full range of tx input is 1160 to 1830

// camera
int SI = 5;
int CL = 6;
int AO = 17;
int out[130];
int filtered[128];
int threshhold = 350;
unsigned int integrationPeriod = 50;
unsigned int prevCameraTime = 0;
void getline();
int findcenter();

//Calculates average of numsToAverage points around all elements in input
// and stores each averaged point in result
// input and result should be the same size.
// input[]: Data to manipulate
// arraySize: Size of input array
// numsToAverage: Number of elements around point p to use to calculate the average
// result[]: Array to return the results
void averageElmenents(int input[], int arraySize, int numsToAverage,int result[]);

//Calcualtes difference between adjencents elements of input and stores in result
//result's size will be arraySize-1
void diff(int input[], int arraySize, int result);

//servo
int servoPinIn = 18;
int servoPinOut = 20;
int servoValue; //0-180 for full range. less because of the physical stops 
int servoMid = 82;
int servoRange = 35;
Servo myServo;

//steering
int measuredMid = 0; 
void steer();

void setup() {
  Serial.begin(9600); //setting speed of communication in bits/second
    
  pinMode(potentioPin,INPUT); //turn on ADC at pin; ready to read voltages
  pinMode(motorPin,OUTPUT);
  pinMode(brakePin,OUTPUT);
  pinMode(triggerPin,OUTPUT);

  //indicator LED that program is running
  pinMode(13, OUTPUT);
  digitalWrite(13,HIGH);
  
  //for transmitter throttle
  pinMode(pinTx,INPUT);
  
  //camera stuff
  pinMode(SI,OUTPUT);
  pinMode(CL,OUTPUT);
  pinMode(AO, INPUT);
  
  //from transmitter to control the servo
  pinMode(servoPinIn,INPUT);
  myServo.attach(servoPinOut);
  //analogWriteFrequency(motorPin, 9800);
  //analogWriteFrequency(brakePin, 9800);
}

void loop() {
 /// pot  
 /*
     potentioValue = analogRead(potentioPin); //value between 0 and 1023 
     motorValue = constrain(map(potentioValue, 512+buff, 1023, 0, 256),0,256);
     brakeValue = constrain(map(potentioValue, 511-buff, 0, 0, 256),0,256);
     Serial.print("potentiometer = " );     
     Serial.print(potentioValue); //integer between 0 and 1023 
 */
 /// transmitter to set motor speed and brake speed
     potentioValue = pulseIn(pinTx, HIGH, 25000);
     motorValue = constrain(map(potentioValue, 1495+buffTx,1830, 0, topSpeed),0,topSpeed);
     brakeValue = constrain(map(potentioValue, 1495-buffTx,1160, 0, topSpeed),0,topSpeed);
     Serial.print("transmitter (1160 to 1830) = " );     
     Serial.print(potentioValue); //integer between 1160 and 1830

 analogWrite(motorPin, motorValue); //motorPin denotes the pin on which the motor is in; motorValue represents the duty cycle of the PWM
 analogWrite(brakePin, brakeValue);
 analogWrite(triggerPin, 10); //help with O-scope reading
 
 Serial.print(" motor = ");
 Serial.print(motorValue); //corresponding value of the motor that runs at the value of PotValue
 Serial.print(" brake = ");
 Serial.print(brakeValue); 

 while(millis()- prevCameraTime < integrationPeriod) {}
 getline();
 measuredMid = findcenter();
 steer(); 

 Serial.println(" ");
}

void getline()
{
  
  digitalWrite(SI, HIGH);
  digitalWrite(CL, HIGH);
  out[0] = analogRead(AO);
  digitalWrite(SI, LOW);
  digitalWrite(CL, LOW);
  Serial.print(" Camera raw: ");
 int i;
  for (i=1;i<128;i++)
  {
    digitalWrite(CL, HIGH);
    out[i] = analogRead(AO);
    
    if (out[i] > threshhold)
      Serial.print(1);
    else
      Serial.print(0); 
    
    digitalWrite(CL, LOW);
    //Serial.print(out[i]);
    //Serial.print(" ");
  }
  
  prevCameraTime = millis();
}


int findcenter()
{
  int leftEdge = 128, rightEdge = 0, center = 64;
  int i=0;
  while (out[i] < threshhold)
  {
    i++;
  }
  leftEdge = i;

  while(out[i] > threshhold)
  {
    i++;
  }
  
  rightEdge = i;
  center = (double) leftEdge / (double) rightEdge;
  Serial.print(" detected center: ");
  Serial.print(center);
  return center; 
}

void steer()
{
// this is for proportional feedback  
/*
  int steerValue;
  steerValue = constrain( map( measuredMid, 128, 0, servoMid - servoRange, servoMid + servoRange ), servoMid - servoRange, servoMid + servoRange );
  myServo.write(steerValue);
  Serial.print(" steerValue: ");
  Serial.print(steerValue);
*/
// without camera and proportional feedback, just use transmitter signal

 servoValue = pulseIn(servoPinIn,HIGH, 25000);
 servoValue = constrain( map( servoValue, 1830, 1160, servoMid - servoRange, servoMid + servoRange ), servoMid - servoRange, servoMid + servoRange );
 myServo.write(servoValue);
 Serial.print(" servoValue: ");
 Serial.print(servoValue);

}

void averageElmenents(int input[], int arraySize, int numsToAverage, int result[])
{
  int sum;
  int elementsUsed; //keep track of how many elements were used for sum
  int average;

  for(int i = 0; i < arraySize; i++)
  {
    sum = 0;
    elementsUsed = 0;
  
    for(int j = 1; j <= numsToAverage; j++)
    {
      //check for valid index going to left of array
      if((i-j) >= 0)
      {
        sum += input[i-j];
        elementsUsed++;  
      }

      //check for valid index going to right of array
      if((i+j) < arraySize)
      {
        sum += input[i+j];
        elementsUsed++;
      }
    }// end of inner loop

    //calculate average
    average = sum / elementsUsed;
    result[i] = average;
    
  } //end of outer loop
  
}


void diff(int input[], int arraySize, int result[])
{
  for(int i = 1; i < arraySize; i++)
    result[i-1] = input[i] - input[i-1];
}

