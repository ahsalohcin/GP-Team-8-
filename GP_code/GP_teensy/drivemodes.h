#ifndef drivemodes_h
#define drivemodes_h

// DRIVE MODES
//************************************************************************
typedef enum {
  RC_TH,
  BT_TH,
  AUTO_TH,
} throttleMode;

throttleMode myThrottleMode = RC_TH;

typedef enum {
  SPEED_OL,
  SPEED_CL,
} speedMode;

speedMode mySpeedMode = SPEED_OL;

typedef enum {
  RC_ST,
  AUTO_ST,
} steerMode;

steerMode mySteerMode = RC_ST;

#endif
