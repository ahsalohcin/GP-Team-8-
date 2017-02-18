#include <Servo.h>

unsigned long int prevLoop;
// MOTORS
//************************************************************************
  int potentioPin = 14; //integer variable denoting the pin with the potentiometer, gets assigned a value from 0-1023 for 0-5
  int motorPin = 3; //likewise with motor, arbitrary pin number 
  int brakePin = 4;
  int triggerPin = 9; // help when looking at O-scope
  int triggerPin2 = 10; // help when looking at O-scope
  int potentioValue = 0; //will be an integer between 0 and 1023
  int motorValue = 0;
  int brakeValue = 0;
  int buff = 100; // deadzone for pot

  /* reads a pin to get input, has deadzone, and prints
  0-512 is braking, and 513-1023 is forward. There is an adjustable buffer;
  77 = .3 of 256. 180 = .7 of 256 */
  void getPot();

  // transmitter and motor driver
  int pinTx = 15; // pin to read signals from reciever
  int throttleValue;
  int topSpeed = 256; // = 100%
  int buffTx = 100; // buffer for tx input . full range of tx input is 1160 to 1830
  void getTxMotor(); //gets input from tx, converts to pwm, and prints

// CAMERA AND PROCESSING
//************************************************************************
  int pinSI = 5;
  int pinCL = 6;
  int pinAO = 17;
  
  int out[128]; //straight from camera;
  double averaged[128]; //after moving average filter
  double differences[127];//after taking differences
  
  int threshhold = 350; // for thershholding technique
  unsigned int integrationPeriod = 50;
  unsigned int prevCameraTime = 0;
  
  //gets one line of camera data
  void getline(int lineBuffer[]);
  
  //Calculates average of numsToAverage points around all elements in input
  // and stores each averaged point in result
  // input and result should be the same size.
  // input[]: Data to manipulate
  // arraySize: Size of input array
  // numsToAverage: Number of elements around point p to use to calculate the average
  // result[]: Array to return the results
  void averageElements(int input[], int arraySize, int numsToAverage, double result[]);
  
  //Calcualtes difference between adjencents elements of input and stores in result
  //result's size will be arraySize-1
  void diff(double input[], int arraySize, int result);
  
  //Finds max and min of input data, calculates center point bewtween max and min,
  //and returns the index of the calculated center point
  double center(double input[], int arraySize);

// SERVO
//************************************************************************
  int servoPinIn = 18;
  int servoPinOut = 20;
  
  int servoValue; //0-180 for full range. is actually less because of the physical stops 
  int servoMid = 82;
  int servoRange = 35;
  
  int servoValueMicro; //1000-2000 for full range. is actually less because of the physical stops 
  int servoMidMicro = 1455; // 2/18 test: min 1140, max 1770 before running into mechanical stops
  int servoRangeMicro = 315;
  
  Servo myServo;

// STEERING
//************************************************************************
  double xError; // xCenter - xMeasured (from camera);
  double xCenter = 64.0;
  double xMeasured;  
  double kSteering = .546875; // 35/64 = .546875 for full range. will be adjusted later.
  double kSteeringMicro =  (double)(servoRangeMicro)/64.00;
     
  void steerCamera(double xCenter, double xMeasured); // steers using camera input 
  void steerTx(); // steers using Tx input

// SPEED SENSING
//************************************************************************

  int hallPin = 16;
  int hallValue;
  double wheelDiameter = 1.8; //inches
  double wheelSpeed;
  double wheelSpeedCopy; //copy to prevent memory issues
  volatile unsigned long int prevHallTime; // volatile to prevent memory issues
  
  void magnet_detect();

// MAIN CODE
//************************************************************************
void setup() {
  Serial.begin(9600); //setting speed of communication in bits/second
  
  //Motor things  
    pinMode(potentioPin,INPUT); //turn on ADC at pin; ready to read voltages
    pinMode(motorPin,OUTPUT);
    pinMode(brakePin,OUTPUT);
    pinMode(triggerPin,OUTPUT);
    //analogWriteFrequency(motorPin, 9800);
    //analogWriteFrequency(brakePin, 9800);
    
  //for transmitter throttle
    pinMode(pinTx,INPUT);
  
  //camera stuff
    pinMode(pinSI,OUTPUT);
    pinMode(pinCL,OUTPUT);
    pinMode(pinAO, INPUT);
  
  //from transmitter to control the servo
    pinMode(servoPinIn,INPUT);
    myServo.attach(servoPinOut);

  //speed sensing
    pinMode(hallPin,INPUT);
    attachInterrupt(digitalPinToInterrupt(hallPin), magnet_detect, RISING); //set up interrupt function. should work on any teensy pin.
  
  //indicator LED that program is running
    pinMode(13, OUTPUT);
    digitalWrite(13,HIGH);
}

void loop() {

  //Getting motor inputs. pot or Tx
    //getPot();
    getTxMotor();
  
  //Write to motor
     analogWrite(motorPin, motorValue); //motorPin denotes the pin on which the motor is in; motorValue represents the duty cycle of the PWM
     analogWrite(brakePin, brakeValue);
     analogWrite(triggerPin, 10); //help with O-scope reading 
     Serial.print(" motor = ");
     Serial.print(motorValue); //corresponding value of the motor that runs at the value of PotValue
     Serial.print(" brake = ");
     Serial.print(brakeValue); 
  
  //Get a line of camera data
    while(millis()- prevCameraTime < integrationPeriod) {}
    getline(out);
    //print it
    /*
    Serial.print("camera raw: ");
    
    for(int i=0; i<128; i++)  {
      
      Serial.print(out[i]);
      Serial.print(",");
      
      }
*/
     
  //Determine the center
    averageElements(out,128,3,averaged);   
    diff(averaged,128,differences);
    xMeasured = center(differences,127);
    Serial.print(" xMeasured: ");
    Serial.print(xMeasured);
    
  //Get Tx steering input and writes to servo
    steerTx();
  
  //Proportional controller write to servo instead
    steerCamera(xCenter,xMeasured);

  //Speed Sensing
    Serial.print(" Speed data: ");
    Serial.print(prevHallTime);
    Serial.print(" ");
    Serial.print(millis());
    Serial.print(" ");
  
    noInterrupts(); // to prevent memory issues
    wheelSpeedCopy = wheelSpeed;
    interrupts();
    
    hallValue = analogRead(hallPin);
    Serial.print(" hallPin: ");
    Serial.print(hallValue);
    Serial.print(" ");
    Serial.print(" wheelSpeed: ");
    Serial.print(wheelSpeedCopy);
  
  //Print loop time    
    Serial.println(millis()-prevLoop);
    prevLoop = millis();
}

// FUNCTION DEFINITIONS
//************************************************************************
void getline(int lineBuffer[])
{ 
  digitalWrite(triggerPin2, HIGH);
  digitalWrite(pinSI, HIGH);
  digitalWrite(pinCL, HIGH);
  lineBuffer[0] = analogRead(pinAO);
  digitalWrite(pinSI, LOW);
  digitalWrite(pinCL, LOW);
 int i;
  for (i=1;i<128;i++)
  {
    digitalWrite(pinCL, HIGH);
    lineBuffer[i] = analogRead(pinAO);
   digitalWrite(pinCL, LOW);
    //Serial.print(out[i]);
    //Serial.print(" ");
  }
  digitalWrite(triggerPin2, LOW);
  prevCameraTime = millis();
}

void averageElements(int input[], int arraySize, int numsToAverage, double result[])
{
  double sum;
  int elementsUsed; //keep track of how many elements were used for sum
  double average;

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

void diff(double input[], int arraySize, double result[])
{
  for(int i = 1; i < arraySize; i++)
    result[i-1] = input[i] - input[i-1];
}

double center(double input[], int arraySize)
{
  //Index of max and min
  int maxIndex = 0;
  int minIndex = 0;
  int maxValue = 0;
  int minValue = 1000.0;  

  //Find max and min
  for(int i = 0; i < arraySize; i++)
  {
    if(input[i] < minValue)
    {
      minValue = input[i];
      minIndex = i;
    }
    else if(input[i] > maxValue)
    {
      maxValue = input[i];
      maxIndex = i;
    }
  }

  return (minIndex + maxIndex) / 2.0;
}

void getPot()
{
     potentioValue = analogRead(potentioPin); //value between 0 and 1023 
     motorValue = constrain(map(potentioValue, 512+buff, 1023, 0, 256),0,256);
     brakeValue = constrain(map(potentioValue, 511-buff, 0, 0, 256),0,256);
     Serial.print("potentiometer = " );     
     Serial.print(potentioValue); //integer between 0 and 1023 
}

void getTxMotor()
{
     throttleValue = pulseIn(pinTx, HIGH, 25000);
     motorValue = constrain(map(throttleValue, 1495+buffTx,1830, 0, topSpeed),0,topSpeed);
     brakeValue = constrain(map(throttleValue, 1495-buffTx,1160, 0, topSpeed),0,topSpeed);
     Serial.print("transmitter (1160 to 1830) = " );     
     Serial.print(throttleValue); //integer between 1160 and 1830
}

void steerTx()
{
 //get data from Tx 
 servoValue = pulseIn(servoPinIn,HIGH, 25000);
 //servoValue = map(servoValue,1350,1730,1000,2000); was used for calibration
 //map to servo steering range
   // 0-180
      //servoValue = constrain( map( servoValue, 1830, 1160, servoMid - servoRange, servoMid + servoRange ), servoMid - servoRange, servoMid + servoRange );
      //myServo.write(servoValue); 
   //1000-2000
      //servoValue = constrain(map( servoValue,1830,1160,servoMidMicro - servoRangeMicro, servoMidMicro + servoRangeMicro), servoMidMicro - servoRangeMicro, servoMidMicro + servoRangeMicro );
 myServo.writeMicroseconds(servoValue);
 Serial.print(" servoValue: ");
 Serial.print(servoValue);
}

void steerCamera(double xCenter, double xMeasured)
{
  double xError;
  double steerValue;
  xError = xCenter - xMeasured;
  //0-180
    steerValue = servoMid + kSteering*xError; // for full range: 64 pixels --> 35 ms has kSteering = 35/64 = .546875
    myServo.write(steerValue);
  //1000-2000
    steerValue = servoMidMicro + kSteeringMicro*xError;
    steerValue = constrain(steerValue,servoMidMicro-servoRangeMicro, servoMidMicro+servoRangeMicro);
    myServo.writeMicroseconds(steerValue);
  //does fullrange camera to fullrange servo
  //steerValue = constrain( map( xMeasured, 128, 0, servoMid - servoRange, servoMid + servoRange ), servoMid - servoRange, servoMid + servoRange );
  
  
  Serial.print(" steerValue: ");
  Serial.print(steerValue);
}

void magnet_detect()
{
  wheelSpeed = (wheelDiameter*PI/12)/((millis()-prevHallTime)/1000.0); // converts to ft/sec
  prevHallTime = millis();
}
