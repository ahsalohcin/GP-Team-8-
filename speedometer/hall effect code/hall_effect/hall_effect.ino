//two magnets in wheel. one N outwards, one S outwards. Hall effect sensor is latching. Up when N passes, stays up until S passes and then goes down. Repeats. Interrupt function is called on rising edges, so once per revolution.  

int hallPin = 16;
int hallValue;

double wheelDiameter = 1.8; //inches
double wheelSpeed;
double wheelSpeedCopy;

volatile unsigned long int prevHallTime;

void magnet_detect();

void setup() {
  // put your setup code here, to run once:
  pinMode(hallPin,INPUT);
  attachInterrupt(digitalPinToInterrupt(hallPin), magnet_detect, RISING); //set up interrupt function. should work on any teensy pin. 
  Serial.begin(9600);
  pinMode(13,OUTPUT); // indicator LED showing teensy is on
  digitalWrite(13,HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print(prevHallTime);
  Serial.print(" ");
  Serial.print(millis());
  Serial.print(" ");

  noInterrupts();
  wheelSpeedCopy = wheelSpeed;
  interrupts();
  hallValue = analogRead(hallPin);
  Serial.print(" hallPin: ");
  Serial.print(hallValue);
  Serial.print(" ");
  Serial.print(" wheelSpeed: ");
  Serial.println(wheelSpeedCopy);
  delay(50);
}

void magnet_detect()
{
  wheelSpeed = (wheelDiameter*PI/12)/((millis()-prevHallTime)/1000.0); // converts to ft/sec
  prevHallTime = millis();
}
