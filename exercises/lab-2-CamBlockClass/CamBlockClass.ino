/**********************************************************************
* Filename    : ServoControlFSM.ino (L2 Version)
* Description : Lab 2 FSM using PCA9685 for servo movement
* Author      : Next Shift / DSAI Camp
* Modified    : 2025-05-31
**********************************************************************/

#include <Arduino.h>
#include "servo_utils.h"
#include "ActionParams.h"

// === FSM Types ===
typedef void (*StateHandler)();
typedef void (*LifecycleHook)();

struct StateConfig {
  const char* name;
  StateHandler handler;
  unsigned long duration;
  const char* next;
  LifecycleHook onEnter;
  LifecycleHook onExit;
};

// === FSM State Vars ===
int currentStateIndex = 0;
unsigned long stateStartTime = 0;

// === Servo Action Parameters ===
ActionParams moveTo90 = { 0, 90, 1000 };  // Channel 0, 90 deg
ActionParams moveTo0  = { 0, 0, 1000 };   // Channel 0, 0 deg

// === State Functions ===
void moveTo90State() {
  // No-op, movement handled in onEnter
}

void moveTo0State() {
  // No-op, movement handled in onEnter
}

void moveTo90Enter() {
  moveServoAction(moveTo90);
}

void moveTo0Enter() {
  moveServoAction(moveTo0);
}

// === FSM Config ===
StateConfig states[] = {
  { "MOVE_90", moveTo90State, 2000, "MOVE_0", moveTo90Enter, nullptr },
  { "MOVE_0",  moveTo0State,  2000, "MOVE_90", moveTo0Enter, nullptr }
};
const int NUM_STATES = sizeof(states) / sizeof(StateConfig);

void setup() {
  Serial.begin(115200);
  delay(500);
  setupServoDriver();
  Serial.print("Starting in state: ");
  Serial.println(states[currentStateIndex].name);
  stateStartTime = millis();
  if (states[currentStateIndex].onEnter) states[currentStateIndex].onEnter();
}

void loop() {
  unsigned long now = millis();
  StateConfig current = states[currentStateIndex];
  current.handler();

  if (current.duration > 0 && now - stateStartTime >= current.duration) {
    if (current.onExit) current.onExit();

    for (int i = 0; i < NUM_STATES; i++) {
      if (strcmp(states[i].name, current.next) == 0) {
        currentStateIndex = i;
        break;
      }
    }

    stateStartTime = millis();
    if (states[currentStateIndex].onEnter) states[currentStateIndex].onEnter();
  }
}