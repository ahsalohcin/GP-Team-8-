#ifndef drivemodes_h
#define drivemodes_h

// DRIVE MODES
//************************************************************************
typedef enum {
  THROTTLE_RC,
  THROTTLE_BT,
  THROTTLE_SWITCHES,
  THROTTLE_SWITCHES_CURVATURE,
  THROTTLE_AUTO, // based on current error, measured curvature, or track memory
} throttleMode;

throttleMode myThrottleMode = THROTTLE_SWITCHES;

typedef enum {
  SPEED_OL,
  SPEED_CL,
} speedMode;

speedMode mySpeedMode = SPEED_OL;

// In case gate drivers break.
// if left one breaks, then change mode to MOTOR_REV_HALF and put high side of motor to backup low.
// if right one breaks, don't change mode, and put low side of motor to backup low.
//MOTOR_FWD_HALF is same as fwd full. MOTOR_REV_FULL would never be used, and is the same code as MOTOR_REV_HALF

typedef enum {
  MOTOR_FWD_FULL, 
  MOTOR_REV_HALF,
} motorMode;

motorMode myMotorMode = MOTOR_FWD_FULL;

typedef enum {
  STEER_RC,
  STEER_PP,
  STEER_PID
} steerMode;

steerMode mySteerMode = STEER_PID;

 #endif
