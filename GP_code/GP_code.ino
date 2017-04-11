// copied from feb5_motors_tx_camera_servo_halleffect on 4/8. Goal was to drive with Tx running single threaded on PCB
// change pin numbers/names

// control modes: TEENSY: no atmega at all.
//        ATMEGA: using atmega for camera and HES

// motor modes: HALF: half bridge. connect neg of motor to MOTOR_NEG_B
//              FULL: full bridge. connect neg of motor to MOTOR_NEG

//First: try to just do single thread and half bridge

#include <Servo.h>

unsigned long int prevLoop; // to track loop time
// MOTORS
//************************************************************************
  int HI = 3; //likewise with motor, arbitrary pin number 
  int LI = 4;
//int triggerPin = 9; // help when looking at O-scope
//int triggerPin2 = 10; // help when looking at O-scope
  int motorValue = 0;
  int brakeValue = 0;
//int buff = 100; // deadzone for pot

  /* reads a pin to get input, has deadzone, and prints
  0-512 is braking, and 513-1023 is forward. There is an adjustable buffer;
  77 = .3 of 256. 180 = .7 of 256 */
  void getPot();

  // transmitter and motor driver
  int REC_MOTOR = 15; // pin to read signals from reciever
  int throttleValue;
  int topSpeed = 256; // = 100%
  int buffTx = 100; // buffer for tx input . full range of tx input is 1160 to 1830
  void getTxMotor(); //gets input from tx, converts to pwm, and prints

// CAMERA AND PROCESSING
//************************************************************************
  int pinSI = 29;
  int pinCL = 30;
  int pinAO = 31;
  
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
  int REC_SERVO = 18;
  int SERVO = 20;
  
  int servoValueMicro; //1000-2000 for full range. is actually less because of the physical stops 
  int servoMidMicro = 1455; // 2/18 test: min 1140, max 1770 before running into mechanical stops
  int servoRangeMicro = 315;
  
  Servo myServo;

// STEERING
//************************************************************************
  // for camera steering
  double xError; // xCenter - xMeasured (from camera);
  double xCenter = 64.0;
  double xMeasured;  
  double kSteering = .546875; // 35/64 = .546875 for full range. will be adjusted later.
  double kSteeringMicro =  (double)(servoRangeMicro)/64.00;
     
  void steerCamera(double xCenter, double xMeasured); // steers using camera input 
  // for Tx steering
  void steerTx(); // steers using Tx input

// SPEED SENSING
//************************************************************************
  double wheelDiameter = 1.8; //inches
  int hallPin_L = 1;//left
  int hallValue_L;
  double wheelSpeed_L;
  double wheelSpeed_L_Copy; //copy to prevent memory issues
  volatile unsigned long int prevHallTime_L; // volatile to prevent memory issues
  void magnet_detect_L();
  
  int hallPin_R = 2;//right
  int hallValue_R;
  double wheelSpeed_R;
  double wheelSpeed_R_Copy; //copy to prevent memory issues
  volatile unsigned long int prevHallTime_R; // volatile to prevent memory issues
  void magnet_detect_R();
  

// MAIN CODE
//************************************************************************
void setup() {
  Serial.begin(9600); //setting speed of communication in bits/second
  
  //Motor things  
    //pinMode(potentioPin,INPUT); //turn on ADC at pin; ready to read voltages
    pinMode(HI,OUTPUT);
    pinMode(LI,OUTPUT);
    //pinMode(triggerPin,OUTPUT);
    //analogWriteFrequency(HI, 9800); appears to be unnecessary
    //analogWriteFrequency(LI, 9800);
    
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
  
  //indicator LED that program is running
    pinMode(13, OUTPUT);
    digitalWrite(13,HIGH);
}

void loop() {

  //Getting motor inputs. pot or Tx
    //getPot();
    getTxMotor();
  
  //Write to motor
     analogWrite(HI, motorValue); //HI denotes the pin on which the motor is in; motorValue represents the duty cycle of the PWM
     analogWrite(LI, brakeValue);
     //analogWrite(triggerPin, 10); //help with O-scope reading 
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
    //steerCamera(xCenter,xMeasured);

  //Speed Sensing
    Serial.print(" Speed data: ");
    Serial.print(prevHallTime_L);
    Serial.print(" ");
    Serial.print(millis());
    Serial.print(" ");
  
    noInterrupts(); // to prevent memory issues
    wheelSpeed_L_Copy = wheelSpeed_L;
    interrupts();
    
    hallValue_L = analogRead(hallPin_L);
    Serial.print(" hallPin_L: ");
    Serial.print(hallValue_L);
    Serial.print(" ");
    Serial.print(" wheelSpeed_L: ");
    Serial.print(wheelSpeed_L_Copy);
  
  //Print loop time
    Serial.print(" Looptime: ");    
    Serial.println(millis()-prevLoop);
    prevLoop = millis();
}

// FUNCTION DEFINITIONS
//************************************************************************
void getline(int lineBuffer[])
{ 
  //digitalWrite(triggerPin2, HIGH);
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
    Serial.print(out[i]);
    Serial.print(" ");
  }
  //digitalWrite(triggerPin2, LOW);
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
/*
void getPot()
{
     potentioValue = analogRead(potentioPin); //value between 0 and 1023 
     motorValue = constrain(map(potentioValue, 512+buff, 1023, 0, 256),0,256);
     brakeValue = constrain(map(potentioValue, 511-buff, 0, 0, 256),0,256);
     Serial.print("potentiometer = " );     
     Serial.print(potentioValue); //integer between 0 and 1023 
}
*/
void getTxMotor()
{
     throttleValue = pulseIn(REC_MOTOR, HIGH, 25000);
     if ( throttleValue < 1000 && throttleValue > 2000)
     {
      throttleValue = 1495;
     }
     motorValue = constrain(map(throttleValue, 1495+buffTx,1830, 0, topSpeed),0,topSpeed);
     brakeValue = constrain(map(throttleValue, 1495-buffTx,1160, 0, topSpeed),0,topSpeed);
     Serial.print("transmitter (1160 to 1830) = " );     
     Serial.print(throttleValue); //integer between 1160 and 1830
}

void steerTx() //needs checking
{
 //get data from Tx 
 servoValueMicro = pulseIn(REC_SERVO,HIGH, 25000);
 //servoValue = map(servoValue,1350,1730,1000,2000); was used for calibration
 //map to servo steering range
   // 0-180
      //servoValue = constrain( map( servoValue, 1830, 1160, servoMid - servoRange, servoMid + servoRange ), servoMid - servoRange, servoMid + servoRange );
      //myServo.write(servoValue); 
   //1000-2000
      //servoValue = constrain(map( servoValue,1830,1160,servoMidMicro - servoRangeMicro, servoMidMicro + servoRangeMicro), servoMidMicro - servoRangeMicro, servoMidMicro + servoRangeMicro );
 myServo.writeMicroseconds(constrain(servoValueMicro,servoMidMicro-servoRangeMicro,servoMidMicro+servoRangeMicro));
 Serial.print(" servoValueMicro: ");
 Serial.print(servoValueMicro);
}

void steerCamera(double xCenter, double xMeasured)
{
  double xError;
  double steerValue;
  xError = xCenter - xMeasured;
  //0-180
    //steerValue = servoMid + kSteering*xError; // for full range: 64 pixels --> 35 ms has kSteering = 35/64 = .546875
    //myServo.write(steerValue);
  //1000-2000
    steerValue = servoMidMicro + kSteeringMicro*xError;
    steerValue = constrain(steerValue,servoMidMicro-servoRangeMicro, servoMidMicro+servoRangeMicro);
    myServo.writeMicroseconds(steerValue);
  //does fullrange camera to fullrange servo
  //steerValue = constrain( map( xMeasured, 128, 0, servoMid - servoRange, servoMid + servoRange ), servoMid - servoRange, servoMid + servoRange );
  
  
  Serial.print(" steerValue: ");
  Serial.print(steerValue);
}

void magnet_detect_L()
{
  wheelSpeed_L = (wheelDiameter*PI/12)/((millis()-prevHallTime_L)/1000.0); // converts to ft/sec
  prevHallTime_L = millis();
}

void magnet_detect_R()
{
  wheelSpeed_R = (wheelDiameter*PI/12)/((millis()-prevHallTime_R)/1000.0); // converts to ft/sec
  prevHallTime_R = millis();
}
