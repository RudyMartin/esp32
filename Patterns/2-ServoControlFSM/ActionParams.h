/**********************************************************************
* Filename    : ActionParams.h
* Description : Struct definition for parameterized hardware actions
* Author      : Next Shift / DSAI Camp
* Modified    : 2025-05-31
**********************************************************************/

#ifndef ACTION_PARAMS_H
#define ACTION_PARAMS_H

struct ActionParams {
  int pin;         // Output pin to control (e.g., servo pin)
  int angle;       // Servo angle or output value
  int duration;    // Duration to hold action in milliseconds
};

#endif