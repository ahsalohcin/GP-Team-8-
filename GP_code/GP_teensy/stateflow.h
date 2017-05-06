// STATE FLOW
//************************************************************************
typedef enum {
  PLAY,
  PAUSE  
} state;
state myState = PAUSE;

// for RC controlled state. pause when right stick all the way up
int stateValue;
int REC_STATE = 22;
int recStatePeriod = 2000;// ms
int prevRecStateTime = 0;
state stateCheck();

unsigned long int saturationStart = 0 ; // 
int saturationDuration = 3000; // ms
int saturationThresh = 5; //ft/sec
int saturationResetTime = 5000; // ms
bool counting = 0;

state motorSaturationTimeout(double vError);


