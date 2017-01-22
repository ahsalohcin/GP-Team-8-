#include <Servo.h>
#include <math.h>
Servo servo;
/*
wheelbase=8 in 
lookahead rear axle to point 
5 down:

5 25"
4 17"
3 12"
*/

// camera
int SI = 3;
int CL = 4;
int AO = A1;
int out[130];
int med; //detected line center out of 128
int filtered[128];
int maxI, minI;
double mean;
int looptime;
int max_loop=10;


//servo and steering constants
int servoPin=A0;
int mid = 85; // center according to servo
double EL=8;
double el_d=20;
//double el_d=25;
//double el_d=32;
double err_el_d=0;
double curv_max=1/36;
double pi=3.14159;
double alpha=0;
double alphamap=0;
double K_alpha=3;

// speed reading
int pinSpd = A5;
int topspeed=40;
int turnspeed=10;
int spd[5];
int avgSpd;
int tempSpd;

//error tracking
int error_array[20];
int avgError;
int offtrack_error;
double sum_deviation;
int cc = 0;


//diagnostics
int pinkP = A2, pinkD = A3, pinkI = A4;
int pinTX = A6;
int th = 50 ;

//motor control
int ctrlSpd=20;
int high_side = 21;
int low_side = 22;

double error = 0, prevError = 0;
double integral = 0;
double derivative = 0;
double kP = 1.2, kI=0, kD=0; 
double dt = 0;
long prev_time;


int i;

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int median( int in[])
// returns median of array in. in needs to be an odd number of elements
{
  int n =sizeof(in)/sizeof(int);
  int i,j,temp;
  for (i=0;i<n-1;i++)
      {
        for(j=1;j<n;j++)
        {
          if (in[j]<in[i])
          {
            temp=in[i];
            in[i]=in[j];
            in[j]=temp;
          }
        }
      }
  return in[n/2];
}
void medianArray(int num, int in[])
// replaces i'th element of array "in" with median of that element with "num" elements to the left and right of i. doesn't change first or last few
{
  int i,j,temp[2*num+1];
  for (i=0;i<num;i++)
  {
    filtered[i]=in[i];
  }
  for (i=127;i>127-num;i--)
  {
    filtered[i]=in[i];
  }
  for (i=num;i<128-num;i++)
  {
    for (j=0;j<2*num+1;j++)
    {
      temp[j]=in[i+j];
    }
    filtered[i]=median(temp);
  }
  return;
}

int findAverage() {
  int grad[128];
  int i;
  int biggest=0;
  int smallest=2000;
  int average;
  //apply gradient filter
  for(i=1;i<128;i++)
          {
                grad[i-1] = filtered[i]-filtered[i-1];
          }

  grad[127] = 0;
  //find the biggest number
  maxI = 0;
  minI = 125;
  for(i=0;i<125;i++)
          {
                if(grad[i]>biggest)
                         {
                                 biggest = grad[i];
                                 maxI = i;
                         }
          }
  //find the smallest number
  for(i=0;i<125;i++)
          {
                if(grad[i]<smallest)
                         {
                                 smallest = grad[i];
                                 minI = i;
                         }
          }
   /*       
  for (i=0; i<128; i++)
    if (i >= maxI && i <= minI)
      Serial.print(1);
    else
      Serial.print(0);
  Serial.print("max:");
  Serial.print(maxI);
  Serial.print(" min:");
  Serial.print(minI);
  */
  average = (maxI + minI)/2;
  //Serial.print("Average: ");
  //Serial.print(average);
  return average;

}


void getLine() {
  
  digitalWrite(SI, HIGH);
  digitalWrite(CL, HIGH);
  out[0] = analogRead(AO);
  digitalWrite(SI, LOW);
  digitalWrite(CL, LOW);
  
 
  for (i=1;i<128;i++)
  {
    digitalWrite(CL, HIGH);
    out[i] = analogRead(AO);
//    if (out[i] > th)
//      Serial.print(1);
//    else
//      Serial.print(0);  
    digitalWrite(CL, LOW);
  }
  medianArray(2, out);
  /*
  for (i = 0; i<126; i++) {
    if (i == 40 || i == 80 || i == 120) Serial.println();
    Serial.print(out[i]);
    Serial.print(' ');    
  } 
  */
  med = findAverage();
 // Serial.print("Center:");
  //Serial.print(med);
  mean = 0;
  for (i = maxI; i<=minI; i++)
    mean += filtered[i];
  sum_deviation = 0;
  mean = mean / (minI - maxI);
  for (i = maxI; i<=minI; i++)
    sum_deviation += (filtered[i]-mean)*(filtered[i]-mean);
  //Serial.print("STD: ");
  //Serial.print(sqrt(sum_deviation/(minI - maxI)));
  if (sqrt(sum_deviation/(minI - maxI)) < 20.0)
    cc ++;
  else
    cc = 0;
  if (cc > 2.0)
  {
    if (offtrack_error == 1000000) offtrack_error = avgError;
    if (offtrack_error > 0) {
      servo.write(116);
     // Serial.print(" Turn R");
    }
    else
    {
      servo.write(56); 
      //Serial.print(" Turn L");
    }
  }
  else {
    offtrack_error = 1000000;
  }
}


void setup() {
  
  // put your setup code here, to run once:
  pinMode(SI,OUTPUT);
  pinMode(CL,OUTPUT);
  pinMode(AO,INPUT);
  pinMode(pinkD,INPUT);
  pinMode(pinkP,INPUT);
  pinMode(pinkI,INPUT);
  pinMode(pinSpd, INPUT);
  pinMode(pinTX,INPUT);
  pinMode(13,OUTPUT);
  Serial.begin(9600);
  digitalWrite(SI, 0);
  digitalWrite(CL, 0);
  servo.attach(servoPin);
   // put your setup code here, to run once:
  pinMode(high_side, OUTPUT);
  pinMode(low_side, OUTPUT);
  analogWrite(low_side, 0);
  digitalWrite(13,HIGH);
  //analogWrite(high_side, 30);
}

void loop() {
// time control  
  while (millis() - looptime < max_loop) {}
  looptime = millis();
  
//read Tx 
  ctrlSpd = pulseIn(pinTX, HIGH, 25000);
  ctrlSpd = constrain(map(ctrlSpd, 1160,1830, 0, topspeed),0,topspeed);
  //ctrlSpd -= map(abs(error),0,60,0,turnspeed);
 // Serial.printf("ctrlspd: %d ", ctrlSpd);

  
//default speed when off track
  if (offtrack_error == 1000000)
    analogWrite(high_side, ctrlSpd);
  else
    analogWrite(high_side, min(20, ctrlSpd));
  tempSpd = analogRead(pinSpd);
  
//read speed 
  if (tempSpd > 20 && tempSpd < 1000) {
    avgSpd = 0;
    for (int i=0; i<4; i++) {
      spd[i] = spd[i+1];
      avgSpd += spd[i];
    }
    spd[4] = tempSpd;
    avgSpd += spd[4];
    avgSpd = avgSpd/5;
  }
  //Serial.printf("actual spd: %d ", avgSpd);
// read from camera
  getLine();
  error=64-med;
// error processing 
  avgError = 0;
  if (offtrack_error == 1000000) {
    for (int i = 0; i < 4; i++) {
      error_array[i] = error_array[i+1];
      if (i < 3)
        avgError += error_array[i];
    }
    avgError /= 3;
    error_array[4] = error;
  }
  
  /* PID here
   * Error = Setpoint - Actual
   * Integral = Integral + (Error * dt)
   * Derivative = (Error - Previous_error)/dt
   * Drive = (Error*kP) + (Integral*kI) + (Derivative*kD)
   * Previous_error = Error
   * wait(dt)
   */
//derivative stuff  
  dt = millis() - prev_time;
  derivative = (error - prevError)/dt;
  prevError = error;
  prev_time = millis();

 
 // Serial.print("deriv: ");
 // Serial.print(derivative);
  //kP = ((double)analogRead(pinkP)) / 1023.0 * 5.0;
  //kD = ((double)analogRead(pinkD)) / 1023.0 * 20;
  //kI = ((double) analogRead(pinkI)) / 1023.0 * 50.0;
  //Serial.printf(" kP:%.2f kD:%.2f kI:%.2f ", kP, kD, kI);

//original steering setting:
  if (offtrack_error == 1000000) servo.write(constrain(mid+kP*error+kD*derivative,56,116));
//  Serial.print("Servo write");
 // Serial.print(constrain(mid+kP*error+kD*derivative,56,116));
//geometric steering

  err_el_d=mapfloat(error,-55,55,-5,5);
  //err_el_d=mapfloat(error,5,115,-7,7);
  //err_el_d=mapfloat(error,5,115,-10,10);
 
  alpha=atan(2.0*EL*err_el_d/el_d/el_d)*180.0/pi;
  //Serial.print(" alpha: ");
  //Serial.println(alpha); 

  alphamap=map(alpha,-11.5,11.5,-15,15);
  
  servo.write(constrain(mid+K_alpha*alphamap,56,116));
  
  //Serial.print(' ');  
  //servo.write(map((int)med,0,127,mid + 28, mid - 28));
  //Serial.println(constrain(mid+kP*error+kD*derivative,50,120));
  //Serial.println(map((int)med,0,127,mid + 28 , mid - 28));
}

