// copied from feb5_motors_tx_camera_servo_halleffect on 4/8. Goal was to drive with Tx running single threaded on PCB
// change pin numbers/names

// 4/24  significant changes to state flow and modes and changed topspeed pause condition:
// use enum types for state and modes. stateCheck() is modified to return "state" type, and it has multiple return statements.any paused condition will return pause. 
// topspeed pause condition is now same as topspeed for Tx throttle control. Maybe set it to something else? Probably limited our performance on saturday 4/22. 
// added throttle mode using bluetooth. 


// currently OL Tx throttle is linear + deadzone in input, linear in output 
// CL is Tx throttle input linear with no deadzone, but with deadzone in output. 
// CL preferred: Tx throttle input linear with deadzone. very little deadzone in output.

#include <Servo.h>
#include <SoftwareSerial.h>

unsigned long int prevLoop; // to track loop time

// useful function
double mapdouble(double x, double in_min, double in_max, double out_min, double out_max)
{
 return (double)(x - in_min) * (out_max - out_min) / (double)(in_max - in_min) + out_min;
}

// BLUETOOTH MODULE COMMUNICATION
//***********************************************************************
#define BTSERIAL Serial3
#define MESSAGE_BEGIN 29
#define MESSAGE_END 30
#define MAX_MESSAGE_SIZE 100

char messageBuffer[MAX_MESSAGE_SIZE]; // bluetooth message content
int bufferPos = 0; //keeps track of last char in messageBuffer
int btPeriod = 100; // ms
unsigned long int prevBtTime = 0;

//Check if complete bluetooth packet was received
bool packetAvailable();

//Parse bt packet into name and value
void packetParse();

// Executes bluetooth command 
void exec_packet_cmd(char *cmd_name, char* cmd_value);

// Sends telemetry data through bluetooth back to computer monitoring
void telemetry();

// STATE FLOW
//************************************************************************
typedef enum {
  PLAY,
  PAUSE  
} state;
state myState = PLAY;

// for RC controlled state. pause when right stick all the way up
int stateValue;
int REC_STATE = 22;

int SW1 = 24; 
int SW2 = 25;
int SW3 = 26;
int SW4 = 27;

state stateCheck();

// DRIVE MODES
//************************************************************************
typedef enum {
  RC_TH,
  BT_TH,
  AUTO_TH,
} throttleMode;

throttleMode myThrottleMode = RC_TH;

typedef enum {
  RC_ST,
  AUTO_ST,
} steerMode;

steerMode mySteerMode = RC_ST;

// MOTORS
//************************************************************************
  int HI = 3; //pin numbers
  int LI = 4;
  
  int motorValue = 0;
  int brakeValue = 0;

  // allow Tx input fomr channel 3 (left stick vertical)
  int REC_MOTOR = 21; // pin to read signals from reciever
  int throttleValue;
  int topDutyCycleOL = 256*.5; //max speed
  int buffTxIn = 10; // buffer for tx input . full range of tx input is 1160 to 1830
  int throttleMid = 1500;
  void getSpeedOL(); //gets input from tx, assigns it to a desired speed, determines what to give HI and LI. 

  // closed loop speed control
  double vRef;
  double vMeas;
  double vError;
  double vValue; 
  double topSpeed = 5; // in ft/sec
  int buffTxOut = 5; // for safety. units of us. (tested)
  int buffOut = 1; // for safety. larger will mess up linearity . may be in units of speed (ft/sec).(untested)
  int topDutyCycleCL = 256*.5; // max speed 
  double kpSpeed = 1.0;
  double kiSpeed = 0.005;
  double vErrorTotal = 0.0;
  void getSpeedCL();

// CAMERA AND PROCESSING
//************************************************************************
  int pinSI = 29;
  int pinCL = 30;
  int pinAO = 31;
  
  int out[128]; //straight from camera;
  double averaged[128]; //after moving average filter
  double differences[127];//after taking differences
  
  int threshhold = 350; // for threshholding technique
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
  int REC_SERVO = 20;
  int SERVO = 9;
  
  int servoValueMicro; //1000-2000 for full range. is actually less because of the physical stops 
  int servoMidMicro = 1455; // 2/18 test: min 1140, max 1770 before running into mechanical stops
  int servoRangeMicro = 315;
  
  Servo myServo;

// STEERING
//************************************************************************
  // for camera steering
  double xError; // xRef - xMeasured (from camera);
  double xRef = 64.0; // center is 64;
  double xMeasured;  
  double kSteering = .546875; // 35/64 = .546875 for full range. will be adjusted later.
  double kSteeringMicro =  (double)(servoRangeMicro)/64.00;
     
  void steerCamera(double xRef, double xMeasured); // steers using camera input 
  
  // for Tx steering
  void steerTx(); // steers using Tx input

// SPEED SENSING
//************************************************************************

  unsigned int hallTimeout = 500; // in millisec
  int speedLimit = topSpeed; //higher than this will pause it. 
  double wheelDiameter = 1.8; //inches
  
  int hallPin_L = 1;//left
  int hallValue_L;
  double wheelSpeed_L;
  double wheelSpeed_L_Copy = 0; //copy to prevent memory issues
  volatile unsigned long int prevHallTime_L; // volatile to prevent memory issues
  void magnet_detect_L();
  
  int hallPin_R = 2;//right
  int hallValue_R;
  double wheelSpeed_R;
  double wheelSpeed_R_Copy=0; //copy to prevent memory issues
  volatile unsigned long int prevHallTime_R; // volatile to prevent memory issues
  void magnet_detect_R();
  
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
  
// MAIN CODE
//************************************************************************
void setup() {
  Serial.begin(9600); //setting speed of communication in bits/second

  //bluetooth serial
  BTSERIAL.begin(9600);

  //state flow
    pinMode(REC_STATE,INPUT);
    pinMode(SW1,INPUT_PULLUP);
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
    //openloop
    //getSpeedOL();
    //closedloop
    getSpeedCL();
  
  //Write to motor
     analogWrite(HI, motorValue); //HI denotes the pin on which the motor is in; motorValue represents the duty cycle of the PWM
     analogWrite(LI, brakeValue);
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
    
  if (mySteerMode == RC_ST)  
    {//Get Tx steering input and writes to servo
    steerTx();
    }
  else 
    {//Camera feedback to write to servo instead
    steerCamera(xRef,xMeasured);
    }
    
  //Speed Sensing
    Serial.print(" Speed data: ");
    /*
    Serial.print(prevHallTime_R);
    Serial.print(" ");
    Serial.print(millis());
    Serial.print(" ");
    */
    if ( (millis()-prevHallTime_R) > hallTimeout )
    wheelSpeed_R = 0;
    
    noInterrupts(); // to prevent memory issues
    wheelSpeed_R_Copy = wheelSpeed_R;
    interrupts();
    
    hallValue_R = analogRead(hallPin_R);
    //Serial.print(" hallPin_R: ");
    //Serial.print(hallValue_R);
    //Serial.print(" ");
    Serial.print(" wheelSpeed_R: ");
    Serial.print(wheelSpeed_R_Copy);
  //Diagnostics
    //batt voltage runs once in a while
    if ( millis() - battPrevTime > battPeriod)
    {
      getBattVoltVal();
    }

    Serial.print(" Batt: ");
    Serial.print(battVoltVal);
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
    //reading motor current
    iSenseV = mapdouble(analogRead(I_SENSE),0,1023,0,3.3);
    iCalc = iConst * iSenseV;
    Serial.print(" iSenseV: ");
    Serial.print(iSenseV);
    Serial.print(" iCalc: ");
    Serial.print(iCalc); 

   if (millis()-prevBtTime >btPeriod)
   {
   telemetry();
   prevBtTime = millis();
   }
  //Print loop time
    Serial.print(" Looptime: ");    
    Serial.println(millis()-prevLoop);
    prevLoop = millis();
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

// FUNCTION DEFINITIONS
//************************************************************************


bool packetAvailable()
{
  while(BTSERIAL.available() > 0)
    {
      char incomingByte = BTSERIAL.read();

      if(incomingByte == MESSAGE_BEGIN)
      {
        //clear buffer for new incoming packet
        bufferPos = 0;
      }
      else if(incomingByte == MESSAGE_END)
      {
        //full packet received
        messageBuffer[bufferPos] = '\0';
        return true;
      }
      else
      {
        messageBuffer[bufferPos] = incomingByte;
        bufferPos++;
      }
         
    }

    return false;
}

void packetParse()
{  
  char packet_name[40];
  char packet_value[40] = {0};
  long int value = 0;
  
  //get divider position
  char *separator = strchr(messageBuffer, ':');
  *separator = '\0';

  //get command name
  strcpy(packet_name, messageBuffer);

  //skip white space
  *separator = ':';
  separator += 2;

  //get value
  for(int i = 0; *separator != '\0'; i++, separator++)
    packet_value[i] = *separator;

  exec_packet_cmd(packet_name, packet_value);

  BTSERIAL.write("\b");
  BTSERIAL.write(messageBuffer);
  BTSERIAL.write("\n");
}

void exec_packet_cmd(char *cmd_name, char* cmd_value)
{

  char chg_state[] = "change_state";
  char change_kpSpeed[] = "kpSpeed";
  char change_kiSpeed[] = "kiSpeed";

  if(strcmp(chg_state, cmd_name) == 0)
  {
      long int value = strtol(cmd_value, NULL, 10);

      if(value == 1)
        myState = PLAY;
      else
        myState = PAUSE;
  }
  else if(strcmp(change_kpSpeed, cmd_name) == 0)
  {
    double value = strtod(cmd_value, NULL);

    kpSpeed = value;
  }
  else if(strcmp(change_kiSpeed, cmd_name) == 0)
  {
    double value = strtod(cmd_value, NULL);

    kiSpeed = value;
  }
  
/*  
  Serial.write("name: ");
  Serial.println(cmd_name);

  Serial.write("value: ");
  Serial.println(cmd_value);
*/
}

void telemetry()
{
  /*
  Serial.print(" motor = ");
  Serial.print(motorValue); //corresponding value of the motor that runs at the value of PotValue
  Serial.print(" brake = ");
  Serial.print(brakeValue); 

  Serial.print("transmitter (1160 to 1830) = " );     
  Serial.print(throttleValue); //integer between 1160 and 1830

  Serial.print(" servoValueMicro: ");
  Serial.print(servoValueMicro);

  Serial.print(" steerValue: ");
  Serial.print(steerValue);

  */

  
  //String strdata = String("mtr:" + String(motorValue) + ";brk:" + String(brakeValue) + ";trst:" + String(throttleValue) + ";srv:" + String(servoValueMicro) + ";spd:" + String(wheelSpeed_R_Copy));
  String strdata = String( "vRef:" +String(vRef) + ";vError:" + String(vError));
  //Serial.println("Telemetry data: ");
  //Serial.println(strdata.c_str());

  //char data[strdata.length() + 1];
  BTSERIAL.write("\b");
  BTSERIAL.write(strdata.c_str());
  BTSERIAL.write("\n");
  //BTSERIAL.write("\b");
  //BTSERIAL.write("A");
  //BTSERIAL.write("\n");  
}

state stateCheck()
{
  //Serial.println("checking state");
  // If pause switch is on
  if (digitalRead(SW1) == LOW) 
    {
      myState = PAUSE;
      Serial.print(digitalRead(SW1));
      Serial.println("pause switch");
      return myState;
    }
  
  //If Tx says to pause
  stateValue = pulseIn(REC_STATE, HIGH, 25000);
  if (stateValue > 1550 && stateValue < 2000 ) // channel is all the way high
    {myState = PAUSE;
    Serial.print(" stateValueTx: ");
    Serial.println(stateValue);
    return myState;}
    
  //Check for bluetooth message that might change state

  /*
  if(packetAvailable())
     packetParse();
  if (myState == PAUSE)
     {return myState;}   
  */
  // low Batt Warning
  
  if ( getBattVoltVal() < lowBattWarning )
  {
    if (millis() > 2000)
    {
      Serial.println("batt volts error");
      myState = PAUSE;
      return myState;
    }
  }
  //exceeding top speed 

  
  noInterrupts();
  wheelSpeed_R_Copy = wheelSpeed_R;
  interrupts();
    
  if (wheelSpeed_R_Copy > speedLimit )
    {myState = PAUSE;
    return myState;}
 
  myState = PLAY;
  return myState;
}

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
    //Serial.print(out[i]);
    //Serial.print(" ");
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
void getSpeedOL()
{
     throttleValue = pulseIn(REC_MOTOR, HIGH, 25000); //throttleValue is speed setpoint open loop
     if ( throttleValue < 1000 || throttleValue > 2000)
     {
      throttleValue = throttleMid;
     }

     motorValue = constrain(map(throttleValue, throttleMid+buffTxIn,1850, 0, topDutyCycleOL),0,topDutyCycleOL);
     brakeValue = constrain(map(throttleValue, throttleMid-buffTxIn,1150, 0, topDutyCycleOL),0,topDutyCycleOL);
     Serial.print("transmitter (1150 to 1850) = " );     
     Serial.print(throttleValue); //integer between 1150 and 1850
}

// needs a lot of work

void getSpeedCL()
{
  throttleValue = pulseIn(REC_MOTOR, HIGH, 25000); // throttleValue is a Speed setpoint used for closed loop
  if ( throttleValue < 1000 || throttleValue > 2000)
  {
    throttleValue = throttleMid;
  }
  Serial.print(" throttleValue: ");
  Serial.print(throttleValue);

  if (throttleValue >=  throttleMid)
  {
   vRef = constrain(mapdouble(throttleValue, throttleMid+buffTxIn, 1850, 0,topSpeed),0,topSpeed); 
  }
  else
  {
   vRef = constrain(mapdouble(throttleValue,throttleMid-buffTxIn, 1150, 0,-topSpeed),-topSpeed,0);
  }
    
  vMeas = wheelSpeed_R_Copy;
  vError = vRef-vMeas; // in ft/sec
  vErrorTotal += vError;
  Serial.print(" vRef: ");
  Serial.print(vRef);
  //Serial.print(" vError: ");
  //Serial.print(vError);

  //throttleValue = throttleValue + kpSpeed*vError + kiSpeed*vErrorTotal; //kSpeed must be in us /(ft/sec). before they were both .5 
  //throttleValue = constrain(mapdouble(vError,-topSpeed,topSpeed,1000,2000),1000,2000); //convert back to microseconds
  
  //motorValue = constrain(map(throttleValue, throttleMid+buffTxOut,1850, 0, topDutyCycleCL),0,topDutyCycleCL);
  //brakeValue = constrain(map(throttleValue, throttleMid-buffTxOut,1150, 0, topDutyCycleCL),0,topDutyCycleCL);
  
  //Serial.print(" throttleValue (1150 to 1850) = " );     
  //Serial.print(throttleValue); //integer between 1150 and 1850,

  vValue = vRef+ kpSpeed*vError + kiSpeed*vErrorTotal; 
  Serial.print(" vValue: ");
  Serial.print(vValue);
  motorValue = constrain(mapdouble(vValue,0+buffOut,10,0,topDutyCycleCL),0,topDutyCycleCL); //should topSpeed be in here? or should it be a constant?
  brakeValue = constrain(mapdouble(vValue,0-buffOut,-10,0,topDutyCycleCL),0,topDutyCycleCL);
  
}

void steerTx() //needs checking
{
 //get data from Tx 
 servoValueMicro = pulseIn(REC_SERVO,HIGH, 25000);

  if ( servoValueMicro < 1000 || servoValueMicro > 2000)
  {
    servoValueMicro = servoMidMicro;
  }
  
 //servoValue = map(servoValue,1350,1730,1000,2000); was used for calibration
 //map to servo steering range
   // 0-180
      //servoValue = constrain( map( servoValue, 1830, 1160, servoMid - servoRange, servoMid + servoRange ), servoMid - servoRange, servoMid + servoRange );
      //myServo.write(servoValue); 
   //1000-2000
      //servoValue = constrain(map( servoValue,1830,1160,servoMidMicro - servoRangeMicro, servoMidMicro + servoRangeMicro), servoMidMicro - servoRangeMicro, servoMidMicro + servoRangeMicro );
 servoValueMicro = constrain(servoValueMicro,servoMidMicro-servoRangeMicro,servoMidMicro+servoRangeMicro);
 servoValueMicro = map(servoValueMicro, servoMidMicro-servoRangeMicro,servoMidMicro+servoRangeMicro,servoMidMicro+servoRangeMicro,servoMidMicro-servoRangeMicro);
 myServo.writeMicroseconds(servoValueMicro);
 Serial.print(" servoValueMicro: ");
 Serial.print(servoValueMicro);
}

void steerCamera(double xRef, double xMeasured)
{
  double xError;
  double steerValue;
  xError = xRef - xMeasured;
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
  wheelSpeed_L = (wheelDiameter*PI/12/3)/((millis()-prevHallTime_L)/1000.0); // converts to ft/sec
  Serial.println("magnet_detect_L");
}

void magnet_detect_R()
{
  wheelSpeed_R = (wheelDiameter*PI/12/3)/((millis()-prevHallTime_R)/1000.0); // converts to ft/sec
  prevHallTime_R = millis();
}

double getBattVoltVal(){
      battVoltSenseVal = analogRead(BATT_VOLTAGE_SENSE);
      battVoltVal = battVoltSenseVal*battConst/1024.0*3.3;
      battPrevTime = millis();
      return battVoltVal;
} 

