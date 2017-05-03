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

state stateCheck();


